#include "mshv_hypervisor.hpp"
#include "mshv_virtual_cpu.hpp"
#include "mshv_vmexit_handler.hpp"

#include "../x86/control_registers.hpp"
#include "../x86/cpuid.hpp"
#include "../x86/intel_ept.hpp"
#include "../x86/intel_vmx.hpp"
#include "../x86/model_specific_registers.hpp"
#include "../x86/segmentation.hpp"

#include "../microsoft_hv/tlfs.hypercalls.hpp"
#include "../microsoft_hv/tlfs.model_specific_registers.hpp"

#include "../debugging.hpp"

namespace siren::vmx {
    void mshv_vmexit_handler::advance_rip(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
        guest_state->rip += vcpu->get_enlightened_vmcs()->info_vmexit_instruction_length;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::siren_hypercall_echo(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
        guest_state->rax = 'srhv';
        advance_rip(vcpu, guest_state);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::siren_hypercall_turn_off_vm(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept  {
        x86::cr3_t guest_cr3{ .storage = vcpu->get_enlightened_vmcs()->guest_cr3 };

        x86::gdtr_t guest_gdtr{
            .semantics = {
                .padding = {},
                .limit = static_cast<uint16_t>(vcpu->get_enlightened_vmcs()->guest_gdtr_limit),
                .base = vcpu->get_enlightened_vmcs()->guest_gdtr_base
            }
        };

        x86::idtr_t guest_idtr = {
            .semantics = {
                .padding = {},
                .limit = static_cast<uint16_t>(vcpu->get_enlightened_vmcs()->guest_idtr_limit),
                .base = vcpu->get_enlightened_vmcs()->guest_idtr_base
            }
        };

        x86::msr_t<x86::IA32_FS_BASE> guest_ia32_fs_base{ .storage = vcpu->get_enlightened_vmcs()->guest_fs_base };
        x86::msr_t<x86::IA32_GS_BASE> guest_ia32_gs_base{ .storage = vcpu->get_enlightened_vmcs()->guest_gs_base };

        x86::write_cr3(guest_cr3);

        x86::write_gdtr(guest_gdtr);
        x86::write_idtr(guest_idtr);

        // todo: restore other segment register?

        x86::write_msr(guest_ia32_fs_base);
        x86::write_msr(guest_ia32_gs_base);

        advance_rip(vcpu, guest_state);

        x86::vmx_off();

        auto host_cr4 = x86::read_cr4();
        host_cr4.semantics.vmx_enable = 0;
        x86::write_cr4(host_cr4);

        return false;
    }

    //[[nodiscard]]
    //bool mshv_vmexit_handler::siren_hypercall_ept_commit_1gb_page(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
    //    x86::guest_paddr_t guest_page_base{ guest_state->rcx };
    //    x86::guest_paddr_t host_page_base{ guest_state->rdx };
    //    auto flags = std::bit_cast<dynamic_ept::page_flags_t>(static_cast<uint32_t>(guest_state->r8));

    //    guest_state->rax = 
    //        static_cast<mshv_hypervisor&>(vcpu->get_hypervisor())
    //            .get_ept_manager().host_commit_page<x86::for_1gb_page_t>(guest_page_base, host_page_base, flags, true).to_integer();

    //    advance_rip(vcpu, guest_state);
    //    return true;
    //}

    //[[nodiscard]]
    //bool mshv_vmexit_handler::siren_hypercall_ept_commit_2mb_page(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
    //    auto guest_page_base = x86::guest_paddr_t{ guest_state->rcx };
    //    auto host_page_base = x86::guest_paddr_t{ guest_state->rdx };
    //    auto flags = std::bit_cast<dynamic_ept::page_flags_t>(static_cast<uint32_t>(guest_state->r8));

    //    guest_state->rax =
    //        static_cast<mshv_hypervisor&>(vcpu->get_hypervisor())
    //        .get_ept_manager().host_commit_page<x86::for_2mb_page_t>(guest_page_base, host_page_base, flags, true).to_integer();

    //    advance_rip(vcpu, guest_state);
    //    return true;
    //}

    //[[nodiscard]]
    //bool mshv_vmexit_handler::siren_hypercall_ept_commit_4kb_page(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
    //    auto guest_page_base = x86::guest_paddr_t{ guest_state->rcx };
    //    auto host_page_base = x86::guest_paddr_t{ guest_state->rdx };
    //    auto flags = std::bit_cast<dynamic_ept::page_flags_t>(static_cast<uint32_t>(guest_state->r8));

    //    guest_state->rax =
    //        static_cast<mshv_hypervisor&>(vcpu->get_hypervisor())
    //            .get_ept_manager().host_commit_page<x86::for_4kb_page_t>(guest_page_base, host_page_base, flags, true).to_integer();

    //    advance_rip(vcpu, guest_state);
    //    return true;
    //}

    //[[nodiscard]]
    //bool mshv_vmexit_handler::siren_hypercall_ept_uncommit_1gb_page(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
    //    auto guest_page_base = x86::guest_paddr_t{ guest_state->rcx };

    //    guest_state->rax =
    //        static_cast<mshv_hypervisor&>(vcpu->get_hypervisor())
    //            .get_ept_manager().host_uncommit_page<x86::for_1gb_page_t>(guest_page_base).to_integer();

    //    advance_rip(vcpu, guest_state);
    //    return true;
    //}

    //[[nodiscard]]
    //bool mshv_vmexit_handler::siren_hypercall_ept_uncommit_2mb_page(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
    //    auto guest_page_base = x86::guest_paddr_t{ guest_state->rcx };

    //    guest_state->rax =
    //        static_cast<mshv_hypervisor&>(vcpu->get_hypervisor())
    //            .get_ept_manager().host_uncommit_page<x86::for_2mb_page_t>(guest_page_base).to_integer();

    //    advance_rip(vcpu, guest_state);
    //    return true;
    //}

    //[[nodiscard]]
    //bool mshv_vmexit_handler::siren_hypercall_ept_uncommit_4kb_page(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
    //    auto guest_page_base = x86::guest_paddr_t{ guest_state->rcx };

    //    guest_state->rax =
    //        static_cast<mshv_hypervisor&>(vcpu->get_hypervisor()).get_ept_manager()
    //            .host_uncommit_page<x86::for_4kb_page_t>(guest_page_base).to_integer();

    //    advance_rip(vcpu, guest_state);
    //    return true;
    //}

    //[[nodiscard]]
    //bool mshv_vmexit_handler::siren_hypercall_ept_flush(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
    //    auto pml4_physical_address = static_cast<mshv_hypervisor_t&>(vcpu->get_hypervisor()).get_ept_manager().get_pml4_physical_address();

    //    if (microsoft_hv::hypercalls::flush_guest_physical_address_space(pml4_physical_address).semantics.result != microsoft_hv::hypercalls::status_code_e::HV_STATUS_SUCCESS) {
    //        vcpu->inject_gp_exception();
    //    }

    //    advance_rip(vcpu, guest_state);
    //    return true;
    //}

    [[nodiscard]]
    bool mshv_vmexit_handler::siren_hypercall_not_implemented(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
        guest_state->rax = nt_status_not_implemented_v.value;
        advance_rip(vcpu, guest_state);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::dispatch(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
        using namespace siren::x86;

        guest_state->rsp = vcpu->get_enlightened_vmcs()->guest_rsp;
        guest_state->rip = vcpu->get_enlightened_vmcs()->guest_rip;
        guest_state->rflags = rflags_t{ .storage = vcpu->get_enlightened_vmcs()->guest_rflags };

        bool resume;
        vmcsf_t<VMCSF_INFO_EXIT_REASON> info_exit_reason{ .storage = vcpu->get_enlightened_vmcs()->info_exit_reason };
        
        if (info_exit_reason.semantics.vm_entry_failure == 0) {
            switch (info_exit_reason.semantics.basic_exit_reason) {
                case x86::vmx_exit_reason_e::CR_ACCESS:
                    return on_cr_access(vcpu, guest_state);
                case x86::vmx_exit_reason_e::INSTRUCTION_VMCALL:
                    resume = on_instruction_vmcall(vcpu, guest_state); break;
                case x86::vmx_exit_reason_e::INSTRUCTION_CPUID:
                    resume = on_instruction_cpuid(vcpu, guest_state); break;
                case x86::vmx_exit_reason_e::INSTRUCTION_HLT:
                    resume = on_instruction_hlt(vcpu, guest_state); break;
                //case x86::vmx_exit_reason_e::INSTRUCTION_INVLPG:
                //    return handle_invlpg(vcpu, guest_state);
                //case x86::vmx_exit_reason_e::INSTRUCTION_WBINVD_OR_WBNOINVD:
                //    return handle_wbinvd(vcpu, guest_state);
                case x86::vmx_exit_reason_e::INSTRUCTION_RDMSR:
                    resume = on_instruction_rdmsr(vcpu, guest_state); break;
                case x86::vmx_exit_reason_e::INSTRUCTION_WRMSR:
                    resume = on_instruction_wrmsr(vcpu, guest_state); break;
                default: 
                    invoke_debugger();
                    advance_rip(vcpu, guest_state);
                    resume = true;
                    break;
            }
        } else {
            invoke_debugger();
            advance_rip(vcpu, guest_state);
            resume = true;
        }

        if (resume) {
            if (vcpu->get_enlightened_vmcs()->guest_rsp != guest_state->rsp) {
                vcpu->get_enlightened_vmcs()->guest_rsp = guest_state->rsp;
                vcpu->get_enlightened_vmcs()->mshv_clean_fields.semantics.guest_basic = 0;
            }

            vcpu->get_enlightened_vmcs()->guest_rip = guest_state->rip;

            if (vcpu->get_enlightened_vmcs()->guest_rflags != guest_state->rflags.storage) {
                vcpu->get_enlightened_vmcs()->guest_rflags = guest_state->rflags.storage;
                vcpu->get_enlightened_vmcs()->mshv_clean_fields.semantics.guest_basic = 0;
            }
        }

        return resume;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::on_cr_access(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
        using namespace siren::x86;

        vmcsf_t<VMCSF_INFO_EXIT_QUALIFICATION> info_exit_qualification
            { .storage = vcpu->get_enlightened_vmcs()->info_exit_qualification };

        if (info_exit_qualification.semantics.cr_access.cr_number == 4 && info_exit_qualification.semantics.cr_access.access_type == 0) { // MOV to CR4
            cr4_t old_cr4;
            cr4_t new_cr4;

            old_cr4.storage = vcpu->get_enlightened_vmcs()->guest_cr4;

            switch (info_exit_qualification.semantics.cr_access.related_gpr) {
                case 0: new_cr4.storage = guest_state->rax; break;
                case 1: new_cr4.storage = guest_state->rcx; break;
                case 2: new_cr4.storage = guest_state->rdx; break;
                case 3: new_cr4.storage = guest_state->rbx; break;
                case 4: new_cr4.storage = guest_state->rsp; break;
                case 5: new_cr4.storage = guest_state->rbp; break;
                case 6: new_cr4.storage = guest_state->rsi; break;
                case 7: new_cr4.storage = guest_state->rdi; break;
                case 8: new_cr4.storage = guest_state->r8; break;
                case 9: new_cr4.storage = guest_state->r9; break;
                case 10: new_cr4.storage = guest_state->r10; break;
                case 11: new_cr4.storage = guest_state->r11; break;
                case 12: new_cr4.storage = guest_state->r12; break;
                case 13: new_cr4.storage = guest_state->r13; break;
                case 14: new_cr4.storage = guest_state->r14; break;
                case 15: new_cr4.storage = guest_state->r15; break;
                default: std::unreachable();
            }

            // A MOV to CR4 instruction that modifies the CR4.PGE (global page enable) bit, the CR4.PSE (page
            // size extensions) bit, or CR4.PAE (page address extensions) bit invalidates all translations (global
            // and non-global) within the processor¡¯s TLB.

            if (auto cpu_index = vcpu->get_index(); cpu_index < 64) {
                auto result_value = microsoft_hv::hypercalls::flush_virtual_address_space(0, microsoft_hv::flush_flags_t{ .semantics = { .all_virtual_address_spaces = 1 } }, uint64_t{ 1 } << cpu_index);
                if (result_value.semantics.result != microsoft_hv::hypercalls::status_code_e::HV_STATUS_SUCCESS) {
                    vcpu->inject_gp_exception();
                }
            } else {
                invoke_debugger();  // todo
            }

            vcpu->get_enlightened_vmcs()->guest_cr4 = new_cr4.storage;
            vcpu->get_enlightened_vmcs()->ctrl_cr4_read_shadow = new_cr4.storage;
            vcpu->get_enlightened_vmcs()->mshv_clean_fields.semantics.crdr = 0;
        } else {
            invoke_debugger();  // todo
        }

        advance_rip(vcpu, guest_state);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::on_instruction_cpuid(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
        auto eax = static_cast<uint32_t>(guest_state->rax);
        auto ecx = static_cast<uint32_t>(guest_state->rcx);

        auto cpuid_result = x86::cpuid(eax, ecx);
        if (eax == 0x40000000) {
            // cpuid_result.semantics.eax = 0x40000001;
            cpuid_result.semantics.ebx = 'eris';
            cpuid_result.semantics.ecx = 'vh-n';
            cpuid_result.semantics.edx = '\x00\x00\x00\x00';
        }

        guest_state->rax = cpuid_result.semantics.eax;
        guest_state->rbx = cpuid_result.semantics.ebx;
        guest_state->rcx = cpuid_result.semantics.ecx;
        guest_state->rdx = cpuid_result.semantics.edx;

        advance_rip(vcpu, guest_state);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::on_instruction_rdmsr(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
        uint32_t msr_address = static_cast<uint32_t>(guest_state->rcx);
        uint64_t msr_value = 0;

        if (msr_bitmap::low_msr_address_interval_v.contains(msr_address)) {
            invoke_debugger();  // should not happen, since we cleared msr bitmap
        } else if (msr_bitmap::high_msr_address_interval_v.contains(msr_address)) {
            invoke_debugger();  // should not happen, since we cleared msr bitmap
        } else {
            msr_value = x86::read_msr(msr_address);
        }

        guest_state->rax = static_cast<uint32_t>(msr_value);
        guest_state->rdx = static_cast<uint32_t>(msr_value >> 32u);

        advance_rip(vcpu, guest_state);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::on_instruction_wrmsr(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
        auto msr_address = static_cast<uint32_t>(guest_state->rcx);
        auto msr_value = (guest_state->rdx << 32u) | (guest_state->rax & 0xffffffffu);

        if (msr_bitmap::low_msr_address_interval_v.contains(msr_address)) {
            invoke_debugger();  // should not happen, since we cleared msr bitmap
        } else if (msr_bitmap::high_msr_address_interval_v.contains(msr_address)) {
            invoke_debugger();  // should not happen, since we cleared msr bitmap
        } else {
            x86::write_msr(msr_address, msr_value);
        }

        advance_rip(vcpu, guest_state);
        return true;
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::on_instruction_vmcall(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
        using namespace siren::x86;

        segment_selector_t guest_cs_selector{ .storage = vcpu->get_enlightened_vmcs()->guest_cs_selector };
        if (guest_cs_selector.semantics.rpl == 0) {
            // eax -> magic number
            // rbx -> function index
            if (guest_state->rax == 'vhrs') {
                switch (guest_state->rbx) {
                    case 0:
                        return siren_hypercall_echo(vcpu, guest_state);
                    case 1:
                        return siren_hypercall_turn_off_vm(vcpu, guest_state);
                    //case 2:
                    //    return siren_hypercall_ept_commit_1gb_page(vcpu, guest_state);
                    //case 3:
                    //    return siren_hypercall_ept_commit_2mb_page(vcpu, guest_state);
                    //case 4:
                    //    return siren_hypercall_ept_commit_4kb_page(vcpu, guest_state);
                    //case 5:
                    //    return siren_hypercall_ept_uncommit_1gb_page(vcpu, guest_state);
                    //case 6:
                    //    return siren_hypercall_ept_uncommit_2mb_page(vcpu, guest_state);
                    //case 7:
                    //    return siren_hypercall_ept_uncommit_4kb_page(vcpu, guest_state);
                    //case 8:
                    //    return siren_hypercall_ept_flush(vcpu, guest_state);
                    default:
                        return siren_hypercall_not_implemented(vcpu, guest_state);
                }
            } else {
                auto input_value = microsoft_hv::hypercalls::input_value_t{ .storage = guest_state->rcx };
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

                    input_output_block.rdx = guest_state->rdx;
                    input_output_block.r8 = guest_state->r8;
                    input_output_block.xmm0 = guest_state->xmm0;
                    input_output_block.xmm1 = guest_state->xmm1;
                    input_output_block.xmm2 = guest_state->xmm2;
                    input_output_block.xmm3 = guest_state->xmm3;
                    input_output_block.xmm4 = guest_state->xmm4;
                    input_output_block.xmm5 = guest_state->xmm5;

                    guest_state->rax = microsoft_fast_hypercall_ex(vcpu->get_hypercall_page(), input_value, &input_output_block).storage;

                    guest_state->rdx = input_output_block.rdx;
                    guest_state->r8 = input_output_block.r8;
                    guest_state->xmm0 = input_output_block.xmm0;
                    guest_state->xmm1 = input_output_block.xmm1;
                    guest_state->xmm2 = input_output_block.xmm2;
                    guest_state->xmm3 = input_output_block.xmm3;
                    guest_state->xmm4 = input_output_block.xmm4;
                    guest_state->xmm5 = input_output_block.xmm5;
                } else {
                    guest_state->rax = microsoft_hypercall(vcpu->get_hypercall_page(), input_value, guest_state->rdx, guest_state->r8).storage;
                }

                advance_rip(vcpu, guest_state);
                return true;
            }
        } else {
            vcpu->inject_ud_exception(); // don't inject gp exception, because we want to hide virtualization for ring3 processes.
            return true;
        }
    }

    [[nodiscard]]
    bool mshv_vmexit_handler::on_instruction_hlt(mshv_virtual_cpu* vcpu, guest_state_t* guest_state) noexcept {
        // A partition which possesses the AccessGuestIdleMsr privilege (refer to section 4.2.2) may trigger entry
        // into the virtual processor idle sleep state through a read to the hypervisor-defined MSR HV_X64_MSR_GUEST_IDLE.
        // The virtual processor will be woken when an interrupt arrives, regardless of whether the interrupt is enabled on the virtual processor or not.
        [[maybe_unused]]
        auto _ = x86::read_msr<microsoft_hv::HV_X64_MSR_GUEST_IDLE>();

        advance_rip(vcpu, guest_state);
        return true;
    }

}
