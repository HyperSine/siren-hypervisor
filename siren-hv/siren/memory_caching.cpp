#include "memory_caching.hpp"
#include "cpuid.hpp"
#include "model_specific_registers.hpp"

namespace siren::x86 {

    [[nodiscard]]
    static constexpr bool mtrr_contains_memory(physical_address_t mtrr_base, physical_address_t mtrr_mask, physical_address_t mem_base, physical_address_t mem_mask) noexcept {
        auto union_mask = mtrr_mask | mem_mask;
        auto intersection_mask = mtrr_mask & mem_mask;
        return (mtrr_base & intersection_mask) == (mem_base & intersection_mask) && union_mask == mem_mask;
    }

    [[nodiscard]]
    static constexpr bool mtrr_disjoints_memory(physical_address_t mtrr_base, physical_address_t mtrr_mask, physical_address_t mem_base, physical_address_t mem_mask) noexcept {
        auto intersection_mask = mtrr_mask & mem_mask;
        return (mtrr_base & intersection_mask) != (mem_base & intersection_mask);
    }

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
    memory_type_t get_best_memory_type(physical_address_t page_base, _1gb_page_traits) noexcept {
        if (64_kb_size_v <= page_base) {
            if (x86::cpuid<1>().semantics.edx.mtrr == 0) {
                return x86::memory_type_t::uncacheable_v;
            }

            auto max_physical_address = get_max_physical_address();
            auto ia32_mtrrcap = read_msr<IA32_MTRRCAP>();
            auto ia32_mtrr_def_type = read_msr<IA32_MTRR_DEF_TYPE>();

            // all MTRRs are disabled when clear, and the UC memory type is applied to all of physical memory.
            if (ia32_mtrr_def_type.semantics.mtrrs_enable == 0) {
                return x86::memory_type_t::uncacheable_v;
            }

            // MTRRs are enabled when set; 
            // all MTRRs are disabled when clear, and the UC memory type is applied to all of physical memory.
            auto mtrr_default_memory_type = x86::memory_type_t::from_integral(ia32_mtrr_def_type.semantics.default_memory_type);
            auto mtrr_variable_count = static_cast<uint32_t>(ia32_mtrrcap.semantics.variable_range_mtrrs_count);

            auto candidate_memory_type = x86::memory_type_t::make_unspecified();
            
            for (uint32_t i = 0; i < mtrr_variable_count; ++i) {
                msr_t<IA32_MTRR_PHYSBASE0> ia32_mtrr_physbase;
                msr_t<IA32_MTRR_PHYSMASK0> ia32_mtrr_physmask;

                ia32_mtrr_physbase.storage = x86::read_msr(x86::IA32_MTRR_PHYSBASE0 + 2 * i);
                ia32_mtrr_physmask.storage = x86::read_msr(x86::IA32_MTRR_PHYSMASK0 + 2 * i);

                if (ia32_mtrr_physmask.semantics.valid) {
                    physical_address_t mtrr_base = ia32_mtrr_physbase.semantics.physical_base;
                    physical_address_t mtrr_mask = x86::pfn_to_address(ia32_mtrr_physmask.semantics.physical_mask, _4kb_page_traits{}) | ~max_physical_address;

                    if (mtrr_contains_memory(mtrr_base, mtrr_mask, page_base, ~1_gb_mask_v)) {
                        if (ia32_mtrr_physbase.semantics.memory_type == x86::memory_type_t::uncacheable_v.to_integral()) {
                            return x86::memory_type_t::uncacheable_v;
                        }

                        if (ia32_mtrr_physbase.semantics.memory_type == x86::memory_type_t::write_back_v.to_integral() && (candidate_memory_type == x86::memory_type_t::write_through_v || candidate_memory_type == x86::memory_type_t::write_back_v)) {
                            continue;
                        } else {
                            candidate_memory_type = x86::memory_type_t::from_integral(ia32_mtrr_physbase.semantics.memory_type);
                        }
                    } else if (mtrr_disjoints_memory(mtrr_base, mtrr_mask, page_base, ~1_gb_mask_v)) {
                        // just ignore
                    } else {
                        return x86::memory_type_t::make_unspecified();
                    }
                }
            }

            return candidate_memory_type.is_reserved() ? mtrr_default_memory_type : candidate_memory_type;
        } else {
            return memory_type_t::make_unspecified();
        }
    }

    [[nodiscard]]
    memory_type_t get_best_memory_type(physical_address_t page_base, _2mb_page_traits) noexcept;

    [[nodiscard]]
    memory_type_t get_best_memory_type(physical_address_t page_base, _4kb_page_traits) noexcept {

    }

}
