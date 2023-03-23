#pragma once
#include "paging.hpp"
#include "../utility.hpp"

namespace siren::x86 {
    struct memory_type_t {
    public:
        uint8_t value;

        [[nodiscard]]
        friend constexpr bool operator==(const memory_type_t& lhs, const memory_type_t& rhs) noexcept = default;

        [[nodiscard]]
        friend constexpr bool operator!=(const memory_type_t& lhs, const memory_type_t& rhs) noexcept = default;

        [[nodiscard]]
        constexpr bool is_reserved() const noexcept {
            switch (value) {
                case 0:
                case 1:
                case 4:
                case 5:
                case 6:
                    return false;
                default:
                    return true;
            }
        }

    private:
        static memory_type_t propose(mask_region_t<paddr_t> region) noexcept;

    public:
        template<size_t PageSize>
        [[nodiscard]]
        static memory_type_t propose_for_page(paddr_t base) noexcept {
            static_assert(std::has_single_bit(PageSize), "The size of a page must be non-zero and a power of 2.");
            static_assert(PageSize - 1 <= std::numeric_limits<paddr_t>::max(), "Too big page.");
            return propose({ .base = base, .mask = ~static_cast<paddr_t>(PageSize - 1) });
        }

        template<typename Ty>
        [[nodiscard]]
        static constexpr memory_type_t cast_from(Ty value) noexcept {
            return memory_type_t{ static_cast<uint8_t>(value) };
        }
    };

    static_assert(sizeof(memory_type_t) == sizeof(uint8_t));
    static_assert(alignof(memory_type_t) == alignof(uint8_t));
    static_assert(std::is_aggregate_v<memory_type_t>);

    constexpr memory_type_t memory_type_uncacheable_v = { 0 };
    constexpr memory_type_t memory_type_write_combining_v = { 1 };
    constexpr memory_type_t memory_type_write_through_v = { 4 };
    constexpr memory_type_t memory_type_write_protected_v = { 5 };
    constexpr memory_type_t memory_type_write_back_v = { 6 };
    constexpr memory_type_t memory_type_reserved_v = { 0xff };
}
