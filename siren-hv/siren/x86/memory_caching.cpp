#include "memory_caching.hpp"
#include "cpuid.hpp"
#include "model_specific_registers.hpp"
#include "../utility.hpp"

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
    [[nodiscard]]
    static memory_type_t memory_type_propose(mask_region_t<paddr_t> region) noexcept {
        using namespace ::siren::x86::address_literals;
        if (0x10000_paddr_v <= region.base) {
            if (cpuid<1>().semantics.edx.mtrr == 0) {
                return memory_type_uncacheable_v;
            }

            paddr_t max_physical_address = get_max_physical_address();
            auto ia32_mtrrcap = read_msr<IA32_MTRRCAP>();
            auto ia32_mtrr_def_type = read_msr<IA32_MTRR_DEF_TYPE>();

            // all MTRRs are disabled when clear, and the UC memory type is applied to all of physical memory.
            if (ia32_mtrr_def_type.semantics.mtrrs_enable == 0) {
                return memory_type_uncacheable_v;
            }

            // MTRRs are enabled when set; 
            // all MTRRs are disabled when clear, and the UC memory type is applied to all of physical memory.
            auto mtrr_default_memory_type = memory_type_t::cast_from(ia32_mtrr_def_type.semantics.default_memory_type);
            auto mtrr_variable_count = static_cast<uint32_t>(ia32_mtrrcap.semantics.variable_range_mtrrs_count);

            auto memory_type_candidate = memory_type_reserved_v;
            
            for (uint32_t i = 0; i < mtrr_variable_count; ++i) {
                msr_t<IA32_MTRR_PHYSBASE0> ia32_mtrr_physbase;
                msr_t<IA32_MTRR_PHYSMASK0> ia32_mtrr_physmask;

                ia32_mtrr_physbase.storage = read_msr(IA32_MTRR_PHYSBASE0 + 2 * i);
                ia32_mtrr_physmask.storage = read_msr(IA32_MTRR_PHYSMASK0 + 2 * i);

                if (ia32_mtrr_physmask.semantics.valid) {
                    mask_region_t<paddr_t> mtrr_region{
                        .base = ia32_mtrr_physbase.semantics.physical_base,
                        .mask = pfn_to_address(ia32_mtrr_physmask.semantics.physical_mask, on_4KiB_page_t{}) | ~max_physical_address
                    };

                    if (mtrr_region.contains(region)) {
                        if (ia32_mtrr_physbase.semantics.memory_type == memory_type_uncacheable_v.value) {
                            return memory_type_uncacheable_v;
                        }

                        if (ia32_mtrr_physbase.semantics.memory_type == memory_type_write_back_v.value && (memory_type_candidate == memory_type_write_through_v || memory_type_candidate == memory_type_write_back_v)) {
                            continue;
                        } else {
                            memory_type_candidate = memory_type_t::cast_from(ia32_mtrr_physbase.semantics.memory_type);
                        }
                    } else if (mtrr_region.disjoints(region)) {
                        // just ignore
                    } else {
                        return memory_type_reserved_v;
                    }
                }
            }

            return memory_type_candidate.is_reserved() ? mtrr_default_memory_type : memory_type_candidate;
        } else {
            return memory_type_reserved_v;
        }
    }

    template<>
    memory_type_t memory_type_t::propose(paddr_t base, on_4KiB_page_t) noexcept {
        return memory_type_propose({ .base = base, .mask = ~(4_KiB_size_v - 1) });
    }

    template<>
    memory_type_t memory_type_t::propose(paddr_t base, on_2MiB_page_t) noexcept {
        return memory_type_propose({ .base = base, .mask = ~(2_MiB_size_v - 1) });
    }

    template<>
    memory_type_t memory_type_t::propose(paddr_t base, on_1GiB_page_t) noexcept {
        return memory_type_propose({ .base = base, .mask = ~(1_GiB_size_v - 1) });
    }
}
