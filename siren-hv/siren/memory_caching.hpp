#pragma once
#include "../siren_global.hpp"
#include "paging.hpp"

namespace siren::x86 {

    struct memory_type_t {
        uint8_t storage;

        static const memory_type_t uncacheable_v;
        static const memory_type_t write_combining_v;
        static const memory_type_t write_through_v;
        static const memory_type_t write_protected_v;
        static const memory_type_t write_back_v;

        [[nodiscard]]
        constexpr bool operator==(const memory_type_t& other) const noexcept = default;

        [[nodiscard]]
        constexpr bool operator!=(const memory_type_t& other) const noexcept = default;

        [[nodiscard]]
        constexpr bool is_reserved() const noexcept;

        [[nodiscard]]
        constexpr auto to_integral() const noexcept {
            return storage;
        }

        template<std::unsigned_integral _Ty>
        [[nodiscard]]
        static constexpr auto from_integral(_Ty value) noexcept {
            return memory_type_t{ static_cast<uint8_t>(value) };
        }

        [[nodiscard]]
        static constexpr auto make_unspecified() noexcept {
            return memory_type_t{ 0xff };
        }
    };

    inline constexpr memory_type_t memory_type_t::uncacheable_v = { 0 };
    inline constexpr memory_type_t memory_type_t::write_combining_v = { 1 };
    inline constexpr memory_type_t memory_type_t::write_through_v = { 4 };
    inline constexpr memory_type_t memory_type_t::write_protected_v = { 5 };
    inline constexpr memory_type_t memory_type_t::write_back_v = { 6 };

    [[nodiscard]]
    constexpr bool memory_type_t::is_reserved() const noexcept {
        switch (storage) {
            case uncacheable_v.storage:
            case write_combining_v.storage:
            case write_through_v.storage:
            case write_protected_v.storage:
            case write_back_v.storage:
                return false;
            default:
                return true;
        }
    }

    [[nodiscard]]
    memory_type_t get_best_memory_type(physical_address_t page_base, _1gb_page_traits) noexcept;

    [[nodiscard]]
    memory_type_t get_best_memory_type(physical_address_t page_base, _2mb_page_traits) noexcept;

    [[nodiscard]]
    memory_type_t get_best_memory_type(physical_address_t page_base, _4kb_page_traits) noexcept;
}
