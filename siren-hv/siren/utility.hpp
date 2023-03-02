#pragma once
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
    inline void unreachable() noexcept {
#if defined(__GNUC__) // GCC, Clang, ICC
        __builtin_unreachable();
#elif defined(_MSC_VER) // MSVC
        __assume(false);
#else
#error "siren::unreachable() is not implemented."
#endif
    }
#endif
}
