#pragma once
#include <bit>
#include <ratio>
#include "../concepts.hpp"
#include "../literals.hpp"

namespace siren::x86 {
    using namespace ::siren::size_literals;

    using paddr_t = uint64_t;

    using laddr32_t = uint32_t;     // 32-bits linear address
    using laddr64_t = uint64_t;     // 64-bits linear address

    using lgaddr_t = uintptr_t;     // logical address
    using lgaddr32_t = uint32_t;    // 32-bits logical address
    using lgaddr64_t = uint64_t;    // 64-bits logical address

    using vaddr_t = uintptr_t;      // virtual address, Aka. logical address
    using vaddr32_t = uint32_t;     // 32-bits virtual address
    using vaddr64_t = uint64_t;     // 64-bits virtual address

    using on_4KiB_page_t = std::ratio<4_KiB_size_v>;
    using on_4MiB_page_t = std::ratio<4_MiB_size_v>;
    using on_2MiB_page_t = std::ratio<2_MiB_size_v>;
    using on_1GiB_page_t = std::ratio<1_GiB_size_v>;

    template<one_of<laddr32_t, laddr64_t> Ty>
    [[nodiscard]]
    constexpr Ty address_to_pfn(Ty address, on_4KiB_page_t) noexcept {
        constexpr uint32_t shift = std::countr_zero(4_KiB_size_v);
        return static_cast<Ty>(address >> shift);
    }

    template<one_of<laddr32_t> Ty>
    [[nodiscard]]
    constexpr Ty address_to_pfn(Ty address, on_4MiB_page_t) noexcept {
        constexpr uint32_t shift = std::countr_zero(4_MiB_size_v);
        return static_cast<Ty>(address >> shift);
    }

    template<one_of<laddr64_t> Ty>
    [[nodiscard]]
    constexpr Ty address_to_pfn(Ty address, on_2MiB_page_t) noexcept {
        constexpr uint32_t shift = std::countr_zero(2_MiB_size_v);
        return static_cast<Ty>(address >> shift);
    }

    template<one_of<laddr64_t> Ty>
    [[nodiscard]]
    constexpr Ty address_to_pfn(Ty address, on_1GiB_page_t) noexcept {
        constexpr uint32_t shift = std::countr_zero(1_GiB_size_v);
        return static_cast<Ty>(address >> shift);
    }

    template<one_of<laddr32_t, laddr64_t> Ty>
    [[nodiscard]]
    constexpr Ty pfn_to_address(Ty pfn, on_4KiB_page_t) noexcept {
        constexpr uint32_t shift = std::countr_zero(4_KiB_size_v);
        return static_cast<Ty>(pfn << shift);
    }

    template<one_of<laddr32_t> Ty>
    [[nodiscard]]
    constexpr Ty pfn_to_address(Ty pfn, on_4MiB_page_t) noexcept {
        constexpr uint32_t shift = std::countr_zero(4_MiB_size_v);
        return static_cast<Ty>(pfn << shift);
    }

    template<one_of<laddr64_t> Ty>
    [[nodiscard]]
    constexpr Ty pfn_to_address(Ty pfn, on_2MiB_page_t) noexcept {
        constexpr uint32_t shift = std::countr_zero(2_MiB_size_v);
        return static_cast<Ty>(pfn << shift);
    }

    template<one_of<laddr64_t> Ty>
    [[nodiscard]]
    constexpr Ty pfn_to_address(Ty pfn, on_1GiB_page_t) noexcept {
        constexpr uint32_t shift = std::countr_zero(1_GiB_size_v);
        return static_cast<Ty>(pfn << shift);
    }

    template<one_of<laddr32_t, laddr64_t> Ty>
    [[nodiscard]]
    constexpr uint32_t page_offset(Ty address, on_4KiB_page_t) noexcept {
        constexpr uint32_t mask = 4_KiB_size_v - 1;
        return static_cast<uint32_t>(Ty & mask);
    }

    template<one_of<laddr32_t> Ty>
    [[nodiscard]]
    constexpr uint32_t page_offset(Ty address, on_4MiB_page_t) noexcept {
        constexpr uint32_t mask = 4_MiB_size_v - 1;
        return static_cast<uint32_t>(Ty & mask);
    }

    template<one_of<laddr64_t> Ty>
    [[nodiscard]]
    constexpr uint32_t page_offset(Ty address, on_2MiB_page_t) noexcept {
        constexpr uint32_t mask = 2_MiB_size_v - 1;
        return static_cast<uint32_t>(Ty & mask);
    }

