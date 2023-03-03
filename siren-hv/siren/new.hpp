#pragma once
#include <new>

namespace siren {
    constexpr uint32_t pool_tag_v = 'vhrs';
}

[[nodiscard]]
void* operator new(std::size_t size, bool paged, std::align_val_t alignment, const std::nothrow_t&) noexcept;

//[[nodiscard]]
//void* operator new[](std::size_t size, bool paged, std::align_val_t alignment, const std::nothrow_t&) noexcept;
