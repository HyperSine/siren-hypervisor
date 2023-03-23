#include "mshv_hypervisor.hpp"
#include "mshv_virtual_cpu.hpp"
#include "mshv_vmexit_handler.hpp"
#include "siren_hypercalls.hpp"

#include "../address_space.hpp"
#include "../debugging.hpp"
#include "../multiprocessor.hpp"

#include "../x86/control_registers.hpp"
#include "../x86/debug_registers.hpp"
#include "../x86/interrupts_and_excpetions.hpp"
#include "../x86/memory_caching.hpp"
#include "../x86/model_specific_registers.hpp"

namespace siren::vmx {
    template<x86::segment_register_e SegmentReg>
    void mshv_virtual_cpu::evmcs_setup_segment(const x86::gdtr_t& gdtr, const x86::segment_selector_t& ldtr, auto seg_selector, auto seg_base, auto seg_limit, auto seg_access_rights) noexcept {
        if (seg_selector || seg_base || seg_limit || seg_access_rights) {
            auto segment_selector = SegmentReg == x86::segment_register_e::LDTR ? ldtr : x86::read_segment_selector<SegmentReg>();

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
                    if constexpr (SegmentReg == x86::segment_register_e::LDTR) {
                        const auto* descriptor_table = reinterpret_cast<const x86::segment_descriptor_t*>(gdtr.semantics.base);
                        seg_base->storage = reinterpret_cast<const x86::segment_descriptor_system_long_t*>(&descriptor_table[segment_selector.semantics.index])->semantics.ldt.base();
                    } else if constexpr (SegmentReg == x86::segment_register_e::TR) {
                        const auto* descriptor_table = reinterpret_cast<const x86::segment_descriptor_t*>(gdtr.semantics.base);
                        seg_base->storage = reinterpret_cast<const x86::segment_descriptor_system_long_t*>(&descriptor_table[segment_selector.semantics.index])->semantics.tss.base();
                    } else if constexpr (SegmentReg == x86::segment_register_e::FS) {
                        seg_base->storage = x86::read_msr<x86::IA32_FS_BASE>().semantics.base;
                    } else if constexpr (SegmentReg == x86::segment_register_e::GS) {
                        seg_base->storage = x86::read_msr<x86::IA32_GS_BASE>().semantics.base;
                    } else {
                        const auto* descriptor_table = reinterpret_cast<const x86::segment_descriptor_t*>(gdtr.semantics.base);

                        if (segment_selector.semantics.table_indicator == 1) {  // use LDT
                            auto ldt_base = reinterpret_cast<const x86::segment_descriptor_system_long_t*>(&descriptor_table[ldtr.semantics.index])->semantics.ldt.base();
                            descriptor_table = reinterpret_cast<const x86::segment_descriptor_t*>(ldt_base);
                        }

                        if constexpr (SegmentReg == x86::segment_register_e::CS) {
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

    void mshv_virtual_cpu::evmcs_setup_guest() noexcept {
        using namespace siren::x86;

        vmcsf_t<VMCSF_GUEST_CR0> guest_cr0 = {};
        vmcsf_t<VMCSF_GUEST_CR3> guest_cr3 = {};
        vmcsf_t<VMCSF_GUEST_CR4> guest_cr4 = {};
        vmcsf_t<VMCSF_GUEST_DR7> guest_dr7 = {};

        //vmcsf_t<VMCSF_GUEST_RSP> guest_rsp = {};     // will be setup in launch() function.
        //vmcsf_t<VMCSF_GUEST_RIP> guest_rip = {};     // will be setup in launch() function.
        vmcsf_t<VMCSF_GUEST_RFLAGS> guest_rflags = {};

        vmcsf_t<VMCSF_GUEST_CS_SELECTOR> guest_cs_selector = {};
        vmcsf_t<VMCSF_GUEST_SS_SELECTOR> guest_ss_selector = {};
        vmcsf_t<VMCSF_GUEST_DS_SELECTOR> guest_ds_selector = {};
        vmcsf_t<VMCSF_GUEST_ES_SELECTOR> guest_es_selector = {};
        vmcsf_t<VMCSF_GUEST_FS_SELECTOR> guest_fs_selector = {};
        vmcsf_t<VMCSF_GUEST_GS_SELECTOR> guest_gs_selector = {};
        vmcsf_t<VMCSF_GUEST_LDTR_SELECTOR> guest_ldtr_selector = {};
        vmcsf_t<VMCSF_GUEST_TR_SELECTOR> guest_tr_selector = {};

        vmcsf_t<VMCSF_GUEST_CS_BASE> guest_cs_base = {};
        vmcsf_t<VMCSF_GUEST_SS_BASE> guest_ss_base = {};
        vmcsf_t<VMCSF_GUEST_DS_BASE> guest_ds_base = {};
        vmcsf_t<VMCSF_GUEST_ES_BASE> guest_es_base = {};
        vmcsf_t<VMCSF_GUEST_FS_BASE> guest_fs_base = {};
        vmcsf_t<VMCSF_GUEST_GS_BASE> guest_gs_base = {};
        vmcsf_t<VMCSF_GUEST_LDTR_BASE> guest_ldtr_base = {};
        vmcsf_t<VMCSF_GUEST_TR_BASE> guest_tr_base = {};

        vmcsf_t<VMCSF_GUEST_CS_LIMIT> guest_cs_limit = {};
        vmcsf_t<VMCSF_GUEST_SS_LIMIT> guest_ss_limit = {};
        vmcsf_t<VMCSF_GUEST_DS_LIMIT> guest_ds_limit = {};
        vmcsf_t<VMCSF_GUEST_ES_LIMIT> guest_es_limit = {};
        vmcsf_t<VMCSF_GUEST_FS_LIMIT> guest_fs_limit = {};
        vmcsf_t<VMCSF_GUEST_GS_LIMIT> guest_gs_limit = {};
        vmcsf_t<VMCSF_GUEST_LDTR_LIMIT> guest_ldtr_limit = {};
        vmcsf_t<VMCSF_GUEST_TR_LIMIT> guest_tr_limit = {};

        vmcsf_t<VMCSF_GUEST_CS_ACCESS_RIGHTS> guest_cs_access_rights = {};
        vmcsf_t<VMCSF_GUEST_SS_ACCESS_RIGHTS> guest_ss_access_rights = {};
        vmcsf_t<VMCSF_GUEST_DS_ACCESS_RIGHTS> guest_ds_access_rights = {};
        vmcsf_t<VMCSF_GUEST_ES_ACCESS_RIGHTS> guest_es_access_rights = {};
        vmcsf_t<VMCSF_GUEST_FS_ACCESS_RIGHTS> guest_fs_access_rights = {};
        vmcsf_t<VMCSF_GUEST_GS_ACCESS_RIGHTS> guest_gs_access_rights = {};
        vmcsf_t<VMCSF_GUEST_LDTR_ACCESS_RIGHTS> guest_ldtr_access_rights = {};
        vmcsf_t<VMCSF_GUEST_TR_ACCESS_RIGHTS> guest_tr_access_rights = {};

        vmcsf_t<VMCSF_GUEST_GDTR_BASE> guest_gdtr_base = {};
        vmcsf_t<VMCSF_GUEST_IDTR_BASE> guest_idtr_base = {};

        vmcsf_t<VMCSF_GUEST_GDTR_LIMIT> guest_gdtr_limit = {};
        vmcsf_t<VMCSF_GUEST_IDTR_LIMIT> guest_idtr_limit = {};

        vmcsf_t<VMCSF_GUEST_IA32_DEBUGCTL> guest_ia32_debugctl = {};
        vmcsf_t<VMCSF_GUEST_IA32_SYSENTER_CS> guest_ia32_sysenter_cs = {};
        vmcsf_t<VMCSF_GUEST_IA32_SYSENTER_ESP> guest_ia32_sysenter_esp = {};
        vmcsf_t<VMCSF_GUEST_IA32_SYSENTER_EIP> guest_ia32_sysenter_eip = {};
        vmcsf_t<VMCSF_GUEST_IA32_PERF_GLOBAL_CTRL> guest_ia32_perf_global_ctrl = {};
        vmcsf_t<VMCSF_GUEST_IA32_PAT> guest_ia32_pat = {};
        vmcsf_t<VMCSF_GUEST_IA32_EFER> guest_ia32_efer = {};
        vmcsf_t<VMCSF_GUEST_IA32_BNDCFGS> guest_ia32_bndcfgs = {};
        vmcsf_t<VMCSF_GUEST_IA32_RTIT_CTL> guest_ia32_rtit_ctl = {};
        vmcsf_t<VMCSF_GUEST_IA32_LBR_CTL> guest_ia32_lbr_ctl = {};
        vmcsf_t<VMCSF_GUEST_IA32_S_CET> guest_ia32_s_cet = {};
        vmcsf_t<VMCSF_GUEST_IA32_INTERRUPT_SSP_TABLE_ADDR> guest_ia32_interrupt_ssp_table_addr = {};
        vmcsf_t<VMCSF_GUEST_IA32_PKRS> guest_ia32_pkrs = {};

        vmcsf_t<VMCSF_GUEST_SSP> guest_ssp = {};
        vmcsf_t<VMCSF_GUEST_SMBASE> guest_smbase = {};

        vmcsf_t<VMCSF_GUEST_ACTIVITY_STATE> guest_activity_state = {};
        vmcsf_t<VMCSF_GUEST_INTERRUPTIBILITY_STATE> guest_interruptibility_state = {};
        vmcsf_t<VMCSF_GUEST_PENDING_DEBUG_EXCEPTIONS> guest_pending_debug_exceptions = {};
        vmcsf_t<VMCSF_GUEST_VMCS_LINK_POINTER> guest_vmcs_link_pointer = {};
        vmcsf_t<VMCSF_GUEST_VMX_PREEMPTION_TIMER_VALUE> guest_vmx_preemption_timer_value = {};
        vmcsf_t<VMCSF_GUEST_PDPTE0> guest_pdpte0 = {};
        vmcsf_t<VMCSF_GUEST_PDPTE1> guest_pdpte1 = {};
        vmcsf_t<VMCSF_GUEST_PDPTE2> guest_pdpte2 = {};
        vmcsf_t<VMCSF_GUEST_PDPTE3> guest_pdpte3 = {};
        vmcsf_t<VMCSF_GUEST_INTERRUPT_STATUS> guest_interrupt_status = {};
        vmcsf_t<VMCSF_GUEST_PML_INDEX> guest_pml_index = {};

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

        guest_vmcs_link_pointer.storage = ~x86::paddr_t{ 0 };

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

        vmcsf_t<VMCSF_HOST_CR0> host_cr0 = {};
        vmcsf_t<VMCSF_HOST_CR3> host_cr3 = {};
        vmcsf_t<VMCSF_HOST_CR4> host_cr4 = {};

        vmcsf_t<VMCSF_HOST_RSP> host_rsp = {};
        vmcsf_t<VMCSF_HOST_RIP> host_rip = {};

        vmcsf_t<VMCSF_HOST_CS_SELECTOR> host_cs_selector = {};
        vmcsf_t<VMCSF_HOST_SS_SELECTOR> host_ss_selector = {};
        vmcsf_t<VMCSF_HOST_DS_SELECTOR> host_ds_selector = {};
        vmcsf_t<VMCSF_HOST_ES_SELECTOR> host_es_selector = {};
        vmcsf_t<VMCSF_HOST_FS_SELECTOR> host_fs_selector = {};
        vmcsf_t<VMCSF_HOST_GS_SELECTOR> host_gs_selector = {};
        vmcsf_t<VMCSF_HOST_TR_SELECTOR> host_tr_selector = {};

        vmcsf_t<VMCSF_HOST_FS_BASE> host_fs_base = {};
        vmcsf_t<VMCSF_HOST_GS_BASE> host_gs_base = {};
        vmcsf_t<VMCSF_HOST_TR_BASE> host_tr_base = {};
        vmcsf_t<VMCSF_HOST_GDTR_BASE> host_gdtr_base = {};
        vmcsf_t<VMCSF_HOST_IDTR_BASE> host_idtr_base = {};

        vmcsf_t<VMCSF_HOST_IA32_SYSENTER_CS> host_ia32_sysenter_cs = {};
        vmcsf_t<VMCSF_HOST_IA32_SYSENTER_ESP> host_ia32_sysenter_esp = {};
        vmcsf_t<VMCSF_HOST_IA32_SYSENTER_EIP> host_ia32_sysenter_eip = {};
        vmcsf_t<VMCSF_HOST_IA32_PERF_GLOBAL_CTRL> host_ia32_perf_global_ctrl = {};
        vmcsf_t<VMCSF_HOST_IA32_PAT> host_ia32_pat = {};
        vmcsf_t<VMCSF_HOST_IA32_EFER> host_ia32_efer = {};
        vmcsf_t<VMCSF_HOST_IA32_S_CET> host_ia32_s_cet = {};
        vmcsf_t<VMCSF_HOST_IA32_INTERRUPT_SSP_TABLE_ADDR> host_ia32_interrupt_ssp_table_addr = {};
        vmcsf_t<VMCSF_HOST_IA32_PKRS> host_ia32_pkrs = {};

        vmcsf_t<VMCSF_HOST_SSP> host_ssp = {};

        auto gdtr = read_gdtr();
        auto idtr = read_idtr();
        auto ldtr = read_segment_selector<x86::segment_register_e::LDTR>();

        host_cr0.storage = read_cr0().storage;
        host_cr3.storage = read_cr3().storage;
        host_cr4.storage = read_cr4().storage;

        m_vmexit_stack->self = this;
        host_rsp.storage = reinterpret_cast<uintptr_t>(std::end(m_vmexit_stack->in_use));
        host_rip.storage = reinterpret_cast<uintptr_t>(&mshv_vmexit_handler::entry_point);

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

        vmcsf_t<VMCSF_CTRL_PIN_BASED_VM_EXECUTION_CONTROLS> ctrl_pin_based_vm_execution_controls = {};

        vmcsf_t<VMCSF_CTRL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS> ctrl_1st_processor_based_vm_execution_controls = {};
        vmcsf_t<VMCSF_CTRL_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS> ctrl_2nd_processor_based_vm_execution_controls = {};
        vmcsf_t<VMCSF_CTRL_TERTIARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS> ctrl_3rd_processor_based_vm_execution_controls = {};

        vmcsf_t<VMCSF_CTRL_EXCEPTION_BITMAP> ctrl_exception_bitmap = {};

        vmcsf_t<VMCSF_CTRL_IO_BITMAP_A_ADDRESS> ctrl_io_bitmap_a_address = {};
        vmcsf_t<VMCSF_CTRL_IO_BITMAP_B_ADDRESS> ctrl_io_bitmap_b_address = {};

        vmcsf_t<VMCSF_CTRL_TSC_OFFSET> ctrl_tsc_offset = {};
        vmcsf_t<VMCSF_CTRL_TSC_MULTIPLIER> ctrl_tsc_multiplier = {};

        vmcsf_t<VMCSF_CTRL_CR0_GUEST_HOST_MASK> ctrl_cr0_guest_host_mask = {};
        vmcsf_t<VMCSF_CTRL_CR4_GUEST_HOST_MASK> ctrl_cr4_guest_host_mask = {};
        vmcsf_t<VMCSF_CTRL_CR0_READ_SHADOW> ctrl_cr0_read_shadow = {};
        vmcsf_t<VMCSF_CTRL_CR4_READ_SHADOW> ctrl_cr4_read_shadow = {};

        vmcsf_t<VMCSF_CTRL_CR3_TARGET_VALUE0> ctrl_cr3_target_value0 = {};
        vmcsf_t<VMCSF_CTRL_CR3_TARGET_VALUE1> ctrl_cr3_target_value1 = {};
        vmcsf_t<VMCSF_CTRL_CR3_TARGET_VALUE2> ctrl_cr3_target_value2 = {};
        vmcsf_t<VMCSF_CTRL_CR3_TARGET_VALUE3> ctrl_cr3_target_value3 = {};
        vmcsf_t<VMCSF_CTRL_CR3_TARGET_COUNT> ctrl_cr3_target_count = {};

        vmcsf_t<VMCSF_CTRL_APIC_ACCESS_ADDRESS> ctrl_apic_access_address = {};
        vmcsf_t<VMCSF_CTRL_VIRTUAL_APIC_ADDRESS> ctrl_virtual_apic_address = {};
        vmcsf_t<VMCSF_CTRL_TPR_THRESHOLD> ctrl_tpr_threshold = {};
        vmcsf_t<VMCSF_CTRL_EOI_EXIT0_BITMAP> ctrl_eoi_exit0_bitmap = {};
        vmcsf_t<VMCSF_CTRL_EOI_EXIT1_BITMAP> ctrl_eoi_exit1_bitmap = {};
        vmcsf_t<VMCSF_CTRL_EOI_EXIT2_BITMAP> ctrl_eoi_exit2_bitmap = {};
        vmcsf_t<VMCSF_CTRL_EOI_EXIT3_BITMAP> ctrl_eoi_exit3_bitmap = {};
        vmcsf_t<VMCSF_CTRL_POSTED_INTERRUPT_NOTIFICATION_VECTOR> ctrl_posted_interrupt_notification_vector = {};
        vmcsf_t<VMCSF_CTRL_POSTED_INTERRUPT_DESCRIPTOR_ADDRESS> ctrl_posted_interrupt_descriptor_address = {};

        vmcsf_t<VMCSF_CTRL_MSR_BITMAP_ADDRESS> ctrl_msr_bitmap_address = {};

        vmcsf_t<VMCSF_CTRL_EXECUTIVE_VMCS_POINTER> ctrl_executive_vmcs_pointer = {};

        vmcsf_t<VMCSF_CTRL_EPT_POINTER> ctrl_ept_pointer = {};

        vmcsf_t<VMCSF_CTRL_VPID> ctrl_vpid = {};

        vmcsf_t<VMCSF_CTRL_PLE_GAP> ctrl_ple_gap = {};
        vmcsf_t<VMCSF_CTRL_PLE_WINDOW> ctrl_ple_window = {};

        vmcsf_t<VMCSF_CTRL_VM_FUNCTION_CONTROLS> ctrl_vm_function_controls = {};

        vmcsf_t<VMCSF_CTRL_VMREAD_BITMAP_ADDRESS> ctrl_vmread_bitmap = {};
        vmcsf_t<VMCSF_CTRL_VMWRITE_BITMAP_ADDRESS> ctrl_vmwrite_bitmap = {};

        vmcsf_t<VMCSF_CTRL_ENCLS_EXITING_BITMAP> ctrl_encls_exiting_bitmap = {};

        vmcsf_t<VMCSF_CTRL_ENCLV_EXITING_BITMAP> ctrl_enclv_exiting_bitmap = {};

        vmcsf_t<VMCSF_CTRL_PCONFIG_EXITING_BITMAP> ctrl_pconfig_exiting_bitmap = {};

        vmcsf_t<VMCSF_CTRL_PML_ADDRESS> ctrl_pml_address = {};

        vmcsf_t<VMCSF_CTRL_VIRTUALIZATION_EXCEPTION_INFO_ADDRESS> ctrl_virtualization_exception_info_address = {};
        vmcsf_t<VMCSF_CTRL_EPTP_INDEX> ctrl_eptp_index = {};

        vmcsf_t<VMCSF_CTRL_XSS_EXITING_BITMAP> ctrl_xss_exiting_bitmap = {};

        vmcsf_t<VMCSF_CTRL_SPPTP> ctrl_spptp = {};

        vmcsf_t<VMCSF_CTRL_HLATP> ctrl_hlatp = {};

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

        ctrl_msr_bitmap_address.storage = m_hv->m_msr_bitmap.get_address();

        ctrl_ept_pointer.semantics.memory_type = memory_type_write_back_v.value;
        ctrl_ept_pointer.semantics.page_walk_length = 4 - 1;
        ctrl_ept_pointer.semantics.enable_accessed_and_dirty_flag = 1;
        ctrl_ept_pointer.semantics.pml4_physical_address = address_to_pfn<4_Kiuz>(m_hv->m_dynamic_ept.get_top_level_address());

        ctrl_vpid.semantics.vpid = 1;

        auto ia32_vmx_basic = read_msr<IA32_VMX_BASIC>();
        bool ctrl_2nd_processor_based_vm_execution_controls_support;
        bool ctrl_3rd_processor_based_vm_execution_controls_support;

        if (ia32_vmx_basic.semantics.default1_controls_can_be_zero) {
            auto ia32_vmx_true_pinbased_ctls = read_msr<IA32_VMX_TRUE_PINBASED_CTLS>();
            auto ia32_vmx_true_procbased_ctls = read_msr<IA32_VMX_TRUE_PROCBASED_CTLS>();

            ctrl_2nd_processor_based_vm_execution_controls_support =
                vmcsf_t<VMCSF_CTRL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS>{ .storage = ia32_vmx_true_procbased_ctls.semantics.mask1 }.semantics.activate_secondary_controls != 0;

            ctrl_3rd_processor_based_vm_execution_controls_support =
                vmcsf_t<VMCSF_CTRL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS>{ .storage = ia32_vmx_true_procbased_ctls.semantics.mask1 }.semantics.activate_tertiary_controls != 0;

            ctrl_pin_based_vm_execution_controls.storage |= ia32_vmx_true_pinbased_ctls.semantics.mask0;
            ctrl_pin_based_vm_execution_controls.storage &= ia32_vmx_true_pinbased_ctls.semantics.mask1;

            ctrl_1st_processor_based_vm_execution_controls.storage |= ia32_vmx_true_procbased_ctls.semantics.mask0;
            ctrl_1st_processor_based_vm_execution_controls.storage &= ia32_vmx_true_procbased_ctls.semantics.mask1;
        } else {
            auto ia32_vmx_pinbased_ctls = read_msr<IA32_VMX_PINBASED_CTLS>();
            auto ia32_vmx_procbased_ctls = read_msr<IA32_VMX_PROCBASED_CTLS>();

            ctrl_2nd_processor_based_vm_execution_controls_support =
                vmcsf_t<VMCSF_CTRL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS>{ .storage = ia32_vmx_procbased_ctls.semantics.mask1 }.semantics.activate_secondary_controls != 0;

            ctrl_3rd_processor_based_vm_execution_controls_support =
                vmcsf_t<VMCSF_CTRL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS>{ .storage = ia32_vmx_procbased_ctls.semantics.mask1 }.semantics.activate_tertiary_controls != 0;

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
            m_evmcs_region->ctrl_vpid = ctrl_vpid.storage;
        }
    }

    void mshv_virtual_cpu::evmcs_setup_controls_exit() noexcept {
        using namespace siren::x86;

        vmcsf_t<VMCSF_CTRL_PRIMARY_VMEXIT_CONTROLS> ctrl_1st_vmexit_controls = {};
        vmcsf_t<VMCSF_CTRL_SECONDARY_VMEXIT_CONTROLS> ctrl_2nd_vmexit_controls = {};

        vmcsf_t<VMCSF_CTRL_VMEXIT_MSR_STORE_COUNT> ctrl_vmexit_msr_store_count = {};
        vmcsf_t<VMCSF_CTRL_VMEXIT_MSR_STORE_ADDRESS> ctrl_vmexit_msr_store_address = {};
        vmcsf_t<VMCSF_CTRL_VMEXIT_MSR_LOAD_COUNT> ctrl_vmexit_msr_load_count = {};
        vmcsf_t<VMCSF_CTRL_VMEXIT_MSR_LOAD_ADDRESS> ctrl_vmexit_msr_load_address = {};

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
                vmcsf_t<VMCSF_CTRL_PRIMARY_VMEXIT_CONTROLS>{ .storage = ia32_vmx_true_exit_ctls.semantics.mask1 }.semantics.activate_secondary_controls != 0;

            ctrl_1st_vmexit_controls.storage |= ia32_vmx_true_exit_ctls.semantics.mask0;
            ctrl_1st_vmexit_controls.storage &= ia32_vmx_true_exit_ctls.semantics.mask1;
        } else {
            auto ia32_vmx_exit_ctls = read_msr<IA32_VMX_EXIT_CTLS>();

            // The IA32_VMX_EXIT_CTLS2 MSR exists only on processors that support the 1-setting of the "activate secondary
            // controls" VM-exit control (only if bit 63 of the IA32_VMX_EXIT_CTLS MSR is 1).
            ctrl_2nd_vmexit_controls_supports =
                vmcsf_t<VMCSF_CTRL_PRIMARY_VMEXIT_CONTROLS>{ .storage = ia32_vmx_exit_ctls.semantics.mask1 }.semantics.activate_secondary_controls != 0;

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

        vmcsf_t<VMCSF_CTRL_VMENTRY_CONTROLS> ctrl_vmentry_controls = {};

        vmcsf_t<VMCSF_CTRL_VMENTRY_MSR_LOAD_COUNT> ctrl_vmentry_msr_load_count = {};
        vmcsf_t<VMCSF_CTRL_VMENTRY_MSR_LOAD_ADDRESS> ctrl_vmentry_msr_load_address = {};

        //vmcsf_t<VMCSF_CTRL_VMENTRY_INTERRUPTION_INFO> ctrl_vmentry_interruption_info = {};
        //vmcsf_t<VMCSF_CTRL_VMENTRY_EXCEPTION_ERROR_CODE> ctrl_vmentry_exception_error_code = {};
        //vmcsf_t<VMCSF_CTRL_VMENTRY_INSTRUCTION_LENGTH> ctrl_vmentry_instruction_length = {};

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

    [[msvc::noinline]]
    void mshv_virtual_cpu::launch() noexcept {
        using namespace siren::x86;

        vmcsf_t<VMCSF_GUEST_RSP> guest_rsp;
        vmcsf_t<VMCSF_GUEST_RIP> guest_rip;

        guest_rsp.storage = reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) + sizeof(void*);
        guest_rip.storage = reinterpret_cast<uintptr_t>(_ReturnAddress());

        m_evmcs_region->guest_rsp = guest_rsp.storage;
        m_evmcs_region->guest_rip = guest_rip.storage;

        m_evmcs_region->mshv_clean_fields.storage = 0;  // reload every evmcs fields

        if (vmx_launch().is_failure()) {
            invoke_debugger_noreturn();
        }
    }

    mshv_virtual_cpu::mshv_virtual_cpu(mshv_hypervisor* hv, uint32_t index) noexcept :
        m_hv{ hv },
        m_index{ index },
        m_running{ false },
        m_hypercall_page{ nullptr },
        m_hypercall_page_physical_address{ 0 },
        m_vp_assist_page{ nullptr },
        m_vp_assist_page_physical_address{ 0 },
        m_partition_assist_page{},
        m_partition_assist_page_physical_address{ 0 },
        m_vmxon_region{},
        m_vmxon_region_physical_address{ 0 },
        m_evmcs_region{},
        m_evmcs_region_physical_address{ 0 },
        m_vmexit_stack{},
        m_vmexit_stack_physical_address{ 0 }
    {
        // nothing to do
    }

    mshv_virtual_cpu::~mshv_virtual_cpu() noexcept {
        if (m_running) {
            invoke_debugger_noreturn();
        }
    }

    expected<void, nt_status> mshv_virtual_cpu::intialize() noexcept {
        void* hypercall_page = nullptr;
        x86::paddr_t hypercall_page_physical_address = 0;

        microsoft_hv::vp_assist_page_t* vp_assist_page = nullptr;
        x86::paddr_t vp_assist_page_physical_address = 0;

        unique_npaged<microsoft_hv::partition_assist_page_t> partition_assist_page;
        x86::paddr_t partition_assist_page_physical_address = 0;

        unique_npaged<microsoft_hv::vmx_enlightened_vmcs_t> vmxon_region;
        x86::paddr_t vmxon_region_physical_address = 0;

        unique_npaged<microsoft_hv::vmx_enlightened_vmcs_t> evmcs_region;
        x86::paddr_t evmcs_region_physical_address = 0;

        unique_npaged<vmexit_stack_t> vmexit_stack;
        x86::paddr_t vmexit_stack_physical_address = 0;

        run_at_cpu(
            m_index, 
            [&hypercall_page_physical_address, &vp_assist_page_physical_address]() noexcept {
                auto msr_hypercall = x86::read_msr<microsoft_hv::HV_X64_MSR_HYPERCALL>();
                if (msr_hypercall.semantics.enable) {
                    hypercall_page_physical_address = x86::pfn_to_address<4_Kiuz>(msr_hypercall.semantics.hypercall_pfn);
                }

                auto msr_vp_assist_page = x86::read_msr<microsoft_hv::HV_X64_MSR_VP_ASSIST_PAGE>();
                if (msr_vp_assist_page.semantics.enable) {
                    vp_assist_page_physical_address = x86::pfn_to_address<4_Kiuz>(msr_vp_assist_page.semantics.physical_address);
                }
            }
        );

        if (hypercall_page_physical_address) {
            hypercall_page = get_virtual_address<uint8_t*>(hypercall_page_physical_address);
        } else {
#if DBG
            // We should establish the hypercall interface manually. But I don't think we would run here since OS should have done it.
            // If still run here, let debugger be invoked.
            invoke_debugger();
#endif
            return unexpected{ nt_status_not_implemented_v };
        }

        if (vp_assist_page_physical_address) {
            vp_assist_page = get_virtual_address<microsoft_hv::vp_assist_page_t*>(vp_assist_page_physical_address);
        } else {
#if DBG
            // We should allocate and map a vp-assist-page manually. But I don't think we would run here since OS should have done it.
            // If still run here, let debugger be invoked.
            invoke_debugger();
#endif
            return unexpected{ nt_status_not_implemented_v };
        }

        {
            auto r = allocate_unique<microsoft_hv::partition_assist_page_t>(npaged_pool);
            if (r.has_value()) {
                partition_assist_page = std::move(r.value());
                partition_assist_page_physical_address = get_physical_address(partition_assist_page.get());
            } else {
                return unexpected{ r.error() };
            }
        }

        {
            auto r = allocate_unique<microsoft_hv::vmx_enlightened_vmcs_t>(npaged_pool);
            if (r.has_value()) {
                vmxon_region = std::move(r.value());
                vmxon_region->version_number = 1;
                vmxon_region_physical_address = get_physical_address(vmxon_region.get());
            } else {
                return unexpected{ r.error() };
            }
        }

        {
            auto r = allocate_unique<microsoft_hv::vmx_enlightened_vmcs_t>(npaged_pool);
            if (r.has_value()) {
                evmcs_region = std::move(r.value());
                evmcs_region->version_number = 1;
                evmcs_region_physical_address = get_physical_address(evmcs_region.get());
            } else {
                return unexpected{ r.error() };
            }
        }

        {
            auto r = allocate_unique<vmexit_stack_t>(npaged_pool);
            if (r.has_value()) {
                vmexit_stack = std::move(r.value());
                vmexit_stack_physical_address = get_physical_address(vmexit_stack.get());
            } else {
                return unexpected{ r.error() };
            }
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

        m_vmexit_stack = std::move(vmexit_stack);
        m_vmexit_stack_physical_address = vmexit_stack_physical_address;

        return {};
    }

    hypervisor* mshv_virtual_cpu::get_hypervisor() noexcept {
        return m_hv;
    }

    const hypervisor* mshv_virtual_cpu::get_hypervisor() const noexcept {
        return m_hv;
    }

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

            if (vmx_on(m_vmxon_region_physical_address).is_failure()) {
                invoke_debugger_noreturn();
            }

            if (vmx_clear(m_evmcs_region_physical_address).is_failure()) {
                invoke_debugger_noreturn();
            }

            // equivalent to vmxptrld
            m_vp_assist_page->current_nested_vmcs = m_evmcs_region_physical_address;
            m_vp_assist_page->enlighten_vm_entry = true;

            evmcs_setup_guest();
            evmcs_setup_host();
            evmcs_setup_controls_execution();
            evmcs_setup_controls_exit();
            evmcs_setup_controls_entry();

            m_evmcs_region->mshv_vp_id = m_index;
            m_evmcs_region->mshv_vm_id = reinterpret_cast<uintptr_t>(m_hv);
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

    microsoft_hv::vmx_enlightened_vmcs_t* mshv_virtual_cpu::get_enlightened_vmcs() noexcept {
        return m_evmcs_region.get();
    }

    const microsoft_hv::vmx_enlightened_vmcs_t* mshv_virtual_cpu::get_enlightened_vmcs() const noexcept {
        return m_evmcs_region.get();
    }

    const void* mshv_virtual_cpu::get_hypercall_page() const noexcept {
        return m_hypercall_page;
    }

    void mshv_virtual_cpu::inject_bp_exception() noexcept {
        using namespace siren::x86;

        vmcsf_t<VMCSF_CTRL_VMENTRY_INTERRUPTION_INFO> ctrl_vmentry_interruption_info = {};

        ctrl_vmentry_interruption_info.semantics.vector_index = std::to_underlying(interrupt_e::TRAP_BREAKPOINT);
        ctrl_vmentry_interruption_info.semantics.interruption_type = 6; // software exception
        ctrl_vmentry_interruption_info.semantics.deliver_error_code = 0;
        ctrl_vmentry_interruption_info.semantics.valid = 1;

        m_evmcs_region->ctrl_vmentry_interruption_info = ctrl_vmentry_interruption_info.storage;
        m_evmcs_region->ctrl_vmentry_instruction_length = m_evmcs_region->info_vmexit_instruction_length;
        m_evmcs_region->mshv_clean_fields.semantics.control_event = 0;
    }

    void mshv_virtual_cpu::inject_ud_exception() noexcept {
        using namespace siren::x86;

        vmcsf_t<VMCSF_CTRL_VMENTRY_INTERRUPTION_INFO> ctrl_vmentry_interruption_info = {};

        ctrl_vmentry_interruption_info.semantics.vector_index = std::to_underlying(interrupt_e::FAULT_UNDEFINED_OPCODE);
        ctrl_vmentry_interruption_info.semantics.interruption_type = 3; // hardware exception
        ctrl_vmentry_interruption_info.semantics.deliver_error_code = 0;
        ctrl_vmentry_interruption_info.semantics.valid = 1;

        m_evmcs_region->ctrl_vmentry_interruption_info = ctrl_vmentry_interruption_info.storage;
        m_evmcs_region->mshv_clean_fields.semantics.control_event = 0;
    }

    void mshv_virtual_cpu::inject_gp_exception(uint32_t error_code) noexcept {
        using namespace siren::x86;

        vmcsf_t<VMCSF_CTRL_VMENTRY_INTERRUPTION_INFO> ctrl_vmentry_interruption_info = {};

        ctrl_vmentry_interruption_info.semantics.vector_index = std::to_underlying(interrupt_e::FAULT_GENERAL_PROTECTION);
        ctrl_vmentry_interruption_info.semantics.interruption_type = 3; // hardware exception
        ctrl_vmentry_interruption_info.semantics.deliver_error_code = 1;
        ctrl_vmentry_interruption_info.semantics.valid = 1;

        m_evmcs_region->ctrl_vmentry_interruption_info = ctrl_vmentry_interruption_info.storage;
        m_evmcs_region->ctrl_vmentry_exception_error_code = error_code;
        m_evmcs_region->ctrl_vmentry_instruction_length = m_evmcs_region->info_vmexit_instruction_length;
        m_evmcs_region->mshv_clean_fields.semantics.control_event = 0;
    }
}
