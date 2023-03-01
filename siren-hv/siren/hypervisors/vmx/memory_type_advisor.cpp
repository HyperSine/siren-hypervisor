#include "memory_type_advisor.hpp"
#include "../../x86/cpuid.hpp"
#include "../../x86/model_specific_registers.hpp"

namespace siren::hypervisors::vmx {

    [[nodiscard]]
    status_t memory_type_advisor::build_mtrr_descriptors(bool with_fixed_mtrr, uint32_t variable_mtrr_count, bool with_smrr, x86::physical_address_t max_physical_address, mtrr_descriptor_t mtrr_array[], uint32_t mtrr_array_length, uint32_t* return_length) noexcept {
        uint32_t mtrr_count = 0;

        if (with_fixed_mtrr) {
            x86::msr_t<x86::IA32_MTRR_FIX64K_00000> ia32_mtrr_fixYYk_ZZZZZ;

            ia32_mtrr_fixYYk_ZZZZZ.storage = x86::read_msr(x86::IA32_MTRR_FIX64K_00000);
            for (uint32_t i = 0; i < 8; ++i) {
                if (mtrr_count < mtrr_array_length) {
                    mtrr_array[mtrr_count].memory_type = ia32_mtrr_fixYYk_ZZZZZ.semantics.memory_type[i];
                    mtrr_array[mtrr_count].is_fixed = 1;
                    mtrr_array[mtrr_count].physical_base = x86::address_to_pfn(x86::physical_address_t{ 0x00000u + 0x10000u * i }, x86::for_4kb_page_t{});
                    mtrr_array[mtrr_count].physical_mask = 0xffffffffffff0000u;
                }
                ++mtrr_count;
            }

            for (uint32_t i = 0; i < 2; ++i) {
                ia32_mtrr_fixYYk_ZZZZZ.storage = x86::read_msr(x86::IA32_MTRR_FIX16K_80000 + i);
                for (uint32_t j = 0; j < 8; ++j) {
                    if (mtrr_count < mtrr_array_length) {
                        mtrr_array[mtrr_count].memory_type = ia32_mtrr_fixYYk_ZZZZZ.semantics.memory_type[i];
                        mtrr_array[mtrr_count].is_fixed = 1;
                        mtrr_array[mtrr_count].physical_base = x86::address_to_pfn(x86::physical_address_t{ 0x80000u + (0x4000u * 8u) * i + 0x4000u * j }, x86::for_4kb_page_t{});
                        mtrr_array[mtrr_count].physical_mask = 0xffffffffffffc000u;
                    }
                    ++mtrr_count;
                }
            }

            for (uint32_t i = 0; i < 8; ++i) {
                ia32_mtrr_fixYYk_ZZZZZ.storage = x86::read_msr(x86::IA32_MTRR_FIX4K_C0000 + i);
                for (uint32_t j = 0; j < 8; ++j) {
                    if (mtrr_count < mtrr_array_length) {
                        mtrr_array[mtrr_count].memory_type = ia32_mtrr_fixYYk_ZZZZZ.semantics.memory_type[i];
                        mtrr_array[mtrr_count].is_fixed = true;
                        mtrr_array[mtrr_count].physical_base = x86::address_to_pfn(x86::physical_address_t{ 0xc0000u + (0x1000u * 8u) * i + 0x1000u * j }, x86::for_4kb_page_t{});
                        mtrr_array[mtrr_count].physical_mask = 0xfffffffffffff000u;
                    }
                    ++mtrr_count;
                }
            }
        }

        for (uint32_t i = 0; i < variable_mtrr_count; ++i) {
            x86::msr_t<x86::IA32_MTRR_PHYSBASE0> ia32_mtrr_physbase;
            x86::msr_t<x86::IA32_MTRR_PHYSMASK0> ia32_mtrr_physmask;

            ia32_mtrr_physbase.storage = x86::read_msr(x86::IA32_MTRR_PHYSBASE0 + 2 * i);
            ia32_mtrr_physmask.storage = x86::read_msr(x86::IA32_MTRR_PHYSMASK0 + 2 * i);

            if (ia32_mtrr_physmask.semantics.valid && ia32_mtrr_physbase.semantics.memory_type != m_mtrr_default_memory_type.to_integral()) {
                if (mtrr_count < mtrr_array_length) {
                    mtrr_array[mtrr_count].memory_type = ia32_mtrr_physbase.semantics.memory_type;
                    mtrr_array[mtrr_count].is_fixed = false;
                    mtrr_array[mtrr_count].physical_base = ia32_mtrr_physbase.semantics.physical_base;
                    mtrr_array[mtrr_count].physical_mask = x86::pfn_to_address(ia32_mtrr_physmask.semantics.physical_mask, x86::for_4kb_page_t{}) | ~max_physical_address;
                }
                ++mtrr_count;
            }
        }

        if (with_smrr) {
            auto ia32_smrr_physbase = x86::read_msr<x86::IA32_SMRR_PHYSBASE>();
            auto ia32_smrr_physmask = x86::read_msr<x86::IA32_SMRR_PHYSMASK>();
            if (ia32_smrr_physmask.semantics.valid && ia32_smrr_physbase.semantics.memory_type != m_mtrr_default_memory_type.to_integral()) {
                if (mtrr_count < mtrr_array_length) {
                    mtrr_array[mtrr_count].memory_type = ia32_smrr_physbase.semantics.memory_type;
                    mtrr_array[mtrr_count].is_fixed = 0;
                    mtrr_array[mtrr_count].physical_base = ia32_smrr_physbase.semantics.physical_base;
                    mtrr_array[mtrr_count].physical_mask = x86::pfn_to_address(ia32_smrr_physmask.semantics.physical_mask, x86::for_4kb_page_t{}) | ~max_physical_address;
                }
                ++mtrr_count;
            }
        }

        if (return_length) {
            *return_length = mtrr_count;
        }

        return mtrr_count <= mtrr_array_length ? status_t::success_v : status_t::buffer_too_small_v;
    }

