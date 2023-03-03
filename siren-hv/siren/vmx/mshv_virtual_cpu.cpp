#include "mshv_hypervisor.hpp"
#include "mshv_virtual_cpu.hpp"
#include "mshv_vmexit_handler.hpp"

#include "../../x86/segmentation.hpp"
#include "../../x86/control_registers.hpp"
#include "../../x86/debug_registers.hpp"
#include "../../x86/paging.hpp"

#include "../../x86/model_specific_registers.hpp"
#include "../../microsoft_hv/tlfs.model_specific_registers.hpp"

#include "../../x86/intel_vmx.hpp"
#include "../../x86/multiprocessor.hpp"
#include "../../x86/interrupts_and_excpetions.hpp"

#include "siren_hypercalls.hpp"

namespace siren::hypervisors::vmx {

#pragma warning(push)
#pragma warning(disable: 6011)   // disable IntelliSense false warning `C6011: Dereferencing NULL pointer`
    template<x86::segment_register_e _SegmentReg>
    static void evmcs_setup_segment(const x86::gdtr_t& gdtr, const x86::segment_selector_t& ldtr, auto seg_selector, auto seg_base, auto seg_limit, auto seg_access_rights) noexcept {
        if (seg_selector || seg_base || seg_limit || seg_access_rights) {
            auto segment_selector = _SegmentReg == x86::segment_register_e::LDTR ? ldtr : x86::read_segment_selector<_SegmentReg>();

            if constexpr (!std::is_null_pointer_v<decltype(seg_selector)>) {
                seg_selector->storage = segment_selector.storage;
            }

            if (segment_selector.semantics.index == 0 && segment_selector.semantics.table_indicator == 0) { // if null selector
                if constexpr (!std::is_null_pointer_v<decltype(seg_base)>) {
                    seg_base->storage = 0;
                }
                if constexpr (!std::is_null_pointer_v<decltype(seg_limit)>) {
                    seg_limit->storage = 0;
                }
                if constexpr (!std::is_null_pointer_v<decltype(seg_access_rights)>) {
                    *seg_access_rights = { .semantics = { .unusable = 1 } };
                }
            } else {
                if constexpr (!std::is_null_pointer_v<decltype(seg_base)>) {
                    // x64 forces flat memory model.
                    if constexpr (_SegmentReg == x86::segment_register_e::LDTR) {
                        const auto* descriptor_table = reinterpret_cast<const x86::segment_descriptor_t*>(gdtr.semantics.base);
                        seg_base->storage = reinterpret_cast<const x86::segment_descriptor_system_long_t*>(&descriptor_table[segment_selector.semantics.index])->semantics.ldt.base();
                    } else if constexpr (_SegmentReg == x86::segment_register_e::TR) {
                        const auto* descriptor_table = reinterpret_cast<const x86::segment_descriptor_t*>(gdtr.semantics.base);
                        seg_base->storage = reinterpret_cast<const x86::segment_descriptor_system_long_t*>(&descriptor_table[segment_selector.semantics.index])->semantics.tss.base();
                    } else if constexpr (_SegmentReg == x86::segment_register_e::FS) {
                        seg_base->storage = x86::read_msr<x86::IA32_FS_BASE>().semantics.base;
                    } else if constexpr (_SegmentReg == x86::segment_register_e::GS) {
                        seg_base->storage = x86::read_msr<x86::IA32_GS_BASE>().semantics.base;
                    } else {
                        const auto* descriptor_table = reinterpret_cast<const x86::segment_descriptor_t*>(gdtr.semantics.base);

                        if (segment_selector.semantics.table_indicator == 1) {  // use LDT
                            auto ldt_base = reinterpret_cast<const x86::segment_descriptor_system_long_t*>(&descriptor_table[ldtr.semantics.index])->semantics.ldt.base();
                            descriptor_table = reinterpret_cast<const x86::segment_descriptor_t*>(ldt_base);
                        }

                        if constexpr (_SegmentReg == x86::segment_register_e::CS) {
                            seg_base->storage = descriptor_table[segment_selector.semantics.index].semantics.code.base();
                        } else {
                            seg_base->storage = descriptor_table[segment_selector.semantics.index].semantics.data.base();
                        }
                    }
                }

                if constexpr (!std::is_null_pointer_v<decltype(seg_limit)>) {
                    seg_limit->storage = x86::read_segment_unscrambled_limit(segment_selector);
                }

                if constexpr (!std::is_null_pointer_v<decltype(seg_access_rights)>) {
                    seg_access_rights->storage = x86::read_segment_access_rights(segment_selector).storage >> 8u;
                }
            }
        }
    }
#pragma warning(pop)

