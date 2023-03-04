#pragma once
#include <concepts>
#include <limits>
#include <type_traits>
#include <utility>

namespace siren {
#if __cpp_lib_to_underlying
    using std::to_underlying;
#else
    template<typename EnumTy>
    constexpr std::underlying_type_t<EnumTy> to_underlying(EnumTy e) noexcept {
        return static_cast<std::underlying_type_t<EnumTy>>(e);
    }
#endif

#if __cpp_lib_unreachable
    using std::unreachable;
#else
    [[noreturn]]
    constexpr void unreachable() noexcept {
#if defined(__GNUC__) // GCC, Clang, ICC
        __builtin_unreachable();
#elif defined(_MSC_VER) // MSVC
        __assume(false);
#else
#error "siren::unreachable() is not implemented."
#endif
    }
#endif

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

    template<std::unsigned_integral Ty, size_t From, size_t To>
        requires (From < To && To <= std::numeric_limits<Ty>::digits)
    struct range_bits_t {
        Ty storage;

        [[nodiscard]]
        static constexpr size_t length() noexcept {
            return To - From;
        }

        [[nodiscard]]
        constexpr Ty to_integral() const noexcept {
            constexpr Ty len = To - From;
            constexpr Ty mask = (1u << len) - 1u;
            constexpr Ty shift = From;
            return (storage & mask) << shift;
        }

        [[nodiscard]]
        static constexpr range_bits_t from_integral(Ty integral) noexcept {
            constexpr Ty len = To - From;
            constexpr Ty mask = (1u << len) - 1u;
            constexpr Ty shift = From;
            return { (integral >> shift) & mask };
        }
    };
}
