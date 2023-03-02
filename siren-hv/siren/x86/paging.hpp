#pragma once
#include <bit>
#include <concepts>
#include <ratio>
#include "address_space.hpp"
#include "../literals.hpp"

namespace siren::x86 {
    using namespace ::siren::size_literals;

    using on_4Kib_page_t = std::ratio<4_KiB_size_v>;
    using on_2MiB_page_t = std::ratio<2_MiB_size_v>;
    using on_1GiB_page_t = std::ratio<1_GiB_size_v>;

    template<std::unsigned_integral Ty>
    [[nodiscard]]
    constexpr Ty address_to_pfn(Ty address, on_4Kib_page_t) noexcept {
        constexpr uint32_t shift = std::countr_zero(4_KiB_size_v);
        return static_cast<Ty>(address >> shift);
    }

    template<std::unsigned_integral Ty>
    [[nodiscard]]
    constexpr Ty address_to_pfn(Ty address, on_2MiB_page_t) noexcept {
        constexpr uint32_t shift = std::countr_zero(2_MiB_size_v);
        return static_cast<Ty>(address >> 2_mb_shift_v);
    }

    template<std::unsigned_integral Ty>
    [[nodiscard]]
    constexpr Ty address_to_pfn(Ty address, on_1GiB_page_t) noexcept {
        constexpr uint32_t shift = std::countr_zero(1_GiB_size_v);
        return static_cast<Ty>(address >> shift);
    }

    template<std::unsigned_integral Ty>
    [[nodiscard]]
    constexpr Ty pfn_to_address(Ty pfn, on_4Kib_page_t) noexcept {
        constexpr uint32_t shift = std::countr_zero(4_KiB_size_v);
        return static_cast<Ty>(pfn << shift);
    }

    template<std::unsigned_integral Ty>
    [[nodiscard]]
    constexpr Ty pfn_to_address(Ty pfn, on_2MiB_page_t) noexcept {
        constexpr uint32_t shift = std::countr_zero(2_MiB_size_v);
        return static_cast<Ty>(pfn << shift);
    }

    template<std::unsigned_integral Ty>
    [[nodiscard]]
    constexpr Ty pfn_to_address(Ty pfn, on_1GiB_page_t) noexcept {
        constexpr uint32_t shift = std::countr_zero(1_GiB_size_v);
        return static_cast<Ty>(pfn << shift);
    }

    //[[nodiscard]]
    //physical_address_t get_physical_address(virtual_address_t address) noexcept;

    //[[nodiscard]]
    //physical_address_t get_max_physical_address() noexcept;

    //template<typename Ty>
    //[[nodiscard]]
    //uint64_t get_physical_address(Ty* ptr) noexcept {
    //    return get_physical_address(reinterpret_cast<uintptr_t>(ptr));
    //}

    //[[nodiscard]]
    //virtual_address_t get_virtual_address(physical_address_t address) noexcept;

    //template<typename _PtrTy = void*>
    //    requires(std::is_pointer_v<_PtrTy>)
    //[[nodiscard]]
    //_PtrTy get_virtual_address(physical_address_t address) noexcept {
    //    return reinterpret_cast<_PtrTy>(get_virtual_address(address));
    //}
}