    void mshv_virtual_cpu::evmcs_setup_guest() noexcept {
        using namespace siren::x86;

        vmcs_field_value_t<vmcs_field_name_e::GUEST_CR0> guest_cr0 = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_CR3> guest_cr3 = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_CR4> guest_cr4 = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_DR7> guest_dr7 = {};

        //vmcs_field_value_t<vmcs_field_name_e::GUEST_RSP> guest_rsp = {};     // will be setup in launch() function.
        //vmcs_field_value_t<vmcs_field_name_e::GUEST_RIP> guest_rip = {};     // will be setup in launch() function.
        vmcs_field_value_t<vmcs_field_name_e::GUEST_RFLAGS> guest_rflags = {};

        vmcs_field_value_t<vmcs_field_name_e::GUEST_CS_SELECTOR> guest_cs_selector = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_SS_SELECTOR> guest_ss_selector = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_DS_SELECTOR> guest_ds_selector = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_ES_SELECTOR> guest_es_selector = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_FS_SELECTOR> guest_fs_selector = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_GS_SELECTOR> guest_gs_selector = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_LDTR_SELECTOR> guest_ldtr_selector = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_TR_SELECTOR> guest_tr_selector = {};

        vmcs_field_value_t<vmcs_field_name_e::GUEST_CS_BASE> guest_cs_base = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_SS_BASE> guest_ss_base = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_DS_BASE> guest_ds_base = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_ES_BASE> guest_es_base = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_FS_BASE> guest_fs_base = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_GS_BASE> guest_gs_base = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_LDTR_BASE> guest_ldtr_base = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_TR_BASE> guest_tr_base = {};

        vmcs_field_value_t<vmcs_field_name_e::GUEST_CS_LIMIT> guest_cs_limit = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_SS_LIMIT> guest_ss_limit = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_DS_LIMIT> guest_ds_limit = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_ES_LIMIT> guest_es_limit = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_FS_LIMIT> guest_fs_limit = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_GS_LIMIT> guest_gs_limit = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_LDTR_LIMIT> guest_ldtr_limit = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_TR_LIMIT> guest_tr_limit = {};

        vmcs_field_value_t<vmcs_field_name_e::GUEST_CS_ACCESS_RIGHTS> guest_cs_access_rights = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_SS_ACCESS_RIGHTS> guest_ss_access_rights = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_DS_ACCESS_RIGHTS> guest_ds_access_rights = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_ES_ACCESS_RIGHTS> guest_es_access_rights = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_FS_ACCESS_RIGHTS> guest_fs_access_rights = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_GS_ACCESS_RIGHTS> guest_gs_access_rights = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_LDTR_ACCESS_RIGHTS> guest_ldtr_access_rights = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_TR_ACCESS_RIGHTS> guest_tr_access_rights = {};

        vmcs_field_value_t<vmcs_field_name_e::GUEST_GDTR_BASE> guest_gdtr_base = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_IDTR_BASE> guest_idtr_base = {};

        vmcs_field_value_t<vmcs_field_name_e::GUEST_GDTR_LIMIT> guest_gdtr_limit = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_IDTR_LIMIT> guest_idtr_limit = {};

        vmcs_field_value_t<vmcs_field_name_e::GUEST_IA32_DEBUGCTL> guest_ia32_debugctl = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_IA32_SYSENTER_CS> guest_ia32_sysenter_cs = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_IA32_SYSENTER_ESP> guest_ia32_sysenter_esp = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_IA32_SYSENTER_EIP> guest_ia32_sysenter_eip = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_IA32_PERF_GLOBAL_CTRL> guest_ia32_perf_global_ctrl = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_IA32_PAT> guest_ia32_pat = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_IA32_EFER> guest_ia32_efer = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_IA32_BNDCFGS> guest_ia32_bndcfgs = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_IA32_RTIT_CTL> guest_ia32_rtit_ctl = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_IA32_LBR_CTL> guest_ia32_lbr_ctl = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_IA32_S_CET> guest_ia32_s_cet = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_IA32_INTERRUPT_SSP_TABLE_ADDR> guest_ia32_interrupt_ssp_table_addr = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_IA32_PKRS> guest_ia32_pkrs = {};

        vmcs_field_value_t<vmcs_field_name_e::GUEST_SSP> guest_ssp = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_SMBASE> guest_smbase = {};

        vmcs_field_value_t<vmcs_field_name_e::GUEST_ACTIVITY_STATE> guest_activity_state = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_INTERRUPTIBILITY_STATE> guest_interruptibility_state = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_PENDING_DEBUG_EXCEPTIONS> guest_pending_debug_exceptions = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_VMCS_LINK_POINTER> guest_vmcs_link_pointer = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_VMX_PREEMPTION_TIMER_VALUE> guest_vmx_preemption_timer_value = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_PDPTE0> guest_pdpte0 = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_PDPTE1> guest_pdpte1 = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_PDPTE2> guest_pdpte2 = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_PDPTE3> guest_pdpte3 = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_INTERRUPT_STATUS> guest_interrupt_status = {};
        vmcs_field_value_t<vmcs_field_name_e::GUEST_PML_INDEX> guest_pml_index = {};

        auto gdtr = read_gdtr();
        auto idtr = read_idtr();
        auto ldtr = read_segment_selector<x86::segment_register_e::LDTR>();

        guest_cr0.storage = read_cr0().storage;
        guest_cr3.storage = read_cr3().storage;
        guest_cr4.storage = read_cr4().storage;
        guest_dr7.storage = read_dr7().storage;
        guest_rflags.storage = __readeflags();

        evmcs_setup_segment<x86::segment_register_e::CS>(gdtr, ldtr, &guest_cs_selector, &guest_cs_base, &guest_cs_limit, &guest_cs_access_rights);
        evmcs_setup_segment<x86::segment_register_e::SS>(gdtr, ldtr, &guest_ss_selector, &guest_ss_base, &guest_ss_limit, &guest_ss_access_rights);
        evmcs_setup_segment<x86::segment_register_e::DS>(gdtr, ldtr, &guest_ds_selector, &guest_ds_base, &guest_ds_limit, &guest_ds_access_rights);
        evmcs_setup_segment<x86::segment_register_e::ES>(gdtr, ldtr, &guest_es_selector, &guest_es_base, &guest_es_limit, &guest_es_access_rights);
        evmcs_setup_segment<x86::segment_register_e::FS>(gdtr, ldtr, &guest_fs_selector, &guest_fs_base, &guest_fs_limit, &guest_fs_access_rights);
        evmcs_setup_segment<x86::segment_register_e::GS>(gdtr, ldtr, &guest_gs_selector, &guest_gs_base, &guest_gs_limit, &guest_gs_access_rights);
        evmcs_setup_segment<x86::segment_register_e::LDTR>(gdtr, ldtr, &guest_ldtr_selector, &guest_ldtr_base, &guest_ldtr_limit, &guest_ldtr_access_rights);
        evmcs_setup_segment<x86::segment_register_e::TR>(gdtr, ldtr, &guest_tr_selector, &guest_tr_base, &guest_tr_limit, &guest_tr_access_rights);

        guest_gdtr_base.storage = gdtr.semantics.base;
        guest_idtr_base.storage = idtr.semantics.base;

        guest_gdtr_limit.storage = gdtr.semantics.limit;
        guest_idtr_limit.storage = idtr.semantics.limit;

        guest_ia32_debugctl.storage = read_msr<IA32_DEBUGCTL>().storage;
        guest_ia32_sysenter_cs.storage = static_cast<uint32_t>(read_msr<IA32_SYSENTER_CS>().storage);
        guest_ia32_sysenter_esp.storage = read_msr<IA32_SYSENTER_ESP>().storage;
        guest_ia32_sysenter_eip.storage = read_msr<IA32_SYSENTER_EIP>().storage;
        guest_ia32_efer.storage = read_msr<IA32_EFER>().storage;

        guest_vmcs_link_pointer.storage = ~x86::physical_address_t{ 0 };

        m_evmcs_region->guest_cr0 = guest_cr0.storage;
        m_evmcs_region->guest_cr3 = guest_cr3.storage;
        m_evmcs_region->guest_cr4 = guest_cr4.storage;
        m_evmcs_region->guest_dr7 = guest_dr7.storage;

        //m_evmcs_region->guest_rsp = guest_rsp.storage;
        //m_evmcs_region->guest_rip = guest_rip.storage;
        m_evmcs_region->guest_rflags = guest_rflags.storage;

        m_evmcs_region->guest_cs_selector = guest_cs_selector.storage;
        m_evmcs_region->guest_ss_selector = guest_ss_selector.storage;
        m_evmcs_region->guest_ds_selector = guest_ds_selector.storage;
        m_evmcs_region->guest_es_selector = guest_es_selector.storage;
        m_evmcs_region->guest_fs_selector = guest_fs_selector.storage;
        m_evmcs_region->guest_gs_selector = guest_gs_selector.storage;
        m_evmcs_region->guest_ldtr_selector = guest_ldtr_selector.storage;
        m_evmcs_region->guest_tr_selector = guest_tr_selector.storage;

        m_evmcs_region->guest_cs_base = guest_cs_base.storage;
        m_evmcs_region->guest_ss_base = guest_ss_base.storage;
        m_evmcs_region->guest_ds_base = guest_ds_base.storage;
        m_evmcs_region->guest_es_base = guest_es_base.storage;
        m_evmcs_region->guest_fs_base = guest_fs_base.storage;
        m_evmcs_region->guest_gs_base = guest_gs_base.storage;
        m_evmcs_region->guest_ldtr_base = guest_ldtr_base.storage;
        m_evmcs_region->guest_tr_base = guest_tr_base.storage;

        m_evmcs_region->guest_cs_limit = guest_cs_limit.storage;
        m_evmcs_region->guest_ss_limit = guest_ss_limit.storage;
        m_evmcs_region->guest_ds_limit = guest_ds_limit.storage;
        m_evmcs_region->guest_es_limit = guest_es_limit.storage;
        m_evmcs_region->guest_fs_limit = guest_fs_limit.storage;
        m_evmcs_region->guest_gs_limit = guest_gs_limit.storage;
        m_evmcs_region->guest_ldtr_limit = guest_ldtr_limit.storage;
        m_evmcs_region->guest_tr_limit = guest_tr_limit.storage;

        m_evmcs_region->guest_cs_attributes = guest_cs_access_rights.storage;
        m_evmcs_region->guest_ss_attributes = guest_ss_access_rights.storage;
        m_evmcs_region->guest_ds_attributes = guest_ds_access_rights.storage;
        m_evmcs_region->guest_es_attributes = guest_es_access_rights.storage;
        m_evmcs_region->guest_fs_attributes = guest_fs_access_rights.storage;
        m_evmcs_region->guest_gs_attributes = guest_gs_access_rights.storage;
        m_evmcs_region->guest_ldtr_attributes = guest_ldtr_access_rights.storage;
        m_evmcs_region->guest_tr_attributes = guest_tr_access_rights.storage;

        m_evmcs_region->guest_gdtr_base = guest_gdtr_base.storage;
        m_evmcs_region->guest_idtr_base = guest_idtr_base.storage;

        m_evmcs_region->guest_gdtr_limit = guest_gdtr_limit.storage;
        m_evmcs_region->guest_idtr_limit = guest_idtr_limit.storage;

        m_evmcs_region->guest_ia32_debug_ctl = guest_ia32_debugctl.storage;
        m_evmcs_region->guest_ia32_sysenter_cs = guest_ia32_sysenter_cs.storage;
        m_evmcs_region->guest_ia32_sysenter_esp = guest_ia32_sysenter_esp.storage;
        m_evmcs_region->guest_ia32_sysenter_eip = guest_ia32_sysenter_eip.storage;
        m_evmcs_region->guest_ia32_efer = guest_ia32_efer.storage;

        m_evmcs_region->guest_vmcs_link_pointer = guest_vmcs_link_pointer.storage;
    }

