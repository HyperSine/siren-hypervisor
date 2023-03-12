#pragma once
#include <stddef.h>
#include <stdint.h>
#include "../literals.hpp"

namespace siren::microsoft_hv {
    using namespace ::siren::size_literals;

    using spa_t = uint64_t;
    using gpa_t = uint64_t;
    using gva_t = uint64_t;

    using address_space_id_t = uint64_t;

    using vp_index_t = uint32_t;
    constexpr auto vp_index_any_v = static_cast<vp_index_t>(-1);
    constexpr auto vp_index_self_v = static_cast<vp_index_t>(-2);

    struct connection_id_t {
        union {
            uint32_t storage;
            struct {
                uint32_t id : 24;
                uint32_t reserved : 8;
            } semantics;
        };
    };

    static_assert(sizeof(connection_id_t) == 4);
    static_assert(sizeof(connection_id_t::storage) == sizeof(connection_id_t::semantics));

    struct flush_flags_t {
        union {
            uint32_t storage;
            struct {
                uint32_t all_processors : 1;
                uint32_t all_virtual_address_spaces : 1;
                uint32_t non_global_mappings_only : 1;
                uint32_t use_extended_range_format : 1;
                uint32_t reserved : 28;
            } semantics;
        };
    };

    static_assert(sizeof(flush_flags_t) == 4);
    static_assert(sizeof(flush_flags_t::storage) == sizeof(flush_flags_t::semantics));

    struct nested_enlightenments_control_t {
        union {
            uint32_t storage;
            struct {
                uint32_t direct_hypercall : 1;
                uint32_t virtualization_exception : 1;
                uint32_t reserved : 30;
            } semantics;
        } features;
        union {
            uint32_t storage;
            struct {
                uint32_t inter_partition_communication : 1;
                uint32_t reserved : 31;
            } semantics;
        } hypercall_controls;
    };

    static_assert(sizeof(nested_enlightenments_control_t) == 8);
    static_assert(alignof(nested_enlightenments_control_t) == 4);

    // ref: https://github.com/ionescu007/hdk/blob/master/hvgdk.h
    struct virtual_apic_assist_t {
        union {
            uint32_t storage;
            struct {
                uint32_t no_eoi : 1;
                uint32_t reserved : 31;
            } semantics;
        };
    };

    static_assert(sizeof(virtual_apic_assist_t) == 4);
    static_assert(sizeof(virtual_apic_assist_t::storage) == sizeof(virtual_apic_assist_t::semantics));

    enum class vtl_entry_reason_e : uint32_t {
        // This reason is reserved and is not used.
        HvVtlEntryReserved = 0,
        // Indicates entry due to a VTL call from a lower VTL.
        HvVtlEntryVtlCall = 1,
        // Indicates entry due to an interrupt targeted to the VTL.
        HvVtlEntryInterrupt = 2
    };

    struct vp_vtl_control_t {
        // The hypervisor updates the entry reason with an indication as to why
        // the VTL was entered on the virtual processor.
        vtl_entry_reason_e entry_reason;

        // This flag determines whether the VINA interrupt line is asserted.
        struct {
            uint8_t asserted : 1;
            uint8_t reserved : 7;
        } vina_status;

        // A guest updates the VtlReturn* fields to provide the register values
        // to restore on VTL return. The specific register values that are
        // restored will vary based on whether the VTL is 32-bit or 64-bit.
        union {
            struct {
                uint64_t rax;
                uint64_t rcx;
            } x64;
            struct {
                uint32_t eax;
                uint32_t ecx;
                uint32_t edx;
                uint32_t reserved;
            } x86;
        } vtl_return;
    };

    static_assert(sizeof(vp_vtl_control_t) == 4 + 4 + 16);
    static_assert(offsetof(vp_vtl_control_t, entry_reason) == 0);
    static_assert(offsetof(vp_vtl_control_t, vina_status) == 4);
    static_assert(offsetof(vp_vtl_control_t, vtl_return) == 8);
    static_assert(alignof(vp_vtl_control_t) == 8);

    struct virtualization_fault_info_t {
        uint16_t parameter0;
        uint16_t reserved;
        uint32_t code;
        uint64_t parameter1;
    };

    static_assert(alignof(virtualization_fault_info_t) == 8);
    static_assert(sizeof(virtualization_fault_info_t) == 16);

    struct alignas(4_KiB_size_v) vp_assist_page_t {
        //
        // APIC assist for optimized EOI processing.
        //
        virtual_apic_assist_t apic_assist;

        //
        // VP-VTL control information
        //
        vp_vtl_control_t vtl_control;

        nested_enlightenments_control_t nested_enlightenments_control;

        bool enlighten_vm_entry;

        gpa_t current_nested_vmcs;

        bool synthetic_time_unhalted_timer_expired;

