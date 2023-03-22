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
            requires (PageSize == 4_Kiuz || PageSize == 2_Miuz || PageSize == 1_Giuz)
        [[nodiscard]]
        static memory_type_t propose_for_page(paddr_t base) noexcept {
            constexpr paddr_t mask = PageSize - 1;
            return propose({ .base = base, .mask = mask });
        }

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
}
