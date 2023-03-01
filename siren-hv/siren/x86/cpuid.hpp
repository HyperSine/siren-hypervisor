#pragma once
#include "../siren_global.hpp"

namespace siren::x86 {
    template<uint32_t... leafs>
    struct cpuid_result_t;

    template<>
    struct cpuid_result_t<> {
        union {
            struct {
                int data[4];
            } storage;
            struct {
                uint32_t eax;
                uint32_t ebx;
                uint32_t ecx;
                uint32_t edx;
            } semantics;
        };
    };

    static_assert(sizeof(cpuid_result_t<>) == 16);
    static_assert(sizeof(cpuid_result_t<>::storage) == sizeof(cpuid_result_t<>::semantics));

    // Defined in
    // [*] Volume 2 (2A, 2B, 2C & 2D): Instruction Set Reference, A-Z
    //  |-> Chapter 3 Instruction Set Reference Pages
    //    |-> 3.2 Instructions (A-L)
    //      |-> CPUID--CPU Identification
    //        |-> Table 3-8. Information Returned by CPUID Instruction
    template<>
    struct cpuid_result_t<0> {
        union {
            struct {
                int area[4];
            } storage;
            struct {
                uint32_t max_leaf_for_basic_cpuid_information;
                uint32_t vendor_name0;
                uint32_t vendor_name2;
                uint32_t vendor_name1;
            } semantics;
        };
    };

    static_assert(sizeof(cpuid_result_t<0>) == 16);
    static_assert(sizeof(cpuid_result_t<0>::storage) == sizeof(cpuid_result_t<0>::semantics));

    // Defined in
    // [*] Volume 2 (2A, 2B, 2C & 2D): Instruction Set Reference, A-Z
    //  |-> Chapter 3 Instruction Set Reference Pages
    //    |-> 3.2 Instructions (A-L)
    //      |-> CPUID--CPU Identification
    //        |-> Table 3-8. Information Returned by CPUID Instruction
    //            Figure 3-6. Version Information Returned by CPUID in EAX
    //            Table 3-10. Feature Information Returned in the ECX Register
    //            Table 3-11. More on Feature Information Returned in the EDX Register
    template<>
    struct cpuid_result_t<1> {
        union {
            struct {
                int area[4];
            } storage;
            struct {
                struct {
                    uint32_t stepping_id : 4;
                    uint32_t model : 4;
                    uint32_t family_id : 4;
                    uint32_t processor_type : 2;
                    uint32_t reserved0 : 2;
                    uint32_t extended_model_id : 4;
                    uint32_t extended_family_id : 8;
                    uint32_t reserved1 : 4;
                } eax;
                struct {
                    uint32_t brand_index : 8;
                    uint32_t clflush_line_size : 8;
                    uint32_t max_number_of_logical_processors_addressable_ids : 8;
                    uint32_t initial_apic_id : 8;
                } ebx;
                struct {
                    uint32_t sse3 : 1;
                    uint32_t pclmulqdq : 1;
                    uint32_t ds_area_with_64bit_layout : 1;
                    uint32_t monitor_mwait : 1;
                    uint32_t cpl_qualified_debug_store : 1;
                    uint32_t virtual_machine_extension : 1;
                    uint32_t safe_mode_extension : 1;
                    uint32_t enhanced_intel_speedstep_technology : 1;
                    uint32_t thermal_monitor2 : 1;
                    uint32_t ssse3 : 1;
                    uint32_t l1_context_id : 1;
                    uint32_t sdbg : 1;
                    uint32_t fma : 1;
                    uint32_t cmpxchg16b : 1;
                    uint32_t xtpr_update_control : 1;
                    uint32_t perfmon_and_debug_capability : 1;
                    uint32_t reserved : 1;
                    uint32_t pcid : 1;
                    uint32_t dca : 1;
                    uint32_t sse4_1 : 1;
                    uint32_t sse4_2 : 1;
                    uint32_t x2apic : 1;
                    uint32_t movbe : 1;
                    uint32_t popcnt : 1;
                    uint32_t tsc_deadline : 1;
                    uint32_t aesni : 1;
                    uint32_t xsave : 1;
                    uint32_t osxsave : 1;
                    uint32_t avx : 1;
                    uint32_t f16c : 1;
                    uint32_t rdrand : 1;
                    uint32_t not_used : 1;
                } ecx;
                struct {
                    uint32_t fpu : 1;
                    uint32_t virtual_8086_mode_enhancement : 1;
                    uint32_t debugging_extension : 1;
                    uint32_t page_size_extension : 1;
                    uint32_t tsc_support : 1;
                    uint32_t msr_support : 1;
                    uint32_t physical_address_extension : 1;
                    uint32_t machine_check_exception : 1;
                    uint32_t cmpxchg8b : 1;
                    uint32_t apic_on_chip : 1;
                    uint32_t reserved0 : 1;
                    uint32_t sysenter_sysexit : 1;
                    uint32_t mtrr : 1;
                    uint32_t page_global_bit_extension : 1;
                    uint32_t machine_check_architecture : 1;
                    uint32_t cmov : 1;
                    uint32_t page_attribute_table : 1;
                    uint32_t page_size_extension_36bit : 1;
                    uint32_t processor_serial_number : 1;
                    uint32_t clflush : 1;
                    uint32_t reserved1 : 1;
                    uint32_t debug_store : 1;
                    uint32_t acpi : 1;
                    uint32_t mmx : 1;
                    uint32_t fxsave_fxrstor : 1;
                    uint32_t sse : 1;
                    uint32_t sse2 : 1;
                    uint32_t self_snoop : 1;
                    uint32_t htt : 1;
                    uint32_t termal_monitor : 1;
                    uint32_t reserved2 : 1;
                    uint32_t pending_break_enable : 1;
                } edx;
            } semantics;
        };
    };