    void mshv_virtual_cpu::evmcs_setup_host() noexcept {
        using namespace siren::x86;

        vmcs_field_value_t<vmcs_field_name_e::HOST_CR0> host_cr0 = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_CR3> host_cr3 = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_CR4> host_cr4 = {};

        vmcs_field_value_t<vmcs_field_name_e::HOST_RSP> host_rsp = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_RIP> host_rip = {};

        vmcs_field_value_t<vmcs_field_name_e::HOST_CS_SELECTOR> host_cs_selector = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_SS_SELECTOR> host_ss_selector = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_DS_SELECTOR> host_ds_selector = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_ES_SELECTOR> host_es_selector = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_FS_SELECTOR> host_fs_selector = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_GS_SELECTOR> host_gs_selector = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_TR_SELECTOR> host_tr_selector = {};

        vmcs_field_value_t<vmcs_field_name_e::HOST_FS_BASE> host_fs_base = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_GS_BASE> host_gs_base = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_TR_BASE> host_tr_base = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_GDTR_BASE> host_gdtr_base = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_IDTR_BASE> host_idtr_base = {};

        vmcs_field_value_t<vmcs_field_name_e::HOST_IA32_SYSENTER_CS> host_ia32_sysenter_cs = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_IA32_SYSENTER_ESP> host_ia32_sysenter_esp = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_IA32_SYSENTER_EIP> host_ia32_sysenter_eip = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_IA32_PERF_GLOBAL_CTRL> host_ia32_perf_global_ctrl = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_IA32_PAT> host_ia32_pat = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_IA32_EFER> host_ia32_efer = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_IA32_S_CET> host_ia32_s_cet = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_IA32_INTERRUPT_SSP_TABLE_ADDR> host_ia32_interrupt_ssp_table_addr = {};
        vmcs_field_value_t<vmcs_field_name_e::HOST_IA32_PKRS> host_ia32_pkrs = {};

        vmcs_field_value_t<vmcs_field_name_e::HOST_SSP> host_ssp = {};

        auto gdtr = read_gdtr();
        auto idtr = read_idtr();
        auto ldtr = read_segment_selector<x86::segment_register_e::LDTR>();

        host_cr0.storage = read_cr0().storage;
        host_cr3.storage = read_cr3().storage;
        host_cr4.storage = read_cr4().storage;

        constexpr size_t stack_region_base_offset = stack_region_size_v - stack_region_reserved_size_v;

        *reinterpret_cast<mshv_virtual_cpu**>(&m_stack_region[stack_region_base_offset]) = this;
        host_rsp.storage = reinterpret_cast<uintptr_t>(&m_stack_region[stack_region_base_offset]);
        host_rip.storage = reinterpret_cast<uintptr_t>(&mshv_vmexit_handler::entry);

        evmcs_setup_segment<x86::segment_register_e::CS>(gdtr, ldtr, &host_cs_selector, nullptr, nullptr, nullptr);
        evmcs_setup_segment<x86::segment_register_e::SS>(gdtr, ldtr, &host_ss_selector, nullptr, nullptr, nullptr);
        evmcs_setup_segment<x86::segment_register_e::DS>(gdtr, ldtr, &host_ds_selector, nullptr, nullptr, nullptr);
        evmcs_setup_segment<x86::segment_register_e::ES>(gdtr, ldtr, &host_es_selector, nullptr, nullptr, nullptr);
        evmcs_setup_segment<x86::segment_register_e::FS>(gdtr, ldtr, &host_fs_selector, &host_fs_base, nullptr, nullptr);
        evmcs_setup_segment<x86::segment_register_e::GS>(gdtr, ldtr, &host_gs_selector, &host_gs_base, nullptr, nullptr);
        evmcs_setup_segment<x86::segment_register_e::TR>(gdtr, ldtr, &host_tr_selector, &host_tr_base, nullptr, nullptr);
        // ---------------------------------------------------------
        // According to 
        // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
        //  |-> Chapter 26 VM Entries
        //    |-> 26.2 Checks on VMX Controls and Host-State Area
        //      |-> 26.2.3 Checks on Host Segment and Descriptor-Table Registers
        // 
        //          The following checks are performed on fields in the host-state area that correspond to segment and descriptor-table registers:
        //          1. In the selector field for each of CS, SS, DS, ES, FS, GS and TR, the RPL (bits 1:0) and the TI flag (bit 2) must be 0.
        //          2. The selector fields for CS and TR cannot be 0000H.
        //          3. The selector field for SS cannot be 0000H if the "host address-space size" VM-exit control is 0.
        //          4. On processors that support Intel 64 architecture, the base-address fields for FS, GS, GDTR, IDTR, and TR must contain canonical addresses.
        //
        // we shound adjust following host selectors
        host_cs_selector.semantics.rpl = 0;
        host_cs_selector.semantics.table_indicator = 0;

        host_ss_selector.semantics.rpl = 0;
        host_ss_selector.semantics.table_indicator = 0;

        host_ds_selector.semantics.rpl = 0;
        host_ds_selector.semantics.table_indicator = 0;

        host_es_selector.semantics.rpl = 0;
        host_es_selector.semantics.table_indicator = 0;

        host_fs_selector.semantics.rpl = 0;
        host_fs_selector.semantics.table_indicator = 0;

        host_gs_selector.semantics.rpl = 0;
        host_gs_selector.semantics.table_indicator = 0;

        host_tr_selector.semantics.rpl = 0;
        host_tr_selector.semantics.table_indicator = 0;
        // ---------------------------------------------------------
        host_gdtr_base.storage = gdtr.semantics.base;
        host_idtr_base.storage = idtr.semantics.base;

        host_ia32_sysenter_cs.storage = static_cast<uint32_t>(read_msr<IA32_SYSENTER_CS>().storage);
        host_ia32_sysenter_esp.storage = read_msr<IA32_SYSENTER_ESP>().storage;
        host_ia32_sysenter_eip.storage = read_msr<IA32_SYSENTER_EIP>().storage;
        host_ia32_efer.storage = read_msr<IA32_EFER>().storage;

        m_evmcs_region->host_cr0 = host_cr0.storage;
        m_evmcs_region->host_cr3 = host_cr3.storage;
        m_evmcs_region->host_cr4 = host_cr4.storage;

        m_evmcs_region->host_rsp = host_rsp.storage;
        m_evmcs_region->host_rip = host_rip.storage;

        m_evmcs_region->host_cs_selector = host_cs_selector.storage;
        m_evmcs_region->host_ss_selector = host_ss_selector.storage;
        m_evmcs_region->host_ds_selector = host_ds_selector.storage;
        m_evmcs_region->host_es_selector = host_es_selector.storage;
        m_evmcs_region->host_fs_selector = host_fs_selector.storage;
        m_evmcs_region->host_gs_selector = host_gs_selector.storage;
        m_evmcs_region->host_tr_selector = host_tr_selector.storage;

        m_evmcs_region->host_fs_base = host_fs_base.storage;
        m_evmcs_region->host_gs_base = host_gs_base.storage;
        m_evmcs_region->host_tr_base = host_tr_base.storage;
        m_evmcs_region->host_gdtr_base = host_gdtr_base.storage;
        m_evmcs_region->host_idtr_base = host_idtr_base.storage;

        m_evmcs_region->host_ia32_sysenter_cs = host_ia32_sysenter_cs.storage;
        m_evmcs_region->host_ia32_sysenter_esp = host_ia32_sysenter_esp.storage;
        m_evmcs_region->host_ia32_sysenter_eip = host_ia32_sysenter_eip.storage;
        //m_evmcs_region->host_ia32_perf_global_ctrl = host_ia32_perf_global_ctrl.storage;
        //m_evmcs_region->host_ia32_pat = host_ia32_pat.storage;
        m_evmcs_region->host_ia32_efer = host_ia32_efer.storage;
        //m_evmcs_region->host_ia32_s_cet = host_ia32_s_cet.storage;
        //m_evmcs_region->host_ia32_interrupt_ssp_table_addr = host_ia32_interrupt_ssp_table_addr.storage;
        //m_evmcs_region->host_ia32_pkrs = host_ia32_pkrs.storage;

        //m_evmcs_region->host_ssp = host_ssp.storage;
    }

