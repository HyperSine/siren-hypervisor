#pragma once
#include <concepts>
#include <type_traits>
#include <new>
#include <limits>
#include <functional>
#include <utility>

namespace siren {
    template<std::unsigned_integral Ty>
    [[nodiscard]]
    constexpr bool align_check(Ty value, std::align_val_t alignment) noexcept {
        size_t mask = to_underlying(alignment) - 1;
        return (value & mask) == 0;
    }

    template<typename Ty>
        requires std::is_pointer_v<Ty>
    [[nodiscard]]
    constexpr bool align_check(Ty ptr, std::align_val_t alignment) noexcept {
        return align_check(reinterpret_cast<uintptr_t>(ptr), alignment);
    }

    template<std::unsigned_integral Ty>
    [[nodiscard]]
    constexpr Ty align_up(Ty value, std::align_val_t alignment) noexcept {
        size_t mask = to_underlying(alignment) - 1;
        return static_cast<Ty>((value + mask) & ~mask);
    }

    template<typename Ty>
        requires std::is_pointer_v<Ty>
    [[nodiscard]]
    constexpr Ty align_up(Ty ptr, std::align_val_t alignment) noexcept {
        return reinterpret_cast<Ty>(align_up(reinterpret_cast<uintptr_t>(ptr), alignment));
    }

    template<std::unsigned_integral Ty>
    [[nodiscard]]
    constexpr Ty align_down(Ty value, std::align_val_t alignment) noexcept {
        size_t mask = to_underlying(alignment) - 1;
        return static_cast<Ty>(value & ~mask);
    }

    template<typename Ty>
        requires std::is_pointer_v<Ty>
    [[nodiscard]]
    constexpr Ty align_down(Ty ptr, std::align_val_t alignment) noexcept {
        return reinterpret_cast<Ty>(align_down(reinterpret_cast<uintptr_t>(ptr), alignment));
    }

    template<typename Ty, bool LeftInclusive, bool RightInclusive>
    struct interval_t {
        Ty min;
        Ty max;

        [[nodiscard]]
        constexpr Ty length() const noexcept requires std::integral<Ty>;    // todo

        [[nodiscard]]
        constexpr bool empty() const noexcept;  // todo

        [[nodiscard]]
        constexpr bool contains(Ty value) const noexcept {
            using left_comparer = std::conditional_t<LeftInclusive, std::less_equal<Ty>, std::less<Ty>>;
            using right_comparer = std::conditional_t<RightInclusive, std::less_equal<Ty>, std::less<Ty>>;
            return left_comparer{}(min, value) && right_comparer {}(value, max);
        }
    };

    template<typename Ty>
    using open_interval_t = interval_t<Ty, false, false>;

    template<typename Ty>
    using closed_interval_t = interval_t<Ty, true, true>;

    template<typename Ty>
    using left_closed_half_interval_t = interval_t<Ty, true, false>;

    template<typename Ty>
    using right_closed_half_interval_t = interval_t<Ty, true, false>;

    template<std::unsigned_integral Ty, size_t From, size_t To>
        requires (From < To && To <= std::numeric_limits<Ty>::digits)
    struct range_bits_t {
        Ty storage;

        [[nodiscard]]
        constexpr Ty value() const noexcept {
            return storage;
        }

        [[nodiscard]]
        static constexpr size_t length() noexcept {
            return To - From;
        }

        [[nodiscard]]
        constexpr Ty as_integral() const noexcept {
            constexpr Ty len = static_cast<Ty>(To - From);
            constexpr Ty mask = (1u << len) - 1u;
            constexpr Ty shift = From;
            return (storage & mask) << shift;
        }

        constexpr Ty& assign_to(Ty& integral) const noexcept {
            constexpr Ty len = static_cast<Ty>(To - From);
            constexpr Ty mask = (1u << len) - 1u;
            constexpr Ty shift = From;
            integral = (integral & ~(mask << shift)) | ((storage & mask) << shift);
            return integral;
        }

        [[nodiscard]]
        static constexpr range_bits_t extract_from(Ty integral) noexcept {
            constexpr Ty len = static_cast<Ty>(To - From);
            constexpr Ty mask = (1u << len) - 1u;
            constexpr Ty shift = From;
            return { (integral >> shift) & mask };
        }
    };
}