        //
        // VirtualizationFaultInformation must be 16 byte aligned.
        //
        virtualization_fault_info_t virtualization_fault_information;
    };

    static_assert(sizeof(vp_assist_page_t) == 4_KiB_size_v);
    static_assert(offsetof(vp_assist_page_t, apic_assist) == 0);
    static_assert(offsetof(vp_assist_page_t, vtl_control) == 8);
    static_assert(offsetof(vp_assist_page_t, nested_enlightenments_control) == 0x20);
    static_assert(offsetof(vp_assist_page_t, enlighten_vm_entry) == 0x28);
    static_assert(offsetof(vp_assist_page_t, current_nested_vmcs) == 0x30);
    static_assert(offsetof(vp_assist_page_t, synthetic_time_unhalted_timer_expired) == 0x38);
    static_assert(offsetof(vp_assist_page_t, virtualization_fault_information) == 0x40);
    static_assert(alignof(vp_assist_page_t) == 4_KiB_size_v);

    struct alignas(4_KiB_size_v) vmx_enlightened_vmcs_t {
        uint32_t version_number;
        uint32_t abort_indicator;

        uint16_t host_es_selector;
        uint16_t host_cs_selector;
        uint16_t host_ss_selector;
        uint16_t host_ds_selector;
        uint16_t host_fs_selector;
        uint16_t host_gs_selector;
        uint16_t host_tr_selector;

        uint64_t host_ia32_pat;
        uint64_t host_ia32_efer;
        uint64_t host_cr0;
        uint64_t host_cr3;
        uint64_t host_cr4;
        uint64_t host_ia32_sysenter_esp;
        uint64_t host_ia32_sysenter_eip;
        uint64_t host_rip;
        uint32_t host_ia32_sysenter_cs;

        uint32_t ctrl_pin_based_vm_execution_controls;
        uint32_t ctrl_primary_vmexit_controls;
        uint32_t ctrl_secondary_processor_based_vm_execution_controls;

        gpa_t ctrl_io_bitmap_a_address;
        gpa_t ctrl_io_bitmap_b_address;

        gpa_t ctrl_msr_bitmap_address;

        uint16_t guest_es_selector;
        uint16_t guest_cs_selector;
        uint16_t guest_ss_selector;
        uint16_t guest_ds_selector;
        uint16_t guest_fs_selector;
        uint16_t guest_gs_selector;
        uint16_t guest_ldtr_selector;
        uint16_t guest_tr_selector;

        uint32_t guest_es_limit;
        uint32_t guest_cs_limit;
        uint32_t guest_ss_limit;
        uint32_t guest_ds_limit;
        uint32_t guest_fs_limit;
        uint32_t guest_gs_limit;
        uint32_t guest_ldtr_limit;
        uint32_t guest_tr_limit;
        uint32_t guest_gdtr_limit;
        uint32_t guest_idtr_limit;

        uint32_t guest_es_attributes;
        uint32_t guest_cs_attributes;
        uint32_t guest_ss_attributes;
        uint32_t guest_ds_attributes;
        uint32_t guest_fs_attributes;
        uint32_t guest_gs_attributes;
        uint32_t guest_ldtr_attributes;
        uint32_t guest_tr_attributes;

        uint64_t guest_es_base;
        uint64_t guest_cs_base;
        uint64_t guest_ss_base;
        uint64_t guest_ds_base;
        uint64_t guest_fs_base;
        uint64_t guest_gs_base;
        uint64_t guest_ldtr_base;
        uint64_t guest_tr_base;
        uint64_t guest_gdtr_base;
        uint64_t guest_idtr_base;

        uint64_t reserved1[3];

        gpa_t ctrl_vmexit_msr_store_address;
        gpa_t ctrl_vmexit_msr_load_address;
        gpa_t ctrl_vmentry_msr_load_address;

        uint64_t ctrl_cr3_target_value0;
        uint64_t ctrl_cr3_target_value1;
        uint64_t ctrl_cr3_target_value2;
        uint64_t ctrl_cr3_target_value3;

        uint32_t ctrl_pagefault_error_code_mask;
        uint32_t ctrl_pagefault_error_code_match;
        uint32_t ctrl_cr3_target_count;

        uint32_t ctrl_vmexit_msr_store_count;
        uint32_t ctrl_vmexit_msr_load_count;
        uint32_t ctrl_vmentry_msr_load_count;

        uint64_t ctrl_tsc_offset;
        gpa_t ctrl_virtual_apic_address;

        gpa_t guest_vmcs_link_pointer;
        uint64_t guest_ia32_debug_ctl;
        uint64_t guest_ia32_pat;
        uint64_t guest_ia32_efer;
        uint64_t guest_pdpte0;
        uint64_t guest_pdpte1;
        uint64_t guest_pdpte2;
        uint64_t guest_pdpte3;
        uint64_t guest_pending_debug_exceptions;