    void mshv_virtual_cpu::evmcs_setup_controls_execution() noexcept {
        using namespace siren::x86;

        vmcs_field_value_t<vmcs_field_name_e::CTRL_PIN_BASED_VM_EXECUTION_CONTROLS> ctrl_pin_based_vm_execution_controls = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS> ctrl_1st_processor_based_vm_execution_controls = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS> ctrl_2nd_processor_based_vm_execution_controls = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_TERTIARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS> ctrl_3rd_processor_based_vm_execution_controls = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_EXCEPTION_BITMAP> ctrl_exception_bitmap = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_IO_BITMAP_A_ADDRESS> ctrl_io_bitmap_a_address = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_IO_BITMAP_B_ADDRESS> ctrl_io_bitmap_b_address = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_TSC_OFFSET> ctrl_tsc_offset = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_TSC_MULTIPLIER> ctrl_tsc_multiplier = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_CR0_GUEST_HOST_MASK> ctrl_cr0_guest_host_mask = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_CR4_GUEST_HOST_MASK> ctrl_cr4_guest_host_mask = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_CR0_READ_SHADOW> ctrl_cr0_read_shadow = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_CR4_READ_SHADOW> ctrl_cr4_read_shadow = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_CR3_TARGET_VALUE0> ctrl_cr3_target_value0 = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_CR3_TARGET_VALUE1> ctrl_cr3_target_value1 = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_CR3_TARGET_VALUE2> ctrl_cr3_target_value2 = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_CR3_TARGET_VALUE3> ctrl_cr3_target_value3 = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_CR3_TARGET_COUNT> ctrl_cr3_target_count = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_APIC_ACCESS_ADDRESS> ctrl_apic_access_address = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_VIRTUAL_APIC_ADDRESS> ctrl_virtual_apic_address = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_TPR_THRESHOLD> ctrl_tpr_threshold = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_EOI_EXIT0_BITMAP> ctrl_eoi_exit0_bitmap = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_EOI_EXIT1_BITMAP> ctrl_eoi_exit1_bitmap = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_EOI_EXIT2_BITMAP> ctrl_eoi_exit2_bitmap = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_EOI_EXIT3_BITMAP> ctrl_eoi_exit3_bitmap = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_POSTED_INTERRUPT_NOTIFICATION_VECTOR> ctrl_posted_interrupt_notification_vector = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_POSTED_INTERRUPT_DESCRIPTOR_ADDRESS> ctrl_posted_interrupt_descriptor_address = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_MSR_BITMAP_ADDRESS> ctrl_msr_bitmap_address = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_EXECUTIVE_VMCS_POINTER> ctrl_executive_vmcs_pointer = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_EPT_POINTER> ctrl_ept_pointer = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_VIRTUAL_PROCESSOR_IDENTIFIER> ctrl_virtual_processor_identifier = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_PLE_GAP> ctrl_ple_gap = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_PLE_WINDOW> ctrl_ple_window = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_VM_FUNCTION_CONTROLS> ctrl_vm_function_controls = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_VMREAD_BITMAP_ADDRESS> ctrl_vmread_bitmap = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_VMWRITE_BITMAP_ADDRESS> ctrl_vmwrite_bitmap = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_ENCLS_EXITING_BITMAP> ctrl_encls_exiting_bitmap = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_ENCLV_EXITING_BITMAP> ctrl_enclv_exiting_bitmap = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_PCONFIG_EXITING_BITMAP> ctrl_pconfig_exiting_bitmap = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_PML_ADDRESS> ctrl_pml_address = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_VIRTUALIZATION_EXCEPTION_INFO_ADDRESS> ctrl_virtualization_exception_info_address = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_EPTP_INDEX> ctrl_eptp_index = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_XSS_EXITING_BITMAP> ctrl_xss_exiting_bitmap = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_SPPTP> ctrl_spptp = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_HLATP> ctrl_hlatp = {};

        ctrl_pin_based_vm_execution_controls.semantics.external_interrupt_exiting = 0;
        ctrl_pin_based_vm_execution_controls.semantics.nmi_exiting = 0;
        ctrl_pin_based_vm_execution_controls.semantics.virtual_nmis = 0;
        ctrl_pin_based_vm_execution_controls.semantics.activate_vmx_preemption_timer = 0;
        ctrl_pin_based_vm_execution_controls.semantics.process_posted_interrupts = 0;

        ctrl_1st_processor_based_vm_execution_controls.semantics.interrupt_window_exiting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.use_tsc_offsetting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.hlt_exiting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.invlpg_exiting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.mwait_exiting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.rdpmc_exiting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.rdtsc_exiting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.cr3_load_exiting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.cr3_store_exiting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.activate_tertiary_controls = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.cr8_load_exiting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.cr8_store_exiting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.use_tpr_shadow = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.nmi_window_exiting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.mov_dr_exiting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.unconditional_io_exiting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.use_io_bitmaps = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.monitor_trap_flag = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.use_msr_bitmaps = 1;
        ctrl_1st_processor_based_vm_execution_controls.semantics.monitor_exiting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.pause_exiting = 0;
        ctrl_1st_processor_based_vm_execution_controls.semantics.activate_secondary_controls = 1;

        ctrl_2nd_processor_based_vm_execution_controls.semantics.virtualize_apic_accesses = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.enable_ept = 1;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.descriptor_table_exiting = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.enable_rdtscp = 1;      // required by Windows 10
        ctrl_2nd_processor_based_vm_execution_controls.semantics.virtualize_x2apic_mode = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.enable_vpid = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.wbinvd_exiting = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.unrestricted_guest = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.apic_register_virtualization = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.virtual_interrupt_delivery = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.pause_loop_exiting = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.rdrand_exiting = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.enable_invpcid = 1;     // required by Windows 10
        ctrl_2nd_processor_based_vm_execution_controls.semantics.enable_vm_functions = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.vmcs_shadowing = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.enable_encls_exiting = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.rdseed_exiting = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.enable_pml = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.raise_ve_exception_when_ept_violation = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.conceal_vmx_from_pt = 1;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.enable_xsave_xrstors = 1;   // required by Windows 10
        ctrl_2nd_processor_based_vm_execution_controls.semantics.mode_based_execute_control_for_ept = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.sub_page_write_permissions_for_ept = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.intel_pt_uses_guest_physical_addresses = 1;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.use_tsc_scaling = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.enable_user_wait_and_pause = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.enable_pconfig = 0;
        ctrl_2nd_processor_based_vm_execution_controls.semantics.enable_enclv_exiting = 0;

        ctrl_cr0_guest_host_mask.storage = 0;
        ctrl_cr0_read_shadow.storage = 0;

        ctrl_cr4_guest_host_mask.storage = cr4_t{ .semantics = { .page_size_extension = 1, .physical_address_extension = 1, .page_global_enable = 1 } }.storage;
        ctrl_cr4_read_shadow.storage = read_cr4().storage;

        ctrl_msr_bitmap_address.storage = m_msr_bitmap_physical_address;

        ctrl_ept_pointer.semantics.memory_type = memory_type_t::write_back_v.to_integral();
        ctrl_ept_pointer.semantics.page_walk_length_minus_one = 4 - 1;
        ctrl_ept_pointer.semantics.enable_accessed_and_dirty_flag = 1;
        ctrl_ept_pointer.semantics.pml4_physical_address = address_to_pfn(m_hypervisor.get_ept_manager().get_pml4_physical_address(), _4kb_page_traits{});

        ctrl_virtual_processor_identifier.semantics.vpid = 1;

        auto ia32_vmx_basic = read_msr<IA32_VMX_BASIC>();
        bool ctrl_2nd_processor_based_vm_execution_controls_support;
        bool ctrl_3rd_processor_based_vm_execution_controls_support;

        if (ia32_vmx_basic.semantics.default1_controls_can_be_zero) {
            auto ia32_vmx_true_pinbased_ctls = read_msr<IA32_VMX_TRUE_PINBASED_CTLS>();
            auto ia32_vmx_true_procbased_ctls = read_msr<IA32_VMX_TRUE_PROCBASED_CTLS>();

            ctrl_2nd_processor_based_vm_execution_controls_support =
                vmcs_field_value_t<vmcs_field_name_e::CTRL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS>{ .storage = ia32_vmx_true_procbased_ctls.semantics.mask1 }.semantics.activate_secondary_controls != 0;

            ctrl_3rd_processor_based_vm_execution_controls_support =
                vmcs_field_value_t<vmcs_field_name_e::CTRL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS>{ .storage = ia32_vmx_true_procbased_ctls.semantics.mask1 }.semantics.activate_tertiary_controls != 0;

            ctrl_pin_based_vm_execution_controls.storage |= ia32_vmx_true_pinbased_ctls.semantics.mask0;
            ctrl_pin_based_vm_execution_controls.storage &= ia32_vmx_true_pinbased_ctls.semantics.mask1;

            ctrl_1st_processor_based_vm_execution_controls.storage |= ia32_vmx_true_procbased_ctls.semantics.mask0;
            ctrl_1st_processor_based_vm_execution_controls.storage &= ia32_vmx_true_procbased_ctls.semantics.mask1;
        } else {
            auto ia32_vmx_pinbased_ctls = read_msr<IA32_VMX_PINBASED_CTLS>();
            auto ia32_vmx_procbased_ctls = read_msr<IA32_VMX_PROCBASED_CTLS>();

            ctrl_2nd_processor_based_vm_execution_controls_support =
                vmcs_field_value_t<vmcs_field_name_e::CTRL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS>{ .storage = ia32_vmx_procbased_ctls.semantics.mask1 }.semantics.activate_secondary_controls != 0;

            ctrl_3rd_processor_based_vm_execution_controls_support =
                vmcs_field_value_t<vmcs_field_name_e::CTRL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS>{ .storage = ia32_vmx_procbased_ctls.semantics.mask1 }.semantics.activate_tertiary_controls != 0;

            ctrl_pin_based_vm_execution_controls.storage |= ia32_vmx_pinbased_ctls.semantics.mask0;
            ctrl_pin_based_vm_execution_controls.storage &= ia32_vmx_pinbased_ctls.semantics.mask1;

            ctrl_1st_processor_based_vm_execution_controls.storage |= ia32_vmx_procbased_ctls.semantics.mask0;
            ctrl_1st_processor_based_vm_execution_controls.storage &= ia32_vmx_procbased_ctls.semantics.mask1;
        }

        if (ctrl_1st_processor_based_vm_execution_controls.semantics.activate_secondary_controls && ctrl_2nd_processor_based_vm_execution_controls_support) {
            auto ia32_vmx_procbased_ctls2 = read_msr<IA32_VMX_PROCBASED_CTLS2>();
            ctrl_2nd_processor_based_vm_execution_controls.storage |= ia32_vmx_procbased_ctls2.semantics.mask0;
            ctrl_2nd_processor_based_vm_execution_controls.storage &= ia32_vmx_procbased_ctls2.semantics.mask1;
        }

        if (ctrl_1st_processor_based_vm_execution_controls.semantics.activate_tertiary_controls && ctrl_3rd_processor_based_vm_execution_controls_support) {
            auto ia32_vmx_procbased_ctls3 = read_msr<IA32_VMX_PROCBASED_CTLS3>();
            ctrl_3rd_processor_based_vm_execution_controls.storage &= ia32_vmx_procbased_ctls3.semantics.mask;
        }

        m_evmcs_region->ctrl_pin_based_vm_execution_controls = ctrl_pin_based_vm_execution_controls.storage;
        m_evmcs_region->ctrl_primary_processor_based_vm_execution_controls = ctrl_1st_processor_based_vm_execution_controls.storage;

        if (ctrl_1st_processor_based_vm_execution_controls.semantics.activate_secondary_controls && ctrl_2nd_processor_based_vm_execution_controls_support) {
            m_evmcs_region->ctrl_secondary_processor_based_vm_execution_controls = ctrl_2nd_processor_based_vm_execution_controls.storage;
        }

        m_evmcs_region->ctrl_cr0_guest_host_mask = ctrl_cr0_guest_host_mask.storage;
        m_evmcs_region->ctrl_cr0_read_shadow = ctrl_cr0_read_shadow.storage;

        m_evmcs_region->ctrl_cr4_guest_host_mask = ctrl_cr4_guest_host_mask.storage;
        m_evmcs_region->ctrl_cr4_read_shadow = ctrl_cr4_read_shadow.storage;

        if (ctrl_1st_processor_based_vm_execution_controls.semantics.use_msr_bitmaps) {
            m_evmcs_region->ctrl_msr_bitmap_address = ctrl_msr_bitmap_address.storage;
        }

        if (ctrl_2nd_processor_based_vm_execution_controls.semantics.enable_ept) {
            m_evmcs_region->ctrl_ept_pointer = ctrl_ept_pointer.storage;
        }

        if (ctrl_2nd_processor_based_vm_execution_controls.semantics.enable_vpid) {
            m_evmcs_region->ctrl_virtual_processor_identifier = ctrl_virtual_processor_identifier.storage;
        }
    }

