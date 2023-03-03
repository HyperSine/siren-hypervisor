#include "mshv_hypervisor.hpp"
#include "mshv_virtual_cpu.hpp"
#include "mshv_vmexit_handler.hpp"

#include "../../x86/cpuid.hpp"
#include "../../x86/segmentation.hpp"
#include "../../x86/control_registers.hpp"
#include "../../x86/model_specific_registers.hpp"
#include "../../x86/intel_vmx.hpp"

#include "../../microsoft_hv/tlfs.hypercalls.hpp"
#include "../../microsoft_hv/tlfs.model_specific_registers.hpp"

namespace siren::hypervisors::vmx {

    void mshv_vmexit_handler::advance_rip(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        guest_registers->rip += 
            static_cast<mshv_virtual_cpu*>(virtual_cpu)->get_enlightened_vmcs()->info_vmexit_instruction_length;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::siren_hypercall_echo(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        guest_registers->rax = 'srhv';
        advance_rip(virtual_cpu, guest_registers);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::siren_hypercall_turn_off_vm(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept  {
        auto guest_cr3 = x86::cr3_t{ .storage = virtual_cpu->get_enlightened_vmcs()->guest_cr3 };

        auto guest_gdtr = x86::gdtr_t{
            .semantics = {
                .padding = {},
                .limit = static_cast<uint16_t>(virtual_cpu->get_enlightened_vmcs()->guest_gdtr_limit),
                .base = virtual_cpu->get_enlightened_vmcs()->guest_gdtr_base
            }
        };

        auto guest_idtr = x86::idtr_t{
            .semantics = {
                .padding = {},
                .limit = static_cast<uint16_t>(virtual_cpu->get_enlightened_vmcs()->guest_idtr_limit),
                .base = virtual_cpu->get_enlightened_vmcs()->guest_idtr_base
            }
        };

        auto guest_ia32_fs_base = x86::msr_t<x86::IA32_FS_BASE>{ .storage = virtual_cpu->get_enlightened_vmcs()->guest_fs_base };
        auto guest_ia32_gs_base = x86::msr_t<x86::IA32_GS_BASE>{ .storage = virtual_cpu->get_enlightened_vmcs()->guest_gs_base };

        x86::write_cr3(guest_cr3);

        x86::write_gdtr(guest_gdtr);
        x86::write_idtr(guest_idtr);

        // todo: restore other segment register?

        x86::write_msr(guest_ia32_fs_base);
        x86::write_msr(guest_ia32_gs_base);

        advance_rip(virtual_cpu, guest_registers);

        x86::vmx_off();

        auto host_cr4 = x86::read_cr4();
        host_cr4.semantics.vmx_enable = 0;

        x86::write_cr4(host_cr4);

        return false;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::siren_hypercall_ept_commit_1gb_page(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        auto guest_page_base = x86::guest_physical_address_t{ guest_registers->rcx };
        auto host_page_base = x86::guest_physical_address_t{ guest_registers->rdx };
        auto flags = std::bit_cast<dynamic_ept::page_flags_t>(static_cast<uint32_t>(guest_registers->r8));

        guest_registers->rax = 
            static_cast<mshv_hypervisor&>(virtual_cpu->get_hypervisor())
                .get_ept_manager().host_commit_page<x86::for_1gb_page_t>(guest_page_base, host_page_base, flags, true).to_integer();

        advance_rip(virtual_cpu, guest_registers);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::siren_hypercall_ept_commit_2mb_page(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        auto guest_page_base = x86::guest_physical_address_t{ guest_registers->rcx };
        auto host_page_base = x86::guest_physical_address_t{ guest_registers->rdx };
        auto flags = std::bit_cast<dynamic_ept::page_flags_t>(static_cast<uint32_t>(guest_registers->r8));

        guest_registers->rax =
            static_cast<mshv_hypervisor&>(virtual_cpu->get_hypervisor())
            .get_ept_manager().host_commit_page<x86::for_2mb_page_t>(guest_page_base, host_page_base, flags, true).to_integer();

        advance_rip(virtual_cpu, guest_registers);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::siren_hypercall_ept_commit_4kb_page(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        auto guest_page_base = x86::guest_physical_address_t{ guest_registers->rcx };
        auto host_page_base = x86::guest_physical_address_t{ guest_registers->rdx };
        auto flags = std::bit_cast<dynamic_ept::page_flags_t>(static_cast<uint32_t>(guest_registers->r8));

        guest_registers->rax =
            static_cast<mshv_hypervisor&>(virtual_cpu->get_hypervisor())
                .get_ept_manager().host_commit_page<x86::for_4kb_page_t>(guest_page_base, host_page_base, flags, true).to_integer();

        advance_rip(virtual_cpu, guest_registers);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::siren_hypercall_ept_uncommit_1gb_page(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        auto guest_page_base = x86::guest_physical_address_t{ guest_registers->rcx };

        guest_registers->rax =
            static_cast<mshv_hypervisor&>(virtual_cpu->get_hypervisor())
                .get_ept_manager().host_uncommit_page<x86::for_1gb_page_t>(guest_page_base).to_integer();

        advance_rip(virtual_cpu, guest_registers);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::siren_hypercall_ept_uncommit_2mb_page(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        auto guest_page_base = x86::guest_physical_address_t{ guest_registers->rcx };

        guest_registers->rax =
            static_cast<mshv_hypervisor&>(virtual_cpu->get_hypervisor())
                .get_ept_manager().host_uncommit_page<x86::for_2mb_page_t>(guest_page_base).to_integer();

        advance_rip(virtual_cpu, guest_registers);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::siren_hypercall_ept_uncommit_4kb_page(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        auto guest_page_base = x86::guest_physical_address_t{ guest_registers->rcx };

        guest_registers->rax =
            static_cast<mshv_hypervisor&>(virtual_cpu->get_hypervisor()).get_ept_manager()
                .host_uncommit_page<x86::for_4kb_page_t>(guest_page_base).to_integer();

        advance_rip(virtual_cpu, guest_registers);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::siren_hypercall_ept_flush(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        auto pml4_physical_address =
            static_cast<mshv_hypervisor&>(virtual_cpu->get_hypervisor()).get_ept_manager().get_pml4_physical_address();

        if (microsoft_hv::hypercalls::flush_guest_physical_address_space(pml4_physical_address).semantics.result != microsoft_hv::hypercalls::status_code_e::HV_STATUS_SUCCESS) {
            virtual_cpu->inject_gp_exception();
        }

        advance_rip(virtual_cpu, guest_registers);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::siren_hypercall_not_implemented(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        guest_registers->rax = status_t::not_implemented_v.to_integer();
        advance_rip(virtual_cpu, guest_registers);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::dispatcher(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        guest_registers->rsp = virtual_cpu->get_enlightened_vmcs()->guest_rsp;
        guest_registers->rip = virtual_cpu->get_enlightened_vmcs()->guest_rip;
        guest_registers->rflags = x86::rflags_t{ .storage = virtual_cpu->get_enlightened_vmcs()->guest_rflags };

        bool resume;

        auto info_exit_reason = 
            x86::vmcs_field_value_t<x86::vmcs_field_name_e::INFO_EXIT_REASON>{ .storage = virtual_cpu->get_enlightened_vmcs()->info_exit_reason };
        
        if (info_exit_reason.semantics.vm_entry_failure == 0) {
            switch (info_exit_reason.semantics.basic_exit_reason) {
                case x86::vmx_exit_reason_e::CR_ACCESS:
                    return handle_cr_access(virtual_cpu, guest_registers);
                case x86::vmx_exit_reason_e::INSTRUCTION_VMCALL:
                    resume = handle_vmcall(virtual_cpu, guest_registers); break;
                case x86::vmx_exit_reason_e::INSTRUCTION_CPUID:
                    resume = handle_cpuid(virtual_cpu, guest_registers); break;
                case x86::vmx_exit_reason_e::INSTRUCTION_HLT:
                    resume = handle_hlt(virtual_cpu, guest_registers); break;
                //case x86::vmx_exit_reason_e::INSTRUCTION_INVLPG:
                //    return handle_invlpg(virtual_cpu, guest_registers);
                //case x86::vmx_exit_reason_e::INSTRUCTION_WBINVD_OR_WBNOINVD:
                //    return handle_wbinvd(virtual_cpu, guest_registers);
                case x86::vmx_exit_reason_e::INSTRUCTION_RDMSR:
                    resume = handle_rdmsr(virtual_cpu, guest_registers); break;
                case x86::vmx_exit_reason_e::INSTRUCTION_WRMSR:
                    resume = handle_wrmsr(virtual_cpu, guest_registers); break;
                default: 
                    invoke_debugger();
                    advance_rip(virtual_cpu, guest_registers);
                    resume = true;
                    break;
            }
        } else {
            invoke_debugger();
            advance_rip(virtual_cpu, guest_registers);
            resume = true;
        }

        if (resume) {
            if (virtual_cpu->get_enlightened_vmcs()->guest_rsp != guest_registers->rsp) {
                virtual_cpu->get_enlightened_vmcs()->guest_rsp = guest_registers->rsp;
                virtual_cpu->get_enlightened_vmcs()->mshv_clean_fields.semantics.guest_basic = 0;
            }

            virtual_cpu->get_enlightened_vmcs()->guest_rip = guest_registers->rip;

            if (virtual_cpu->get_enlightened_vmcs()->guest_rflags != guest_registers->rflags.storage) {
                virtual_cpu->get_enlightened_vmcs()->guest_rflags = guest_registers->rflags.storage;
                virtual_cpu->get_enlightened_vmcs()->mshv_clean_fields.semantics.guest_basic = 0;
            }
        }

        return resume;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::handle_cr_access(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        auto info_exit_qualification = 
            x86::vmcs_field_value_t<x86::vmcs_field_name_e::INFO_EXIT_QUALIFICATION>{ .storage = virtual_cpu->get_enlightened_vmcs()->info_exit_qualification };

        if (info_exit_qualification.semantics.cr_access.cr_number == 4 && info_exit_qualification.semantics.cr_access.access_type == 0) { // MOV to CR4
            x86::cr4_t old_cr4;
            x86::cr4_t new_cr4;

            old_cr4.storage = virtual_cpu->get_enlightened_vmcs()->guest_cr4;

            switch (info_exit_qualification.semantics.cr_access.related_gpr) {
                case 0: new_cr4.storage = guest_registers->rax; break;
                case 1: new_cr4.storage = guest_registers->rcx; break;
                case 2: new_cr4.storage = guest_registers->rdx; break;
                case 3: new_cr4.storage = guest_registers->rbx; break;
                case 4: new_cr4.storage = guest_registers->rsp; break;
                case 5: new_cr4.storage = guest_registers->rbp; break;
                case 6: new_cr4.storage = guest_registers->rsi; break;
                case 7: new_cr4.storage = guest_registers->rdi; break;
                case 8: new_cr4.storage = guest_registers->r8; break;
                case 9: new_cr4.storage = guest_registers->r9; break;
                case 10: new_cr4.storage = guest_registers->r10; break;
                case 11: new_cr4.storage = guest_registers->r11; break;
                case 12: new_cr4.storage = guest_registers->r12; break;
                case 13: new_cr4.storage = guest_registers->r13; break;
                case 14: new_cr4.storage = guest_registers->r14; break;
                case 15: new_cr4.storage = guest_registers->r15; break;
                default: SIREN_UNREACHABLE();
            }

            // A MOV to CR4 instruction that modifies the CR4.PGE (global page enable) bit, the CR4.PSE (page
            // size extensions) bit, or CR4.PAE (page address extensions) bit invalidates all translations (global
            // and non-global) within the processor¡¯s TLB.

            if (auto cpu_index = virtual_cpu->get_index(); cpu_index < 64) {
                auto result_value = microsoft_hv::hypercalls::flush_virtual_address_space(0, microsoft_hv::flush_flags_t{ .semantics = { .all_virtual_address_spaces = 1 } }, uint64_t{ 1 } << cpu_index);
                if (result_value.semantics.result != microsoft_hv::hypercalls::status_code_e::HV_STATUS_SUCCESS) {
                    virtual_cpu->inject_gp_exception();
                }
            } else {
                invoke_debugger();  // todo
            }

            virtual_cpu->get_enlightened_vmcs()->guest_cr4 = new_cr4.storage;
            virtual_cpu->get_enlightened_vmcs()->ctrl_cr4_read_shadow = new_cr4.storage;
            virtual_cpu->get_enlightened_vmcs()->mshv_clean_fields.semantics.crdr = 0;
        } else {
            invoke_debugger();  // todo
        }

        advance_rip(virtual_cpu, guest_registers);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::handle_cpuid(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        auto eax = static_cast<uint32_t>(guest_registers->rax);
        auto ecx = static_cast<uint32_t>(guest_registers->rcx);

        auto cpuid_result = x86::cpuid(eax, ecx);
        if (eax == 0x40000000) {
            // cpuid_result.semantics.eax = 0x40000001;
            cpuid_result.semantics.ebx = 'eris';
            cpuid_result.semantics.ecx = 'vh-n';
            cpuid_result.semantics.edx = '\x00\x00\x00\x00';
        }

        guest_registers->rax = cpuid_result.semantics.eax;
        guest_registers->rbx = cpuid_result.semantics.ebx;
        guest_registers->rcx = cpuid_result.semantics.ecx;
        guest_registers->rdx = cpuid_result.semantics.edx;

        advance_rip(virtual_cpu, guest_registers);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::handle_rdmsr(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        auto msr_address = static_cast<x86::msr_address_t>(guest_registers->rcx);
        x86::msr_storage_t msr_value = 0;

        if (msr_address <= 0x00001fffu) {
            invoke_debugger();  // should not happen, since we cleared msr bitmap
        } else if (0xc0000000u <= msr_address && msr_address <= 0xc0001fff) {
            invoke_debugger();  // should not happen, since we cleared msr bitmap
        } else {
            msr_value = x86::read_msr(msr_address);
        }

        guest_registers->rax = static_cast<uint32_t>(msr_value);
        guest_registers->rdx = static_cast<uint32_t>(msr_value >> 32u);

        advance_rip(virtual_cpu, guest_registers);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::handle_wrmsr(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        auto msr_address = static_cast<x86::msr_address_t>(guest_registers->rcx);
        auto msr_value = (guest_registers->rdx << 32u) | (guest_registers->rax & 0xffffffffu);

        if (msr_address <= 0x00001fffu) {
            invoke_debugger();  // should not happen, since we cleared msr bitmap
        } else if (0xc0000000u <= msr_address && msr_address <= 0xc0001fff) {
            invoke_debugger();  // should not happen, since we cleared msr bitmap
        } else {
            x86::write_msr(msr_address, msr_value);
        }

        advance_rip(virtual_cpu, guest_registers);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::handle_vmcall(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        auto guest_cs_selector = x86::segment_selector_t{ .storage = virtual_cpu->get_enlightened_vmcs()->guest_cs_selector };

        if (guest_cs_selector.semantics.rpl == 0) {
            // eax -> magic number
            // rbx -> function index
            if (guest_registers->rax == 'vhrs') {
                switch (guest_registers->rbx) {
                    case 0:
                        return siren_hypercall_echo(virtual_cpu, guest_registers);
                    case 1:
                        return siren_hypercall_turn_off_vm(virtual_cpu, guest_registers);
                    case 2:
                        return siren_hypercall_ept_commit_1gb_page(virtual_cpu, guest_registers);
                    case 3:
                        return siren_hypercall_ept_commit_2mb_page(virtual_cpu, guest_registers);
                    case 4:
                        return siren_hypercall_ept_commit_4kb_page(virtual_cpu, guest_registers);
                    case 5:
                        return siren_hypercall_ept_uncommit_1gb_page(virtual_cpu, guest_registers);
                    case 6:
                        return siren_hypercall_ept_uncommit_2mb_page(virtual_cpu, guest_registers);
                    case 7:
                        return siren_hypercall_ept_uncommit_4kb_page(virtual_cpu, guest_registers);
                    case 8:
                        return siren_hypercall_ept_flush(virtual_cpu, guest_registers);
                    default:
                        return siren_hypercall_not_implemented(virtual_cpu, guest_registers);
                }
            } else {
                auto input_value = microsoft_hv::hypercalls::input_value_t{ .storage = guest_registers->rcx };
                auto result_value = microsoft_hv::hypercalls::result_value_t{};

                if (input_value.semantics.fast) {
                    struct {
                        uint64_t rdx;
                        uint64_t r8;
                        x86::xmm_t xmm0;
                        x86::xmm_t xmm1;
                        x86::xmm_t xmm2;
                        x86::xmm_t xmm3;
                        x86::xmm_t xmm4;
                        x86::xmm_t xmm5;
                    } input_output_block;

                    input_output_block.rdx = guest_registers->rdx;
                    input_output_block.r8 = guest_registers->r8;
                    input_output_block.xmm0 = guest_registers->xmm0;
                    input_output_block.xmm1 = guest_registers->xmm1;
                    input_output_block.xmm2 = guest_registers->xmm2;
                    input_output_block.xmm3 = guest_registers->xmm3;
                    input_output_block.xmm4 = guest_registers->xmm4;
                    input_output_block.xmm5 = guest_registers->xmm5;

                    guest_registers->rax = microsoft_fast_hypercall_ex(virtual_cpu->get_hypercall_page(), input_value, &input_output_block).storage;

                    guest_registers->rdx = input_output_block.rdx;
                    guest_registers->r8 = input_output_block.r8;
                    guest_registers->xmm0 = input_output_block.xmm0;
                    guest_registers->xmm1 = input_output_block.xmm1;
                    guest_registers->xmm2 = input_output_block.xmm2;
                    guest_registers->xmm3 = input_output_block.xmm3;
                    guest_registers->xmm4 = input_output_block.xmm4;
                    guest_registers->xmm5 = input_output_block.xmm5;
                } else {
                    guest_registers->rax = microsoft_hypercall(virtual_cpu->get_hypercall_page(), input_value, guest_registers->rdx, guest_registers->r8).storage;
                }

                advance_rip(virtual_cpu, guest_registers);
                return true;
            }
        } else {
            virtual_cpu->inject_ud_exception(); // don't inject gp exception, because we want to hide virtualization for ring3 processes.
            return true;
        }        
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::handle_hlt(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept {
        // A partition which possesses the AccessGuestIdleMsr privilege (refer to section 4.2.2) may trigger entry
        // into the virtual processor idle sleep state through a read to the hypervisor-defined MSR HV_X64_MSR_GUEST_IDLE.
        // The virtual processor will be woken when an interrupt arrives, regardless of whether the interrupt is enabled on the virtual processor or not.
        static_cast<void>(x86::read_msr(microsoft_hv::HV_X64_MSR_GUEST_IDLE));
        advance_rip(virtual_cpu, guest_registers);
        return true;
    }

}
