#pragma once
#include <cstddef>
#include <cstdint>
#include <intrin.h>
#include <atomic>
#include <concepts>
#include <type_traits>
#include <utility>

#if defined(_MSC_VER)
#define SIREN_NOINLINE [[msvc::noinline]]
#define SIREN_ALWAYS_INLINE [[msvc::forceinline]]
#define SIREN_UNREACHABLE() __assume(0)
#elif defined(__GNUG__)
#define SIREN_NOINLINE [[gnu::noinline]]
#define SIREN_ALWAYS_INLINE [[gnu::always_inline]]
#define SIREN_UNREACHABLE() __builtin_unreachable()
#else
#error "siren_global.hpp: Unknown compiler."
#endif

namespace siren {

    struct status_t {
        uint32_t storage;

        static const status_t success_v;
        static const status_t access_denied_v;
        static const status_t invalid_argument_v;
        static const status_t not_implemented_v;
        static const status_t not_supported_v;
        static const status_t not_exist_v;
        static const status_t not_aligned_v;
        static const status_t not_available_yet_v;
        static const status_t insufficient_memory_v;
        static const status_t buffer_too_small_v;
        static const status_t ambiguous_memory_type_v;

        [[nodiscard]]
        constexpr bool operator==(const status_t& other) const noexcept = default;

        [[nodiscard]]
        constexpr bool operator!=(const status_t& other) const noexcept = default;

        [[nodiscard]]
        constexpr bool fails() const noexcept;

        [[nodiscard]]
        constexpr bool succeeds() const noexcept;

        [[nodiscard]]
        constexpr auto to_integral() const noexcept {
            return storage;
        }

        template<typename _NativeErrorTy>
        [[nodiscard]]
        _NativeErrorTy to_native_error() const noexcept;
    };

    inline constexpr status_t status_t::success_v = { 0 };
    inline constexpr status_t status_t::access_denied_v = { 1 };
    inline constexpr status_t status_t::invalid_argument_v = { 2 };
    inline constexpr status_t status_t::not_implemented_v = { 3 };
    inline constexpr status_t status_t::not_supported_v = { 4 };
    inline constexpr status_t status_t::not_exist_v = { 7 };
    inline constexpr status_t status_t::not_aligned_v = { 8 };
    inline constexpr status_t status_t::not_available_yet_v = { 9 };
    inline constexpr status_t status_t::insufficient_memory_v = { 10 };
    inline constexpr status_t status_t::buffer_too_small_v = { 11 };
    inline constexpr status_t status_t::ambiguous_memory_type_v = { 100 };

    [[nodiscard]]
    constexpr bool status_t::fails() const noexcept {
        return *this != success_v;
    }

    [[nodiscard]]
    constexpr bool status_t::succeeds() const noexcept {
        return *this == success_v;
    }

    template<typename _RetTy>
    struct result_wrapper_t {
        status_t status;
        _RetTy retval;
    };

    template<>
    struct result_wrapper_t<void> {
        status_t status;
    };

    void invoke_debugger() noexcept;

    void invoke_debugger_if(bool condition) noexcept;

    void invoke_debugger_if_not(bool condition) noexcept;

    [[noreturn]]
    void invoke_debugger_noreturn() noexcept;

    [[noreturn]]
    void raise_assertion_failure() noexcept;

    [[noreturn]]
    void raise_assertion_failure_if(bool condition) noexcept;

    [[noreturn]]
    void raise_assertion_failure_if_not(bool condition) noexcept;

    // -----------------------------
    // user-defined literal
    // -----------------------------

    [[nodiscard]]
    consteval size_t operator""_kb_size_v(unsigned long long int n) noexcept {
        constexpr uint64_t one_kb_size = uint64_t{ 1 } << 10u;
        constexpr uint64_t max_kb_magnitude = std::numeric_limits<uint64_t>::max() / one_kb_size;

        if (n <= max_kb_magnitude) {
            auto retval = n * one_kb_size;
            if (retval <= std::numeric_limits<size_t>::max()) {
                return static_cast<size_t>(retval);
            }
        }

        // return nothing at the final so that this user-defined literal fails at compiler-time
    }

    [[nodiscard]]
    consteval size_t operator""_mb_size_v(unsigned long long int n) noexcept {
        constexpr uint64_t one_mb_size = uint64_t{ 1 } << 20u;
        constexpr uint64_t max_mb_magnitude = std::numeric_limits<uint64_t>::max() / one_mb_size;

        if (n <= max_mb_magnitude) {
            auto retval = n * one_mb_size;
            if (retval <= std::numeric_limits<size_t>::max()) {
                return static_cast<size_t>(retval);
            }
        }
        
        // return nothing at the final so that this user-defined literal fails at compiler-time
    }

    [[nodiscard]]
    consteval size_t operator""_gb_size_v(unsigned long long int n) noexcept {
        constexpr uint64_t one_gb_size = uint64_t{ 1 } << 30u;
        constexpr uint64_t max_gb_magnitude = std::numeric_limits<uint64_t>::max() / one_gb_size;

        if (n <= max_gb_magnitude) {
            auto retval = n * one_gb_size;
            if (retval <= std::numeric_limits<size_t>::max()) {
                return static_cast<size_t>(retval);
            }
        }

        // return nothing at the final so that this user-defined literal fails at compiler-time
    }

    [[nodiscard]]
    consteval uint32_t operator""_kb_shift_v(unsigned long long int n) noexcept {
        if (n > 0 && std::has_single_bit(n)) {
            return std::countr_zero(n) + 10u;
        } else {
            // return nothing so that this user-defined literal fails at compiler-time
        }
    }

    [[nodiscard]]
    consteval uint32_t operator""_mb_shift_v(unsigned long long int n) noexcept {
        if (n > 0 && std::has_single_bit(n)) {
            return std::countr_zero(n) + 20u;
        } else {
            // return nothing so that this user-defined literal fails at compiler-time
        }
    }

    [[nodiscard]]
    consteval uint32_t operator""_gb_shift_v(unsigned long long int n) noexcept {
        if (n > 0 && std::has_single_bit(n)) {
            return std::countr_zero(n) + 30u;
        } else {
            // return nothing so that this user-defined literal fails at compiler-time
        }
    }

    [[nodiscard]]
    consteval uint64_t operator""_kb_mask_v(unsigned long long int n) noexcept {
        if (n > 0 && std::has_single_bit(n)) {
            return (uint64_t{ 1 } << 10u) * n - 1;
        } else {
            // return nothing so that this user-defined literal fails at compiler-time
        }
    }

    [[nodiscard]]
    consteval uint64_t operator""_mb_mask_v(unsigned long long int n) noexcept {
        if (n > 0 && std::has_single_bit(n)) {
            return (uint64_t{ 1 } << 20u) * n - 1;
        } else {
            // return nothing so that this user-defined literal fails at compiler-time
        }
    }

    [[nodiscard]]
    consteval uint64_t operator""_gb_mask_v(unsigned long long int n) noexcept {
        if (n > 0 && std::has_single_bit(n)) {
            return (uint64_t{ 1 } << 30u) * n - 1;
        } else {
            // return nothing so that this user-defined literal fails at compiler-time
        }
    }

    // -----------------------------
    // constants
    // -----------------------------

    constexpr uint32_t pool_tag_v = 'vhrs';

}
