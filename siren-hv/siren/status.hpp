#pragma once
#include <stdint.h>

namespace siren {
    struct status_t {
        uint32_t code;

        [[nodiscard]]
        constexpr bool operator==(const status_t& other) const noexcept = default;

        [[nodiscard]]
        constexpr bool operator!=(const status_t& other) const noexcept = default;

        [[nodiscard]]
        constexpr bool is_failure() const noexcept { code != 0; }

        [[nodiscard]]
        constexpr bool is_success() const noexcept { code == 0; }

        template<typename NativeErrTy>
        [[nodiscard]]
        NativeErrTy native_error() const noexcept;
    };

    inline constexpr status_t status_success_v = { 0 };
    inline constexpr status_t status_access_denied_v = { 1 };
    inline constexpr status_t status_invalid_argument_v = { 2 };
    inline constexpr status_t status_not_implemented_v = { 3 };
    inline constexpr status_t status_not_supported_v = { 4 };
    inline constexpr status_t status_not_exist_v = { 7 };
    inline constexpr status_t status_not_aligned_v = { 8 };
    inline constexpr status_t status_not_available_yet_v = { 9 };
    inline constexpr status_t status_insufficient_memory_v = { 10 };
    inline constexpr status_t status_buffer_too_small_v = { 11 };
    inline constexpr status_t status_ambiguous_memory_type_v = { 100 };
}