    template<one_of<laddr64_t> Ty>
    [[nodiscard]]
    constexpr uint32_t page_offset(Ty address, on_1GiB_page_t) noexcept {
        constexpr uint32_t mask = 1_GiB_size_v - 1;
        return static_cast<uint32_t>(Ty & mask);
    }

    template<int Level>
        requires(1 <= Level && Level <= 2)    // 32-bits paging only has Page-Map-Level 1 ~ 2
    [[nodiscard]]
    constexpr uint32_t index_of_pml(laddr32_t address) noexcept {
        constexpr uint32_t pml_mask = (1u << 10u) - 1;
        return static_cast<uint32_t>((address >> (12u + (Level - 1u) * 10u)) & pml_mask);
    }

    template<int Level>
        requires(1 <= Level && Level <= 5)    // 64-bits paging has 5 Page-Map-Levels at most
    [[nodiscard]]
    constexpr uint32_t index_of_pml(laddr64_t address) noexcept {
        constexpr uint32_t pml_mask = (1u << 9u) - 1;
        return static_cast<uint32_t>((address >> (12u + (Level - 1u) * 9u)) & pml_mask);
    }

    [[nodiscard]]
    constexpr laddr32_t make_address(on_4KiB_page_t, uint32_t pml2, uint32_t pml1, uint32_t offset) noexcept {
        constexpr uint32_t pml_mask = (1u << 10u) - 1;
        constexpr uint32_t offset_mask = static_cast<uint32_t>(4_KiB_size_v - 1);
        return laddr64_t{
            uint64_t{ offset & offset_mask } |
            uint64_t{ pml1 & pml_mask } << std::countr_zero(4_KiB_size_v) |
            uint64_t{ pml2 & pml_mask } << std::countr_zero(4_MiB_size_v)
        };
    }

    [[nodiscard]]
    constexpr laddr64_t make_address(on_4KiB_page_t, uint32_t pml4, uint32_t pml3, uint32_t pml2, uint32_t pml1, uint32_t offset) noexcept {
        constexpr uint32_t pml_mask = (1u << 9u) - 1;
        constexpr uint32_t offset_mask = static_cast<uint32_t>(4_KiB_size_v - 1);
        return laddr64_t{
            uint64_t{ offset & offset_mask } |
            uint64_t{ pml1 & pml_mask } << std::countr_zero(4_KiB_size_v) |
            uint64_t{ pml2 & pml_mask } << std::countr_zero(2_MiB_size_v) |
            uint64_t{ pml3 & pml_mask } << std::countr_zero(1_GiB_size_v) |
            uint64_t{ pml4 & pml_mask } << std::countr_zero(512_GiB_size_v)
        };
    }

    [[nodiscard]]
    constexpr laddr32_t make_address(on_2MiB_page_t, uint32_t pml2, uint32_t offset) noexcept {
        constexpr uint32_t pml_mask = (1u << 10u) - 1;
        constexpr uint32_t offset_mask = static_cast<uint32_t>(4_MiB_size_v - 1);
        return laddr64_t{
            uint64_t{ offset & offset_mask } |
            uint64_t{ pml2 & pml_mask } << std::countr_zero(4_MiB_size_v)
        };
    }

    [[nodiscard]]
    constexpr laddr64_t make_address(on_2MiB_page_t, uint32_t pml4, uint32_t pml3, uint32_t pml2, uint32_t offset) noexcept {
        constexpr uint32_t pml_mask = (1u << 9u) - 1;
        constexpr uint32_t offset_mask = static_cast<uint32_t>(2_MiB_size_v - 1);
        return laddr64_t{
            uint64_t{ offset & offset_mask } |
            uint64_t{ pml2 & pml_mask } << std::countr_zero(2_MiB_size_v) |
            uint64_t{ pml3 & pml_mask } << std::countr_zero(1_GiB_size_v) |
            uint64_t{ pml4 & pml_mask } << std::countr_zero(512_GiB_size_v)
        };
    }

    [[nodiscard]]
    constexpr laddr64_t make_address(on_1GiB_page_t, uint32_t pml4, uint32_t pml3, uint32_t offset) noexcept {
        constexpr uint32_t pml_mask = (1u << 9u) - 1;
        constexpr uint32_t offset_mask = static_cast<uint32_t>(1_GiB_size_v - 1);
        return laddr64_t{
            uint64_t{ offset & offset_mask } |
            uint64_t{ pml3 & pml_mask } << std::countr_zero(1_GiB_size_v) |
            uint64_t{ pml4 & pml_mask } << std::countr_zero(512_GiB_size_v)
        };
    }
}
