#include "memory_caching.hpp"
#include "cpuid.hpp"
#include "model_specific_registers.hpp"
#include "../synchronization.hpp"

namespace siren::x86 {
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 11 Memory Cache Control
    //    |-> 11.11 Memory Type Range Registers (MTRRs)
    //      |-> 11.11.4 Range Size and Alignment Requirement
    //        |-> 11.11.4.1 MTRR Precedences
    // 
    // If the MTRRs are enabled, then the memory type used for a memory access is determined as follows:
    // 
    // 1. If the physical address falls within the first 1 MByte of physical memory and fixed MTRRs are enabled, the
    //    processor uses the memory type stored for the appropriate fixed-range MTRR.
    // 
    // 2. Otherwise, the processor attempts to match the physical address with a memory type set by the variable-range MTRRs:
    //    - If one variable memory range matches, the processor uses the memory type stored in the IA32_MTRR_PHYSBASEn register for that range.
    //    - If two or more variable memory ranges match and the memory types are identical, then that memory type is used.
    //    - If two or more variable memory ranges match and one of the memory types is UC, the UC memory type used.
    //    - If two or more variable memory ranges match and the memory types are WT and WB, the WT memory type is used.
    //    - For overlaps not defined by the above rules, processor behavior is undefined.
    // 
    // 3. If no fixed or variable memory range matches, the processor uses the default memory type.
    memory_type_t memory_type_t::propose(mask_region_t<paddr_t> region) noexcept {
        using namespace ::siren::x86::address_literals;

        static once_flag once;

        static bool cpuid_mtrr_feature;
        static msr_t<IA32_MTRRCAP> ia32_mtrr_cap;
        static msr_t<IA32_MTRR_DEF_TYPE> ia32_mtrr_def_type;
        static msr_t<IA32_MTRR_FIX64K_00000> ia32_mtrr_fix64k_00000_80000;
        static msr_t<IA32_MTRR_FIX16K_80000> ia32_mtrr_fix16k_80000_c0000[2];
        static msr_t<IA32_MTRR_FIX4K_C0000> ia32_mtrr_fix4k_c0000_100000[8];
        static msr_t<IA32_MTRR_PHYSBASE0> ia32_mtrr_physbase_00_100[256];
        static msr_t<IA32_MTRR_PHYSMASK0> ia32_mtrr_physmask_00_100[256];

        once.call_once(
            []() {
                cpuid_mtrr_feature = cpuid<1>().semantics.edx.mtrr != 0;
                if (cpuid_mtrr_feature) {
                    ia32_mtrr_cap = read_msr<IA32_MTRRCAP>();
                    ia32_mtrr_def_type = read_msr<IA32_MTRR_DEF_TYPE>();

                    if (ia32_mtrr_def_type.semantics.fixed_range_mtrrs_enable) {
                        ia32_mtrr_fix64k_00000_80000 = read_msr<IA32_MTRR_FIX64K_00000>();

                        ia32_mtrr_fix16k_80000_c0000[0] = read_msr<IA32_MTRR_FIX16K_80000>();
                        ia32_mtrr_fix16k_80000_c0000[1].storage = read_msr<IA32_MTRR_FIX16K_A0000>().storage;

                        ia32_mtrr_fix4k_c0000_100000[0] = read_msr<IA32_MTRR_FIX4K_C0000>();
                        ia32_mtrr_fix4k_c0000_100000[1].storage = read_msr<IA32_MTRR_FIX4K_C8000>().storage;
                        ia32_mtrr_fix4k_c0000_100000[2].storage = read_msr<IA32_MTRR_FIX4K_D0000>().storage;
                        ia32_mtrr_fix4k_c0000_100000[3].storage = read_msr<IA32_MTRR_FIX4K_D8000>().storage;
                        ia32_mtrr_fix4k_c0000_100000[4].storage = read_msr<IA32_MTRR_FIX4K_E0000>().storage;
                        ia32_mtrr_fix4k_c0000_100000[5].storage = read_msr<IA32_MTRR_FIX4K_E8000>().storage;
                        ia32_mtrr_fix4k_c0000_100000[6].storage = read_msr<IA32_MTRR_FIX4K_F0000>().storage;
                        ia32_mtrr_fix4k_c0000_100000[7].storage = read_msr<IA32_MTRR_FIX4K_F8000>().storage;
                    }
                    
                    for (uint32_t i = 0, count = ia32_mtrr_cap.semantics.variable_range_mtrrs_count; i < count; ++i) {
                        ia32_mtrr_physbase_00_100[i].storage = read_msr(IA32_MTRR_PHYSBASE0 + 2 * i);
                        ia32_mtrr_physmask_00_100[i].storage = read_msr(IA32_MTRR_PHYSMASK0 + 2 * i);
                    }
                }
            }
        );

        if (cpuid_mtrr_feature == false) {
            return memory_type_uncacheable_v;
        }

        // all MTRRs are disabled when clear, and the UC memory type is applied to all of physical memory.
        if (ia32_mtrr_def_type.semantics.mtrrs_enable == 0) {
            return memory_type_uncacheable_v;
        }

        if (ia32_mtrr_def_type.semantics.fixed_range_mtrrs_enable && region.base < 0x10000_paddr_v) {
            for (uint32_t i = 0; i < 8; ++i) {
                constexpr paddr_t region_size = 64_Kiuz;
                
                mask_region_t<paddr_t> fix64k_region = 
                    { .base = 0x00000 + region_size * i, .mask = ~(region_size - 1) };

                if (fix64k_region.contains(region)) {
                    return memory_type_t{ ia32_mtrr_fix64k_00000_80000.semantics.memory_type[i] };
                }
            }

            for (uint32_t i = 0; i < 2; ++i) {
                for (uint32_t j = 0; j < 8; ++j) {
                    constexpr paddr_t region_size = 16_Kiuz;

                    mask_region_t<paddr_t> fix16k_region =
                        { .base = 0x80000_paddr_v + region_size * (8 * i + j), .mask = ~(region_size - 1)};

                    if (fix16k_region.contains(region)) {
                        return memory_type_t{ ia32_mtrr_fix16k_80000_c0000[i].semantics.memory_type[j]};
                    }
                }
            }

            for (uint32_t i = 0; i < 8; ++i) {
                for (uint32_t j = 0; j < 8; ++j) {
                    constexpr paddr_t region_size = 4_Kiuz;

                    mask_region_t<paddr_t> fix4k_region =
                        { .base = 0xc0000_paddr_v + region_size * (8 * i + j), .mask = ~(region_size - 1) };

                    if (fix4k_region.contains(region)) {
                        return memory_type_t{ ia32_mtrr_fix4k_c0000_100000[i].semantics.memory_type[j] };
                    }
                }
            }
        }

        auto memory_type_default = memory_type_t::cast_from(ia32_mtrr_def_type.semantics.default_memory_type);
        auto memory_type_candidate = memory_type_reserved_v;

        auto max_physical_address = get_max_physical_address();

        for (uint32_t i = 0, count = ia32_mtrr_cap.semantics.variable_range_mtrrs_count; i < count; ++i) {
            if (ia32_mtrr_physmask_00_100[i].semantics.valid) {
                auto memory_type_current =
                    memory_type_t::cast_from(ia32_mtrr_physbase_00_100[i].semantics.memory_type);

                mask_region_t<paddr_t> variable_region = {
                    .base = pfn_to_address<4_Kiuz>(ia32_mtrr_physbase_00_100[i].semantics.physical_base),
                    .mask = pfn_to_address<4_Kiuz>(ia32_mtrr_physmask_00_100[i].semantics.physical_mask) | ~max_physical_address
                };

                if (variable_region.contains(region)) {
                    if (memory_type_current == memory_type_uncacheable_v) {
                        return memory_type_uncacheable_v;
                    }

                    if (memory_type_current == memory_type_write_back_v && (memory_type_candidate == memory_type_write_through_v || memory_type_candidate == memory_type_write_back_v)) {
                        continue;
                    } else {
                        memory_type_candidate = memory_type_current;
                    }
                } else if (variable_region.disjoints(region)) {
                    // just ignore
                } else {
                    return memory_type_reserved_v;
                }
            }
        }

        return memory_type_candidate.is_reserved() ? memory_type_default : memory_type_candidate;
    }
}
