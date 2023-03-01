#pragma once
#include "siren_global.hpp"
#include <new>

[[nodiscard]]
void* operator new(std::size_t size, bool paged, std::align_val_t alignment, const std::nothrow_t&) noexcept;

//[[nodiscard]]
//void* operator new[](std::size_t size, bool paged, std::align_val_t alignment, const std::nothrow_t&) noexcept;