    memory_type_advisor::memory_type_advisor() noexcept :
        m_mtrr_default_memory_type(x86::memory_type_t::make_unspecified()),
        m_mtrr_descriptor_count(0),
        m_mtrr_descriptor_array(nullptr) {}

    memory_type_advisor::memory_type_advisor(memory_type_advisor&& other) noexcept :
        m_mtrr_default_memory_type(std::move(other.m_mtrr_default_memory_type)),
        m_mtrr_descriptor_count(std::move(other.m_mtrr_descriptor_count)),
        m_mtrr_descriptor_array(std::move(other.m_mtrr_descriptor_array))
    {
        other.m_mtrr_descriptor_count = 0;
        other.m_mtrr_descriptor_array = nullptr;
    }

    memory_type_advisor::~memory_type_advisor() noexcept {
        if (m_mtrr_descriptor_array) {
            default_npaged_pool{}.deallocate(m_mtrr_descriptor_array);
            m_mtrr_descriptor_array = nullptr;
            m_mtrr_descriptor_count = 0;
        }
    }

    [[nodiscard]]
    status_t memory_type_advisor::init() noexcept {
        auto status = status_t::success_v;

        uint32_t mtrr_descriptor_count = 0;
        mtrr_descriptor_t* mtrr_descriptor_array = nullptr;

        auto max_physical_address = x86::get_max_physical_address();

        // The availability of the MTRR feature is model-specific. Software can determine if MTRRs are supported on a
        // processor by executing the CPUID instruction and reading the state of the MTRR flag (bit 12) in the feature infor-
        // mation register (EDX).
        if (x86::cpuid<1>().semantics.edx.mtrr == 1) {
            auto ia32_mtrrcap = x86::read_msr<x86::IA32_MTRRCAP>();
            auto ia32_mtrr_def_type = x86::read_msr<x86::IA32_MTRR_DEF_TYPE>();

            // MTRRs are enabled when set; 
            // all MTRRs are disabled when clear, and the UC memory type is applied to all of physical memory.
            if (ia32_mtrr_def_type.semantics.mtrrs_enable == 1) {
                m_mtrr_default_memory_type = x86::memory_type_t::from_integral(ia32_mtrr_def_type.semantics.default_memory_type);

                auto with_fixed_mtrr = ia32_mtrrcap.semantics.fixed_range_mtrrs_support == 1 && ia32_mtrr_def_type.semantics.fixed_range_mtrrs_enable == 1;
                auto variable_mtrr_count = static_cast<uint32_t>(ia32_mtrrcap.semantics.variable_range_mtrrs_count);
                auto with_smrr = ia32_mtrrcap.semantics.smrr_support == 1;

                // discard return value, for it must be `status_t::buffer_too_small_v`
                static_cast<void>(build_mtrr_descriptors(with_fixed_mtrr, variable_mtrr_count, with_smrr, max_physical_address, nullptr, 0, &mtrr_descriptor_count));

                mtrr_descriptor_array = default_npaged_pool{}.allocate<mtrr_descriptor_t[]>(mtrr_descriptor_count);
                if (mtrr_descriptor_array == nullptr) {
                    status = status_t::insufficient_memory_v;
                    goto ON_FINAL;
                }

                status = build_mtrr_descriptors(with_fixed_mtrr, variable_mtrr_count, with_smrr, max_physical_address, mtrr_descriptor_array, mtrr_descriptor_count, nullptr);
                if (status.fails()) {
                    goto ON_FINAL;
                }
            } else {
                m_mtrr_default_memory_type = x86::memory_type_t::uncacheable_v;
            }
        } else {
            m_mtrr_default_memory_type = x86::memory_type_t::uncacheable_v;
        }

        std::swap(m_mtrr_descriptor_count, mtrr_descriptor_count);
        std::swap(m_mtrr_descriptor_array, mtrr_descriptor_array);

    ON_FINAL:
        if (mtrr_descriptor_array) {
            default_npaged_pool{}.deallocate(mtrr_descriptor_array);
            mtrr_descriptor_array = nullptr;
        }

        mtrr_descriptor_count = 0;

        return status;
    }

