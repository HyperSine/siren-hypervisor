#pragma once
#include "../x86/cpuid.hpp"

namespace siren::x86 {
    template<>
    struct cpuid_result_t<0x40000000> {
        union {
            struct {
                int area[4];
            } storage;
            struct {
                uint32_t max_leaf_for_hypervisor_information;
                uint32_t vendor_name0;
                uint32_t vendor_name1;
                uint32_t vendor_name2;
            } semantics;
        };
    };

    static_assert(sizeof(cpuid_result_t<0x40000000>) == 16);
    static_assert(sizeof(cpuid_result_t<0x40000000>::storage) == sizeof(cpuid_result_t<0x40000000>::semantics));

    template<>
    struct cpuid_result_t<0x40000001> {
        union {
            struct {
                int area[4];
            } storage;
            struct {
                uint32_t vendor_neutral_interface;
                uint32_t reserved0;
                uint32_t reserved1;
                uint32_t reserved2;
            } semantics;
        };
    };

    static_assert(sizeof(cpuid_result_t<0x40000001>) == 16);
    static_assert(sizeof(cpuid_result_t<0x40000001>::storage) == sizeof(cpuid_result_t<0x40000001>::semantics));

    template<>
    struct cpuid_result_t<0x40000002> {
        union {
            struct {
                int area[4];
            } storage;
            struct {
                uint32_t build_number;
                uint32_t minor_version : 16;
                uint32_t major_version : 16;
                uint32_t service_pack;
                uint32_t service_number : 24;
                uint32_t service_branch : 8;
            } semantics;
        };
    };

    static_assert(sizeof(cpuid_result_t<0x40000002>) == 16);
    static_assert(sizeof(cpuid_result_t<0x40000002>::storage) == sizeof(cpuid_result_t<0x40000002>::semantics));

    template<>
    struct cpuid_result_t<0x40000003> {
        union {
            struct {
                int area[4];
            } storage;
            struct {
                uint64_t privilege_mask;
                uint32_t reserved0;
                uint32_t mwait_available : 1;   // deprecated
                uint32_t guest_debugging_support : 1;
                uint32_t performance_monitor_support : 1;
                uint32_t physical_cpu_dynamic_partitioning_support : 1;
                uint32_t xmm_fast_hypercall_input_support : 1;
                uint32_t virtual_guest_idle_state_support : 1;
                uint32_t hypervisor_sleep_state_support : 1;
                uint32_t querying_numa_distance_support : 1;
                uint32_t determining_timer_frequency_support : 1;
                uint32_t injecting_synthetic_machine_check_support : 1;
                uint32_t guest_crash_msr_support : 1;
                uint32_t debug_msr_support : 1;
                uint32_t npiep_support : 1;
                uint32_t disable_hypervisor_available : 1;
                uint32_t extended_gva_ranges_available_for_flush_virtual_address_list : 1;
                uint32_t xmm_fast_hypercall_output_support : 1;
                uint32_t reserved1 : 1;
                uint32_t sint_polling_mode_available : 1;
                uint32_t hypercall_msr_lock_available : 1;
                uint32_t use_direct_synthetic_timers : 1;
                uint32_t vsm_pat_register_available : 1;
                uint32_t vsm_bndcfgs_register_available : 1;
                uint32_t reserved2 : 1;
                uint32_t synthetic_time_unhalted_timer_support : 1;
                uint32_t reserved3 : 2;
                uint32_t use_intel_lbr_feature : 1;
                uint32_t reserved4 : 5;
            } semantics;
        };
    };

    static_assert(sizeof(cpuid_result_t<0x40000003>) == 16);
    static_assert(sizeof(cpuid_result_t<0x40000003>::storage) == sizeof(cpuid_result_t<0x40000003>::semantics));
}
