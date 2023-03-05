#pragma once
#include <specstrings.h>
#include <kernelspecs.h>
#include <new>

namespace siren {
    constexpr uint32_t pool_tag_v = 'vhrs';
}

_When_(paged == true, _IRQL_requires_max_(APC_LEVEL))
_When_(paged == false, _IRQL_requires_max_(DISPATCH_LEVEL))
[[nodiscard]]
void* operator new(std::size_t size, bool paged, std::align_val_t alignment);

_When_(paged == true, _IRQL_requires_max_(APC_LEVEL))
_When_(paged == false, _IRQL_requires_max_(DISPATCH_LEVEL))
[[nodiscard]]
void* operator new(std::size_t size, std::size_t count, bool paged, std::align_val_t alignment);

_When_(paged == true, _IRQL_requires_max_(APC_LEVEL))
_When_(paged == false, _IRQL_requires_max_(DISPATCH_LEVEL))
[[nodiscard]]
void* operator new(std::size_t size, bool paged, std::align_val_t alignment, const std::nothrow_t&) noexcept;

_When_(paged == true, _IRQL_requires_max_(APC_LEVEL))
_When_(paged == false, _IRQL_requires_max_(DISPATCH_LEVEL))
[[nodiscard]]
void* operator new(std::size_t size, std::size_t count, bool paged, std::align_val_t alignment, const std::nothrow_t&) noexcept;

//[[nodiscard]]
//void* operator new[](std::size_t size, bool paged, std::align_val_t alignment, const std::nothrow_t&) noexcept;