    void mshv_virtual_cpu::evmcs_setup_controls_exit() noexcept {
        using namespace siren::x86;

        vmcs_field_value_t<vmcs_field_name_e::CTRL_PRIMARY_VMEXIT_CONTROLS> ctrl_1st_vmexit_controls = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_SECONDARY_VMEXIT_CONTROLS> ctrl_2nd_vmexit_controls = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_VMEXIT_MSR_STORE_COUNT> ctrl_vmexit_msr_store_count = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_VMEXIT_MSR_STORE_ADDRESS> ctrl_vmexit_msr_store_address = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_VMEXIT_MSR_LOAD_COUNT> ctrl_vmexit_msr_load_count = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_VMEXIT_MSR_LOAD_ADDRESS> ctrl_vmexit_msr_load_address = {};

        ctrl_1st_vmexit_controls.semantics.save_debug_controls = 1;
        ctrl_1st_vmexit_controls.semantics.host_address_space_size = 1;
        ctrl_1st_vmexit_controls.semantics.load_ia32_perf_global_ctrl = 0;
        ctrl_1st_vmexit_controls.semantics.acknowledge_interrupt_on_exit = 1;
        ctrl_1st_vmexit_controls.semantics.save_ia32_pat = 0;
        ctrl_1st_vmexit_controls.semantics.load_ia32_pat = 0;
        ctrl_1st_vmexit_controls.semantics.save_ia32_efer = 0;
        ctrl_1st_vmexit_controls.semantics.load_ia32_efer = 0;
        ctrl_1st_vmexit_controls.semantics.save_vmx_preemption_timer_value = 0;
        ctrl_1st_vmexit_controls.semantics.clear_ia32_bndcfgs = 0;
        ctrl_1st_vmexit_controls.semantics.conceal_vmx_from_pt = 0;
        ctrl_1st_vmexit_controls.semantics.clear_ia32_rtit_ctl = 0;
        ctrl_1st_vmexit_controls.semantics.clear_ia32_lbr_ctl = 0;
        ctrl_1st_vmexit_controls.semantics.load_cet_state = 0;
        ctrl_1st_vmexit_controls.semantics.load_pkrs = 0;
        ctrl_1st_vmexit_controls.semantics.save_ia32_perf_global_ctrl = 0;
        ctrl_1st_vmexit_controls.semantics.activate_secondary_controls = 0;

        ctrl_vmexit_msr_store_count.semantics.count = 0;
        ctrl_vmexit_msr_store_address.semantics.physical_address = 0;

        ctrl_vmexit_msr_load_count.semantics.count = 0;
        ctrl_vmexit_msr_load_address.semantics.physical_address = 0;

        auto ia32_vmx_basic = read_msr<IA32_VMX_BASIC>();
        bool ctrl_2nd_vmexit_controls_supports;

        if (ia32_vmx_basic.semantics.default1_controls_can_be_zero) {
            auto ia32_vmx_true_exit_ctls = read_msr<IA32_VMX_TRUE_EXIT_CTLS>();

            // The IA32_VMX_EXIT_CTLS2 MSR exists only on processors that support the 1-setting of the "activate secondary
            // controls" VM-exit control (only if bit 63 of the IA32_VMX_EXIT_CTLS MSR is 1).
            ctrl_2nd_vmexit_controls_supports =
                vmcs_field_value_t<vmcs_field_name_e::CTRL_PRIMARY_VMEXIT_CONTROLS>{ .storage = ia32_vmx_true_exit_ctls.semantics.mask1 }.semantics.activate_secondary_controls != 0;

            ctrl_1st_vmexit_controls.storage |= ia32_vmx_true_exit_ctls.semantics.mask0;
            ctrl_1st_vmexit_controls.storage &= ia32_vmx_true_exit_ctls.semantics.mask1;
        } else {
            auto ia32_vmx_exit_ctls = read_msr<IA32_VMX_EXIT_CTLS>();

            // The IA32_VMX_EXIT_CTLS2 MSR exists only on processors that support the 1-setting of the "activate secondary
            // controls" VM-exit control (only if bit 63 of the IA32_VMX_EXIT_CTLS MSR is 1).
            ctrl_2nd_vmexit_controls_supports =
                vmcs_field_value_t<vmcs_field_name_e::CTRL_PRIMARY_VMEXIT_CONTROLS>{ .storage = ia32_vmx_exit_ctls.semantics.mask1 }.semantics.activate_secondary_controls != 0;

            ctrl_1st_vmexit_controls.storage |= ia32_vmx_exit_ctls.semantics.mask0;
            ctrl_1st_vmexit_controls.storage &= ia32_vmx_exit_ctls.semantics.mask1;
        }

        if (ctrl_2nd_vmexit_controls_supports && ctrl_1st_vmexit_controls.semantics.activate_secondary_controls) {
            // VM entry allows control X (bit X of the secondary VM-exit controls) to be 1 if bit X in the MSR is set to 1; if bit X in
            // the MSR is cleared to 0, VM entry fails if control X and the ¡°activate secondary controls¡± primary VM-exit control are both 1.
            auto ia32_vmx_exit_ctls2 = read_msr<IA32_VMX_EXIT_CTLS2>();
            ctrl_2nd_vmexit_controls.storage &= ia32_vmx_exit_ctls2.semantics.mask;
        }

        m_evmcs_region->ctrl_primary_vmexit_controls = ctrl_1st_vmexit_controls.storage;

        //if (ctrl_2nd_vmexit_controls_supports) {
        //    m_evmcs_region->ctrl_2nd_vmexit_controls = ctrl_2nd_vmexit_controls.storage;
        //}

        m_evmcs_region->ctrl_vmexit_msr_store_count = ctrl_vmexit_msr_store_count.storage;
        m_evmcs_region->ctrl_vmexit_msr_store_address = ctrl_vmexit_msr_store_address.storage;
        m_evmcs_region->ctrl_vmexit_msr_load_count = ctrl_vmexit_msr_load_count.storage;
        m_evmcs_region->ctrl_vmexit_msr_load_address = ctrl_vmexit_msr_load_address.storage;
    }

