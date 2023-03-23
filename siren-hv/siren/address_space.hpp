#pragma once
#include <stdint.h>
#include <type_traits>

namespace siren {
    [[nodiscard]]
    uint64_t get_physical_address(uintptr_t virtual_address) noexcept;

    template<typename PtrTy>
        requires std::is_pointer_v<PtrTy>
    [[nodiscard]]
    uint64_t get_physical_address(PtrTy ptr) noexcept {
        return get_physical_address(reinterpret_cast<uintptr_t>(ptr));
    }

    [[nodiscard]]
    uint64_t get_max_physical_address() noexcept;

    [[nodiscard]]
    uintptr_t get_virtual_address(uint64_t physical_address) noexcept;

    template<typename PtrTy>
        requires std::is_pointer_v<PtrTy>
    [[nodiscard]]
    PtrTy get_virtual_address(uint64_t physical_address) noexcept {
        return reinterpret_cast<PtrTy>(get_virtual_address(physical_address));
    }
}
