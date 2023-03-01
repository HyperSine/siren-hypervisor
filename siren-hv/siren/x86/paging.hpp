#pragma once
#include "../siren_global.hpp"

namespace siren::x86 {
    using physical_address_t = uint64_t;

    using linear_address32_t = uint32_t;
    using linear_address_64_t = uint64_t;

    using logical_address_t = uintptr_t;
    using logical_address_32_t = uint32_t;
    using logical_address_64_t = uint64_t;

    using virtual_address_t = logical_address_t;
    using virtual_address_32_t = logical_address_32_t;
    using virtual_address_64_t = logical_address_64_t;

    struct _4kb_page_traits {
        static constexpr size_t page_size_v = 4_kb_size_v;
    };

    struct _2mb_page_traits {
        static constexpr size_t page_size_v = 2_mb_size_v;
    };

    struct _1gb_page_traits {
        static constexpr size_t page_size_v = 1_gb_size_v;
    };

    template<std::unsigned_integral _Ty>
    [[nodiscard]]
    constexpr _Ty address_to_pfn(_Ty address, _4kb_page_traits) noexcept {
        return static_cast<_Ty>(address >> 4_kb_shift_v);
    }

    template<std::unsigned_integral _Ty>
    [[nodiscard]]
    constexpr _Ty address_to_pfn(_Ty address, _2mb_page_traits) noexcept {
        return static_cast<_Ty>(address >> 2_mb_shift_v);
    }

    template<std::unsigned_integral _Ty>
    [[nodiscard]]
    constexpr _Ty address_to_pfn(_Ty address, _1gb_page_traits) noexcept {
        return static_cast<_Ty>(address >> 1_gb_shift_v);
    }

    template<std::unsigned_integral _Ty>
    [[nodiscard]]
    constexpr _Ty pfn_to_address(_Ty pfn, _4kb_page_traits) noexcept {
        return static_cast<_Ty>(pfn << 4_kb_shift_v);
    }

    template<std::unsigned_integral _Ty>
    [[nodiscard]]
    constexpr _Ty pfn_to_address(_Ty pfn, _2mb_page_traits) noexcept {
        return static_cast<_Ty>(pfn << 2_mb_shift_v);
    }

    template<std::unsigned_integral _Ty>
    [[nodiscard]]
    constexpr _Ty pfn_to_address(_Ty pfn, _1gb_page_traits) noexcept {
        return static_cast<_Ty>(pfn << 1_gb_shift_v);
    }

    [[nodiscard]]
    physical_address_t get_physical_address(virtual_address_t address) noexcept;

    [[nodiscard]]
    physical_address_t get_max_physical_address() noexcept;

    template<typename _Ty>
    [[nodiscard]]
    uint64_t get_physical_address(_Ty* ptr) noexcept {
        return get_physical_address(reinterpret_cast<uintptr_t>(ptr));
    }

    [[nodiscard]]
    virtual_address_t get_virtual_address(physical_address_t address) noexcept;

    template<typename _PtrTy = void*>
        requires(std::is_pointer_v<_PtrTy>)
    [[nodiscard]]
    _PtrTy get_virtual_address(physical_address_t address) noexcept {
        return reinterpret_cast<_PtrTy>(get_virtual_address(address));
    }

}