    void mshv_virtual_cpu::evmcs_setup_controls_entry() noexcept {
        using namespace siren::x86;

        vmcs_field_value_t<vmcs_field_name_e::CTRL_VMENTRY_CONTROLS> ctrl_vmentry_controls = {};

        vmcs_field_value_t<vmcs_field_name_e::CTRL_VMENTRY_MSR_LOAD_COUNT> ctrl_vmentry_msr_load_count = {};
        vmcs_field_value_t<vmcs_field_name_e::CTRL_VMENTRY_MSR_LOAD_ADDRESS> ctrl_vmentry_msr_load_address = {};

        //vmcs_field_value_t<vmcs_field_name_e::CTRL_VMENTRY_INTERRUPTION_INFO> ctrl_vmentry_interruption_info = {};
        //vmcs_field_value_t<vmcs_field_name_e::CTRL_VMENTRY_EXCEPTION_ERROR_CODE> ctrl_vmentry_exception_error_code = {};
        //vmcs_field_value_t<vmcs_field_name_e::CTRL_VMENTRY_INSTRUCTION_LENGTH> ctrl_vmentry_instruction_length = {};

        ctrl_vmentry_controls.semantics.load_debug_controls = 1;
        ctrl_vmentry_controls.semantics.ia32e_mode_guest = 1;
        ctrl_vmentry_controls.semantics.entry_to_smm = 0;
        ctrl_vmentry_controls.semantics.deactivate_dual_monitor_treatment = 0;
        ctrl_vmentry_controls.semantics.load_ia32_perf_global_ctrl = 0;
        ctrl_vmentry_controls.semantics.load_ia32_pat = 0;
        ctrl_vmentry_controls.semantics.load_ia32_efer = 0;
        ctrl_vmentry_controls.semantics.load_ia32_bndcfgs = 0;
        ctrl_vmentry_controls.semantics.conceal_vmx_from_pt = 0;
        ctrl_vmentry_controls.semantics.load_ia32_rtit_ctl = 0;
        ctrl_vmentry_controls.semantics.load_cet_state = 0;
        ctrl_vmentry_controls.semantics.load_guest_ia32_lbr_ctl = 0;
        ctrl_vmentry_controls.semantics.load_pkrs = 0;

        ctrl_vmentry_msr_load_count.semantics.count = 0;
        ctrl_vmentry_msr_load_address.semantics.physical_address = 0;

        auto ia32_vmx_basic = read_msr<IA32_VMX_BASIC>();

        if (ia32_vmx_basic.semantics.default1_controls_can_be_zero) {
            auto ia32_vmx_true_entry_ctls = read_msr<IA32_VMX_TRUE_ENTRY_CTLS>();
            ctrl_vmentry_controls.storage |= ia32_vmx_true_entry_ctls.semantics.mask0;
            ctrl_vmentry_controls.storage &= ia32_vmx_true_entry_ctls.semantics.mask1;
        } else {
            auto ia32_vmx_entry_ctls = read_msr<IA32_VMX_ENTRY_CTLS>();
            ctrl_vmentry_controls.storage |= ia32_vmx_entry_ctls.semantics.mask0;
            ctrl_vmentry_controls.storage &= ia32_vmx_entry_ctls.semantics.mask1;
        }

        m_evmcs_region->ctrl_vmentry_controls = ctrl_vmentry_controls.storage;
        m_evmcs_region->ctrl_vmentry_msr_load_count = ctrl_vmentry_msr_load_count.storage;
        m_evmcs_region->ctrl_vmentry_msr_load_address = ctrl_vmentry_msr_load_address.storage;
    }

