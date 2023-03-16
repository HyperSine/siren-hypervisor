#pragma once
#include "paging.hpp"

namespace siren::x86 {
    struct memory_type_t {
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

        template<intmax_t PageSize>
        [[nodiscard]]
        static memory_type_t propose(paddr_t base, std::ratio<PageSize>) noexcept;

        template<typename Ty>
        [[nodiscard]]
        static constexpr memory_type_t cast_from(Ty value) noexcept {
            return memory_type_t{ static_cast<uint8_t>(value) };
        }
    };

    static_assert(std::is_aggregate_v<memory_type_t>);

    constexpr memory_type_t memory_type_uncacheable_v = { 0 };
    constexpr memory_type_t memory_type_write_combining_v = { 1 };
    constexpr memory_type_t memory_type_write_through_v = { 4 };
    constexpr memory_type_t memory_type_write_protected_v = { 5 };
    constexpr memory_type_t memory_type_write_back_v = { 6 };
    constexpr memory_type_t memory_type_reserved_v = { 0xff };

    [[nodiscard]]
    memory_type_t get_best_memory_type(paddr_t page_base, on_1GiB_page_t) noexcept;

    [[nodiscard]]
    memory_type_t get_best_memory_type(paddr_t page_base, on_2MiB_page_t) noexcept;

    [[nodiscard]]
    memory_type_t get_best_memory_type(paddr_t page_base, on_4KiB_page_t) noexcept;
}