    [[nodiscard]]
    x86::memory_type_t memory_type_advisor::get_best_memory_type(x86::physical_address_t mem_base, x86::physical_address_t mem_mask) const noexcept {
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

        auto retval = x86::memory_type_t::make_unspecified();

        for (size_t i = 0; i < m_mtrr_descriptor_count; ++i) {
            if (m_mtrr_descriptor_array[i].contain_memory(mem_base, mem_mask)) {
                if (m_mtrr_descriptor_array[i].is_fixed) {
                    return x86::memory_type_t::from_integral(m_mtrr_descriptor_array[i].memory_type);
                }

                if (m_mtrr_descriptor_array[i].memory_type == x86::memory_type_t::uncacheable_v.to_integral()) {
                    return x86::memory_type_t::from_integral(m_mtrr_descriptor_array[i].memory_type);
                }

                if ((retval == x86::memory_type_t::write_through_v || retval == x86::memory_type_t::write_back_v) && m_mtrr_descriptor_array[i].memory_type == x86::memory_type_t::write_back_v.to_integral()) {
                    continue;
                } else {
                    retval = x86::memory_type_t::from_integral(m_mtrr_descriptor_array[i].memory_type);
                }
            } else if (m_mtrr_descriptor_array[i].disjoint_memory(mem_base, mem_mask)) {
                // just ignore
            } else {
                return x86::memory_type_t::make_unspecified();
            }
        }

        return retval.is_reserved() ? m_mtrr_default_memory_type : retval;
    }

}