    SIREN_NOINLINE
    void mshv_virtual_cpu::launch() noexcept {
        using namespace siren::x86;

        vmcs_field_value_t<vmcs_field_name_e::GUEST_RSP> guest_rsp;
        vmcs_field_value_t<vmcs_field_name_e::GUEST_RIP> guest_rip;

        guest_rsp.storage = reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) + sizeof(void*);
        guest_rip.storage = reinterpret_cast<uintptr_t>(_ReturnAddress());

        m_evmcs_region->guest_rsp = guest_rsp.storage;
        m_evmcs_region->guest_rip = guest_rip.storage;

        m_evmcs_region->mshv_clean_fields.storage = 0;  // reload every evmcs fields

        vmx_launch().invoke_debugger_when_failure();
    }

    mshv_virtual_cpu::mshv_virtual_cpu(mshv_hypervisor& hypervisor, uint32_t index) noexcept :
        m_hypervisor(hypervisor),
        m_index(index),
        m_running(false),
        m_hypercall_page(nullptr),
        m_hypercall_page_physical_address(0),
        m_vp_assist_page(nullptr),
        m_vp_assist_page_physical_address(0),
        m_partition_assist_page(),
        m_partition_assist_page_physical_address(0),
        m_vmxon_region(),
        m_vmxon_region_physical_address(0),
        m_evmcs_region(),
        m_evmcs_region_physical_address(0),
        m_msr_bitmap(),
        m_msr_bitmap_physical_address(0),
        m_stack_region(),
        m_stack_region_physical_address(0) {}

    mshv_virtual_cpu::~mshv_virtual_cpu() noexcept {
        if (m_running) {
            invoke_debugger_noreturn();
        }
    }

    [[nodiscard]]
    status_t mshv_virtual_cpu::init() noexcept {
        std::byte* hypercall_page = nullptr;
        x86::physical_address_t hypercall_page_physical_address = 0;

        microsoft_hv::vp_assist_page_t* vp_assist_page = nullptr;
        x86::physical_address_t vp_assist_page_physical_address = 0;

        managed_memory_ptr<microsoft_hv::partition_assist_page_t> partition_assist_page ;
        x86::physical_address_t partition_assist_page_physical_address = 0;

        managed_memory_ptr<microsoft_hv::vmx_enlightened_vmcs_t> vmxon_region;
        x86::physical_address_t vmxon_region_physical_address = 0;

        managed_memory_ptr<microsoft_hv::vmx_enlightened_vmcs_t> evmcs_region;
        x86::physical_address_t evmcs_region_physical_address = 0;

        managed_memory_ptr<std::byte[]> msr_bitmap = nullptr;
        x86::physical_address_t msr_bitmap_physical_address = 0;

        managed_memory_ptr<std::byte[]> stack_region = nullptr;
        x86::physical_address_t stack_region_physical_address = 0;

        x86::run_for_cpu(
            m_index, 
            [&hypercall_page_physical_address, &vp_assist_page_physical_address]() noexcept {
                auto msr_hypercall = x86::read_msr<microsoft_hv::HV_X64_MSR_HYPERCALL>();
                if (msr_hypercall.semantics.hypercall_page_enable) {
                    hypercall_page_physical_address = x86::pfn_to_address(msr_hypercall.semantics.hypercall_pfn, x86::_4kb_page_traits{});
                }

                auto msr_vp_assist_page = x86::read_msr<microsoft_hv::HV_X64_MSR_VP_ASSIST_PAGE>();
                if (msr_vp_assist_page.semantics.enable) {
                    vp_assist_page_physical_address = x86::pfn_to_address(msr_vp_assist_page.semantics.physical_address, x86::_4kb_page_traits{});
                }
            }
        );

        if (hypercall_page_physical_address) {
            hypercall_page = x86::get_virtual_address<std::byte*>(hypercall_page_physical_address);
        } else {
#if DBG
            // We should establish the hypercall interface manually. But I don't think we would encounter here since OS should have done it.
            // If still run here, let debugger be invoked.
            invoke_debugger();
#endif
            return status_t::not_implemented_v;
        }

        if (vp_assist_page_physical_address) {
            vp_assist_page = x86::get_virtual_address<microsoft_hv::vp_assist_page_t*>(vp_assist_page_physical_address);
        } else {
#if DBG
            // We should allocate and map a vp-assist-page manually. But I don't think we would encounter here since OS should have done it.
            // If still run here, let debugger be invoked.
            invoke_debugger();
#endif
            return status_t::not_implemented_v;
        }

        partition_assist_page = make_managed_memory<false, microsoft_hv::partition_assist_page_t>(4_kb_size_v);
        if (partition_assist_page) {
            partition_assist_page_physical_address = x86::get_physical_address(partition_assist_page);
        } else {
            return status_t::insufficient_memory_v;
        }

        vmxon_region = make_managed_memory<false, microsoft_hv::vmx_enlightened_vmcs_t>(4_kb_size_v);
        if (vmxon_region) {
            vmxon_region->version_number = 1;   // todo
            vmxon_region_physical_address = x86::get_physical_address(vmxon_region);
        } else {
            return status_t::insufficient_memory_v;
        }

        evmcs_region = make_managed_memory<false, microsoft_hv::vmx_enlightened_vmcs_t>(4_kb_size_v);
        if (evmcs_region) {
            evmcs_region->version_number = 1;   // todo
            evmcs_region_physical_address = x86::get_physical_address(evmcs_region);
        } else {
            return status_t::insufficient_memory_v;
        }

        msr_bitmap = make_managed_memory<false, std::byte[]>(4_kb_size_v);
        if (msr_bitmap) {
            msr_bitmap_physical_address = x86::get_physical_address(msr_bitmap);
        } else {
            return status_t::insufficient_memory_v;
        }

        stack_region = make_managed_memory<false, std::byte[]>(stack_region_size_v);
        if (stack_region) {
            stack_region_physical_address = x86::get_physical_address(stack_region);
        } else {
            return status_t::insufficient_memory_v;
        }

        m_hypercall_page = hypercall_page;
        m_hypercall_page_physical_address = hypercall_page_physical_address;

        m_vp_assist_page = vp_assist_page;
        m_vp_assist_page_physical_address = vp_assist_page_physical_address;

        m_partition_assist_page = std::move(partition_assist_page);
        m_partition_assist_page_physical_address = partition_assist_page_physical_address;

        m_vmxon_region = std::move(vmxon_region);
        m_vmxon_region_physical_address = vmxon_region_physical_address;

        m_evmcs_region = std::move(evmcs_region);
        m_evmcs_region_physical_address = evmcs_region_physical_address;

        m_msr_bitmap = std::move(msr_bitmap);
        m_msr_bitmap_physical_address = msr_bitmap_physical_address;

        m_stack_region = std::move(stack_region);
        m_stack_region_physical_address = stack_region_physical_address;

        return status_t::success_v;
    }

    [[nodiscard]]
    abstract_hypervisor& mshv_virtual_cpu::get_hypervisor() noexcept {
        return m_hypervisor;
    }

    [[nodiscard]]
    const abstract_hypervisor& mshv_virtual_cpu::get_hypervisor() const noexcept {
        return m_hypervisor;
    }

    [[nodiscard]]
    uint32_t mshv_virtual_cpu::get_index() const noexcept {
        return m_index;
    }

    void mshv_virtual_cpu::start() noexcept {
        using namespace siren::x86;

        if (!m_running) {
            auto vmx_cr0 = read_cr0();
            auto ia32_vmx_cr0_fixed0 = read_msr<IA32_VMX_CR0_FIXED0>();
            auto ia32_vmx_cr0_fixed1 = read_msr<IA32_VMX_CR0_FIXED1>();

            vmx_cr0.storage |= ia32_vmx_cr0_fixed0.semantics.mask_value;
            vmx_cr0.storage &= ia32_vmx_cr0_fixed1.semantics.mask_value;

            auto vmx_cr4 = read_cr4();
            auto ia32_vmx_cr4_fixed0 = read_msr<IA32_VMX_CR4_FIXED0>();
            auto ia32_vmx_cr4_fixed1 = read_msr<IA32_VMX_CR4_FIXED1>();

            vmx_cr4.storage |= ia32_vmx_cr4_fixed0.semantics.mask_value;
            vmx_cr4.storage &= ia32_vmx_cr4_fixed1.semantics.mask_value;

            write_cr0(vmx_cr0);
            write_cr4(vmx_cr4);

            vmx_on(m_vmxon_region_physical_address).invoke_debugger_when_failure();
            vmx_clear(m_evmcs_region_physical_address).invoke_debugger_when_failure();

            // equivalent to vmxptrld
            m_vp_assist_page->current_nested_vmcs = m_evmcs_region_physical_address;
            m_vp_assist_page->enlighten_vm_entry = true;

            evmcs_setup_guest();
            evmcs_setup_host();
            evmcs_setup_controls_execution();
            evmcs_setup_controls_exit();
            evmcs_setup_controls_entry();

            m_evmcs_region->mshv_vp_id = m_index;
            m_evmcs_region->mshv_vm_id = reinterpret_cast<uintptr_t>(&m_hypervisor);
            m_evmcs_region->mshv_partition_assist_page = microsoft_hv::gpa_t{ m_partition_assist_page_physical_address };

            m_vp_assist_page->nested_enlightenments_control.features.semantics.direct_hypercall = 1;
            m_evmcs_region->mshv_enlightenments_control.semantics.nested_flush_virtual_hypercall = 1;

            launch();

            m_running = true;
        }
    }

    void mshv_virtual_cpu::stop() noexcept {
        if (m_running) {
            siren_hypercalls::turn_off_vm();
            m_running = false;
        }
    }

    [[nodiscard]]
    microsoft_hv::vmx_enlightened_vmcs_t* mshv_virtual_cpu::get_enlightened_vmcs() noexcept {
        return m_evmcs_region.get();
    }

    [[nodiscard]]
    const microsoft_hv::vmx_enlightened_vmcs_t* mshv_virtual_cpu::get_enlightened_vmcs() const noexcept {
        return m_evmcs_region.get();
    }

    [[nodiscard]]
    const std::byte* mshv_virtual_cpu::get_hypercall_page() const noexcept {
        return m_hypercall_page;
    }

    void mshv_virtual_cpu::inject_bp_exception() noexcept {
        using namespace siren::x86;

        vmcs_field_value_t<vmcs_field_name_e::CTRL_VMENTRY_INTERRUPTION_INFO> ctrl_vmentry_interruption_info = {};

        ctrl_vmentry_interruption_info.semantics.vector_index = std::to_underlying(exception_vector_e::EV_BREAKPOINT);
        ctrl_vmentry_interruption_info.semantics.interruption_type = 6; // software exception
        ctrl_vmentry_interruption_info.semantics.deliver_error_code = 0;
        ctrl_vmentry_interruption_info.semantics.valid = 1;

        m_evmcs_region->ctrl_vmentry_interruption_info = ctrl_vmentry_interruption_info.storage;
        m_evmcs_region->ctrl_vmentry_instruction_length = m_evmcs_region->info_vmexit_instruction_length;
        m_evmcs_region->mshv_clean_fields.semantics.control_event = 0;
    }

    void mshv_virtual_cpu::inject_ud_exception() noexcept {
        using namespace siren::x86;

        vmcs_field_value_t<vmcs_field_name_e::CTRL_VMENTRY_INTERRUPTION_INFO> ctrl_vmentry_interruption_info = {};

        ctrl_vmentry_interruption_info.semantics.vector_index = std::to_underlying(exception_vector_e::EV_UNDEFINED_OPCODE);
        ctrl_vmentry_interruption_info.semantics.interruption_type = 3; // hardware exception
        ctrl_vmentry_interruption_info.semantics.deliver_error_code = 0;
        ctrl_vmentry_interruption_info.semantics.valid = 1;

        m_evmcs_region->ctrl_vmentry_interruption_info = ctrl_vmentry_interruption_info.storage;
        m_evmcs_region->mshv_clean_fields.semantics.control_event = 0;
    }

    void mshv_virtual_cpu::inject_gp_exception(uint32_t error_code) noexcept {
        using namespace siren::x86;

        vmcs_field_value_t<vmcs_field_name_e::CTRL_VMENTRY_INTERRUPTION_INFO> ctrl_vmentry_interruption_info = {};

        ctrl_vmentry_interruption_info.semantics.vector_index = std::to_underlying(exception_vector_e::EV_GENERAL_PROTECTION);
        ctrl_vmentry_interruption_info.semantics.interruption_type = 3; // hardware exception
        ctrl_vmentry_interruption_info.semantics.deliver_error_code = 1;
        ctrl_vmentry_interruption_info.semantics.valid = 1;

        m_evmcs_region->ctrl_vmentry_interruption_info = ctrl_vmentry_interruption_info.storage;
        m_evmcs_region->ctrl_vmentry_exception_error_code = error_code;
        m_evmcs_region->ctrl_vmentry_instruction_length = m_evmcs_region->info_vmexit_instruction_length;
        m_evmcs_region->mshv_clean_fields.semantics.control_event = 0;
    }

}