    static_assert(sizeof(cpuid_result_t<1>) == 16);
    static_assert(sizeof(cpuid_result_t<1>::storage) == sizeof(cpuid_result_t<1>::semantics));

    // Defined in
    // [*] Volume 2 (2A, 2B, 2C & 2D): Instruction Set Reference, A-Z
    //  |-> Chapter 3 Instruction Set Reference, A-L
    //    |-> 3.2 Instructions (A-L)
    //      |-> CPUID--CPU Identification
    //        |-> Table 3-8. Information Returned by CPUID Instruction
    template<>
    struct cpuid_result_t<0x80000000> {
        union {
            struct {
                int area[4];
            } storage;
            struct {
                uint32_t max_leaf_for_extended_cpuid_information;
                uint32_t reserved0;
                uint32_t reserved1;
                uint32_t reserved2;
            } semantics;
        };
    };

    static_assert(sizeof(cpuid_result_t<0x80000000>) == 16);
    static_assert(sizeof(cpuid_result_t<0x80000000>::storage) == sizeof(cpuid_result_t<0x80000000>::semantics));

    template<>
    struct cpuid_result_t<0x80000008> {
        union {
            struct {
                int area[4];
            } storage;
            struct {
                uint32_t physical_address_bits : 8;
                uint32_t linear_address_bits : 8;
                uint32_t reserved0 : 16;
                uint32_t reserved1 : 9;
                uint32_t wbnoinvd_available : 1;
                uint32_t reserved2 : 22;
                uint32_t reserved3;
                uint32_t reserved4;
            } semantics;
        };
    };

    static_assert(sizeof(cpuid_result_t<0x80000008>) == 16);
    static_assert(sizeof(cpuid_result_t<0x80000008>::storage) == sizeof(cpuid_result_t<0x80000008>::semantics));

    [[nodiscard]]
    inline cpuid_result_t<> cpuid(uint32_t eax) noexcept {
        cpuid_result_t<> cpuid_result = {};
        __cpuid(cpuid_result.storage.data, eax);
        return cpuid_result;
    }

    [[nodiscard]]
    inline cpuid_result_t<> cpuid(uint32_t eax, uint32_t ecx) noexcept {
        cpuid_result_t<> cpuid_result = {};
        __cpuidex(cpuid_result.storage.data, eax, ecx);
        return cpuid_result;
    }

    template<uint32_t... leafs>
    [[nodiscard]]
    cpuid_result_t<leafs...> cpuid() noexcept {
        cpuid_result_t<leafs...> cpuid_result = {};

        if constexpr (sizeof...(leafs) == 1) {
            __cpuid(cpuid_result.storage.area, leafs...);
        } else if constexpr (sizeof...(leafs) == 2) {
            __cpuidex(cpuid_result.storage.area, leafs...);
        } else {
            static_assert(sizeof...(leafs) == 1 || sizeof...(leafs) == 2, "cpuid(): Two leafs at most!");
        }

        return cpuid_result;
    }
}
