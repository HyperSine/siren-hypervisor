#pragma once
#include "../../siren_global.hpp"
#include "../../siren_memory.hpp"
#include "../../x86/memory_caching.hpp"
#include "../../x86/paging.hpp"

namespace siren::hypervisors::vmx {

    class memory_type_advisor {
    public:
        struct mtrr_descriptor_t {
            x86::physical_address_t memory_type : 8;
            x86::physical_address_t is_fixed : 1;
            x86::physical_address_t not_used : 3;
            x86::physical_address_t physical_base : 52;
            x86::physical_address_t physical_mask;

            [[nodiscard]]
            constexpr bool contain_memory(x86::physical_address_t mem_base, x86::physical_address_t mem_mask) const noexcept {
                auto union_mask = physical_mask | mem_mask;
                auto intersection_mask = physical_mask & mem_mask;
                return (x86::pfn_to_address(physical_base, x86::_4kb_page_traits{}) & intersection_mask) == (mem_base & intersection_mask) && union_mask == mem_mask;
            }

            [[nodiscard]]
            constexpr bool disjoint_memory(x86::physical_address_t mem_base, x86::physical_address_t mem_mask) const noexcept {
                auto intersection_mask = physical_mask & mem_mask;
                return (x86::pfn_to_address(physical_base, x86::_4kb_page_traits{}) & intersection_mask) != (mem_base & intersection_mask);
            }
        };

    private:
        x86::memory_type_t m_mtrr_default_memory_type;
        managed_object_ptr<mtrr_descriptor_t[]> m_mtrr_descriptors;

        [[nodiscard]]
        status_t build_mtrr_descriptors(bool with_fixed_mtrr, uint32_t variable_mtrr_count, bool with_smrr, x86::physical_address_t max_physical_address, mtrr_descriptor_t mtrr_array[], uint32_t mtrr_array_length, uint32_t* return_length) noexcept;

    public:
        memory_type_advisor() noexcept;

        memory_type_advisor(memory_type_advisor&& other) noexcept;

        memory_type_advisor& operator=(memory_type_advisor&& other) = delete;

        ~memory_type_advisor() noexcept;

        [[nodiscard]]
        status_t init() noexcept;

        [[nodiscard]]
        x86::memory_type_t get_best_memory_type(x86::physical_address_t mem_base, x86::physical_address_t mem_mask) const noexcept;
    };

}
