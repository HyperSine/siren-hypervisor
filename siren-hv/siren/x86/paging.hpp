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

    using vaddr_t = lgaddr_t;       // virtual address, Aka. logical address
    using vaddr32_t = lgaddr32_t;   // 32-bits virtual address
    using vaddr64_t = lgaddr64_t;   // 64-bits virtual address

    namespace address_literals {
        [[nodiscard]]
        consteval paddr_t operator""_paddr_v(unsigned long long int x) noexcept {
            if (x <= std::numeric_limits<paddr_t>::max()) {
                return static_cast<paddr_t>(x);
            }
        }

        [[nodiscard]]
        consteval paddr_t operator""_laddr32_v(unsigned long long int x) noexcept {
            if (x <= std::numeric_limits<laddr32_t>::max()) {
                return static_cast<laddr32_t>(x);
            }
        }

        [[nodiscard]]
        consteval paddr_t operator""_laddr64_v(unsigned long long int x) noexcept {
            if (x <= std::numeric_limits<laddr64_t>::max()) {
                return static_cast<laddr64_t>(x);
            }
        }

        [[nodiscard]]
        consteval paddr_t operator""_lgaddr_v(unsigned long long int x) noexcept {
            if (x <= std::numeric_limits<lgaddr_t>::max()) {
                return static_cast<lgaddr_t>(x);
            }
        }

        [[nodiscard]]
        consteval paddr_t operator""_lgaddr32_v(unsigned long long int x) noexcept {
            if (x <= std::numeric_limits<lgaddr32_t>::max()) {
                return static_cast<lgaddr32_t>(x);
            }
        }

        [[nodiscard]]
        consteval paddr_t operator""_lgaddr64_v(unsigned long long int x) noexcept {
            if (x <= std::numeric_limits<lgaddr64_t>::max()) {
                return static_cast<lgaddr64_t>(x);
            }
        }

        [[nodiscard]]
        consteval paddr_t operator""_vaddr_v(unsigned long long int x) noexcept {
            if (x <= std::numeric_limits<vaddr_t>::max()) {
                return static_cast<vaddr_t>(x);
            }
        }

        [[nodiscard]]
        consteval paddr_t operator""_vaddr32_v(unsigned long long int x) noexcept {
            if (x <= std::numeric_limits<vaddr32_t>::max()) {
                return static_cast<vaddr32_t>(x);
            }
        }

        [[nodiscard]]
        consteval paddr_t operator""_vaddr64_v(unsigned long long int x) noexcept {
            if (x <= std::numeric_limits<vaddr64_t>::max()) {
                return static_cast<vaddr64_t>(x);
            }
        }
    }

    using on_4KiB_page_t = std::ratio<4_Kiuz>;
    using on_4MiB_page_t = std::ratio<4_Miuz>;
    using on_2MiB_page_t = std::ratio<2_Miuz>;
    using on_1GiB_page_t = std::ratio<1_Giuz>;

    template<size_t PageSize, unsigned_integral_least<32> Ty>
    [[nodiscard]]
    constexpr Ty address_to_pfn(Ty address) noexcept {
        static_assert(std::has_single_bit(PageSize), "The size of a page must be non-zero and a power of 2.");
        static_assert(PageSize - 1 <= std::numeric_limits<Ty>::max(), "Too big page.");

        constexpr uint32_t shift = std::countr_zero(PageSize);

        return static_cast<Ty>(address >> shift);
    }

    template<size_t PageSize, unsigned_integral_least<32> Ty>
    [[nodiscard]]
    constexpr Ty pfn_to_address(Ty pfn) noexcept {
        static_assert(std::has_single_bit(PageSize), "The size of a page must be non-zero and a power of 2.");
        static_assert(PageSize - 1 <= std::numeric_limits<Ty>::max(), "Too big page.");

        constexpr uint32_t shift = std::countr_zero(PageSize);

        return static_cast<Ty>(pfn << shift);
    }

    template<size_t PageSize, unsigned_integral_least<32> Ty>
    [[nodiscard]]
    constexpr Ty page_offset(Ty address) noexcept {
        static_assert(std::has_single_bit(PageSize), "The size of a page must be non-zero and a power of 2.");
        static_assert(PageSize - 1 <= std::numeric_limits<Ty>::max(), "Too big page.");
        return address & static_cast<Ty>(PageSize - 1);
    }

    template<int Level, unsigned_integral_exact<32> Ty>
    [[nodiscard]]
    constexpr uint32_t pml_index(Ty address) noexcept {
        static_assert(1 <= Level && Level <= 2, "32-bits paging has only Page-Map-Level 1 ~ 2.");

        constexpr int shift = 12 + (Level - 1) * 10;
        constexpr uint32_t mask = (1u << 10u) - 1u;

        return static_cast<uint32_t>(address >> shift) & mask;
    }

    template<int Level, unsigned_integral_exact<64> Ty>
    [[nodiscard]]
    constexpr uint32_t pml_index(Ty address) noexcept {
        static_assert(1 <= Level && Level <= 5, "64-bits paging has only Page-Map-Level 1 ~ 5.");

        constexpr int shift = 12 + (Level - 1) * 9;
        constexpr uint32_t mask = (1u << 9u) - 1u;

        return static_cast<uint32_t>(address >> shift) & mask;
    }

    [[nodiscard]]
    constexpr laddr32_t make_address(on_4KiB_page_t, uint32_t pml2, uint32_t pml1, uint32_t offset) noexcept {
        constexpr uint32_t pml_mask = (1u << 10u) - 1;
        constexpr uint32_t offset_mask = static_cast<uint32_t>(4_Kiuz - 1);
        return laddr64_t{
            uint64_t{ offset & offset_mask } |
            uint64_t{ pml1 & pml_mask } << std::countr_zero(4_Kiuz) |
            uint64_t{ pml2 & pml_mask } << std::countr_zero(4_Miuz)
        };
    }

    [[nodiscard]]
    constexpr laddr64_t make_address(on_4KiB_page_t, uint32_t pml4, uint32_t pml3, uint32_t pml2, uint32_t pml1, uint32_t offset) noexcept {
        constexpr uint32_t pml_mask = (1u << 9u) - 1;
        constexpr uint32_t offset_mask = static_cast<uint32_t>(4_Kiuz - 1);
        return laddr64_t{
            uint64_t{ offset & offset_mask } |
            uint64_t{ pml1 & pml_mask } << std::countr_zero(4_Kiuz) |
            uint64_t{ pml2 & pml_mask } << std::countr_zero(2_Miuz) |
            uint64_t{ pml3 & pml_mask } << std::countr_zero(1_Giuz) |
            uint64_t{ pml4 & pml_mask } << std::countr_zero(512_Giuz)
        };
    }

    [[nodiscard]]
    constexpr laddr32_t make_address(on_2MiB_page_t, uint32_t pml2, uint32_t offset) noexcept {
        constexpr uint32_t pml_mask = (1u << 10u) - 1;
        constexpr uint32_t offset_mask = static_cast<uint32_t>(4_Miuz - 1);
        return laddr64_t{
            uint64_t{ offset & offset_mask } |
            uint64_t{ pml2 & pml_mask } << std::countr_zero(4_Miuz)
        };
    }

    [[nodiscard]]
    constexpr laddr64_t make_address(on_2MiB_page_t, uint32_t pml4, uint32_t pml3, uint32_t pml2, uint32_t offset) noexcept {
        constexpr uint32_t pml_mask = (1u << 9u) - 1;
        constexpr uint32_t offset_mask = static_cast<uint32_t>(2_Miuz - 1);
        return laddr64_t{
            uint64_t{ offset & offset_mask } |
            uint64_t{ pml2 & pml_mask } << std::countr_zero(2_Miuz) |
            uint64_t{ pml3 & pml_mask } << std::countr_zero(1_Giuz) |
            uint64_t{ pml4 & pml_mask } << std::countr_zero(512_Giuz)
        };
    }

    [[nodiscard]]
    constexpr laddr64_t make_address(on_1GiB_page_t, uint32_t pml4, uint32_t pml3, uint32_t offset) noexcept {
        constexpr uint32_t pml_mask = (1u << 9u) - 1;
        constexpr uint32_t offset_mask = static_cast<uint32_t>(1_Giuz - 1);
        return laddr64_t{
            uint64_t{ offset & offset_mask } |
            uint64_t{ pml3 & pml_mask } << std::countr_zero(1_Giuz) |
            uint64_t{ pml4 & pml_mask } << std::countr_zero(512_Giuz)
        };
    }

    [[nodiscard]]
    paddr_t get_max_physical_address() noexcept;
}
