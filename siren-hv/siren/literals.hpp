#pragma once
#include <numeric>
#include <ratio>

namespace siren {
    namespace size_literals {
        [[nodiscard]]
        consteval std::size_t operator""_Kiuz(unsigned long long int n) noexcept {
            constexpr uint64_t one_KiB_size = uint64_t{1} << 10u;
            constexpr uint64_t max_KiB_magnitude = std::numeric_limits<size_t>::max() / one_KiB_size;

            if (n <= max_KiB_magnitude) {
                auto retval = n * one_KiB_size;
                if (retval <= std::numeric_limits<size_t>::max()) {
                    return static_cast<size_t>(retval);
                }
            }

            // return nothing at the final so that this user-defined literal fails at compiler-time
        }

        [[nodiscard]]
        consteval std::size_t operator""_Miuz(unsigned long long int n) noexcept {
            constexpr uint64_t one_MiB_size = uint64_t{1} << 20u;
            constexpr uint64_t max_MiB_magnitude = std::numeric_limits<uint64_t>::max() / one_MiB_size;

            if (n <= max_MiB_magnitude) {
                auto retval = n * one_MiB_size;
                if (retval <= std::numeric_limits<size_t>::max()) {
                    return static_cast<size_t>(retval);
                }
            }

            // return nothing at the final so that this user-defined literal fails at compiler-time
        }

        [[nodiscard]]
        consteval std::size_t operator""_Giuz(unsigned long long int n) noexcept {
            constexpr uint64_t one_GiB_size = uint64_t{ 1 } << 30u;
            constexpr uint64_t max_GiB_magnitude = std::numeric_limits<uint64_t>::max() / one_GiB_size;

            if (n <= max_GiB_magnitude) {
                auto retval = n * one_GiB_size;
                if (retval <= std::numeric_limits<size_t>::max()) {
                    return static_cast<size_t>(retval);
                }
            }

            // return nothing at the final so that this user-defined literal fails at compiler-time
        }
    }
}