        uint64_t guest_ia32_sysenter_esp;
        uint64_t guest_ia32_sysenter_eip;

        uint32_t guest_sleep_state;
        uint32_t guest_ia32_sysenter_cs;
        uint64_t ctrl_cr0_guest_host_mask;
        uint64_t ctrl_cr4_guest_host_mask;
        uint64_t ctrl_cr0_read_shadow;
        uint64_t ctrl_cr4_read_shadow;

        uint64_t guest_cr0;
        uint64_t guest_cr3;
        uint64_t guest_cr4;

        uint64_t guest_dr7;

        uint64_t host_fs_base;
        uint64_t host_gs_base;
        uint64_t host_tr_base;
        uint64_t host_gdtr_base;
        uint64_t host_idtr_base;

        uint64_t host_rsp;

        uint64_t ctrl_ept_pointer;
        uint16_t ctrl_vpid;

        uint16_t reserved2[3];
        uint64_t reserved3[5];

        uint64_t info_guest_physical_address;
        uint32_t info_vm_instruction_error;
        uint32_t info_exit_reason;
        uint32_t info_vmexit_interruption_info;
        uint32_t info_vmexit_exception_error_code;
        uint32_t info_idt_vectoring_info;
        uint32_t info_idt_vectoring_error_code;
        uint32_t info_vmexit_instruction_length;
        uint32_t info_vmexit_instruction_info;
        uint64_t info_exit_qualification;
        uint64_t info_io_rcx;
        uint64_t info_io_rsi;
        uint64_t info_io_rdi;
        uint64_t info_io_rip;
        uint64_t info_guest_linear_address;

        uint64_t guest_rsp;
        uint64_t guest_rflags;
        uint32_t guest_interruptibility_state;

        uint32_t ctrl_primary_processor_based_vm_execution_controls;
        uint32_t ctrl_exception_bitmap;
        uint32_t ctrl_vmentry_controls;
        uint32_t ctrl_vmentry_interruption_info;
        uint32_t ctrl_vmentry_exception_error_code;
        uint32_t ctrl_vmentry_instruction_length;
        uint32_t ctrl_tpr_threshold;
        uint64_t guest_rip;

        union {
            uint32_t storage;
            struct {
                uint32_t io_bitmap : 1;
                uint32_t msr_bitmap : 1;
                uint32_t control_grp2 : 1;
                uint32_t control_grp1 : 1;
                uint32_t control_proc : 1;
                uint32_t control_event : 1;
                uint32_t control_entry : 1;
                uint32_t control_excpn : 1;
                uint32_t crdr : 1;
                uint32_t control_xlat : 1;
                uint32_t guest_basic : 1;
                uint32_t guest_grp1 : 1;
                uint32_t guest_grp2 : 1;
                uint32_t host_pointer : 1;
                uint32_t host_grp1 : 1;
                uint32_t enlightenments_control : 1;
                uint32_t reserved : 16;
            } semantics;
        } mshv_clean_fields;

        uint32_t reserved4;

        uint32_t mshv_synthetic_controls;

        union {
            uint32_t storage;
            struct {
                uint32_t nested_flush_virtual_hypercall : 1;
                uint32_t msr_bitmap : 1;
                uint32_t reserved : 30;
            } semantics;
        } mshv_enlightenments_control;

        uint32_t mshv_vp_id;
        uint32_t reserved5;
        uint64_t mshv_vm_id;
        gpa_t mshv_partition_assist_page;
        uint64_t reserved6[4];

        uint64_t guest_ia32_bndcfgs;
        uint64_t guest_ia32_perf_global_ctrl;
        uint64_t guest_ia32_s_cet;
        uint64_t guest_ssp;
        uint64_t guest_ia32_interrupt_ssp_table_addr;
        uint64_t guest_ia32_lbr_ctl;
        uint64_t reserved7[2];

        uint64_t ctrl_xss_exiting_bitmap;
        uint64_t ctrl_encls_exiting_bitmap;
        uint64_t host_ia32_perf_global_ctrl;
        uint64_t ctrl_tsc_multiplier;
        uint64_t host_ia32_s_cet;
        uint64_t host_ssp;
        uint64_t host_ia32_interrupt_ssp_table_addr;
        uint64_t reserved8;
    };

    static_assert(sizeof(vmx_enlightened_vmcs_t) <= 4_KiB_size_v);
    static_assert(alignof(vmx_enlightened_vmcs_t) == 4_KiB_size_v);

    struct alignas(4_KiB_size_v) partition_assist_page_t {
        uint32_t tlb_lock_count;
    };

    static_assert(sizeof(partition_assist_page_t) <= 4_KiB_size_v);
    static_assert(alignof(partition_assist_page_t) == 4_KiB_size_v);
}
