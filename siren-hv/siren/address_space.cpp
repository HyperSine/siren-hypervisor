#include "address_space.hpp"
#include <ntddk.h>

namespace siren {
    uint64_t get_physical_address(uintptr_t virtual_address) noexcept {
        return MmGetPhysicalAddress(reinterpret_cast<void*>(virtual_address)).QuadPart;
    }

    uintptr_t get_virtual_address(uint64_t physical_address) noexcept {
        auto pa = PHYSICAL_ADDRESS{ .QuadPart = static_cast<decltype(PHYSICAL_ADDRESS::QuadPart)>(physical_address) };
        return reinterpret_cast<uintptr_t>(MmGetVirtualForPhysical(pa));
    }
}
