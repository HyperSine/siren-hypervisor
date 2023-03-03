#include "new.hpp"
#include "utility.hpp"
#include <wdm.h>

static_assert(__STDCPP_DEFAULT_NEW_ALIGNMENT__ == MEMORY_ALLOCATION_ALIGNMENT);

[[nodiscard]]
void* operator new(std::size_t size, bool paged, std::align_val_t alignment, const std::nothrow_t&) noexcept {
    std::size_t reserved_size;

    //
    // If NumberOfBytes is PAGE_SIZE or greater, a page-aligned buffer is allocated.
    // Memory allocations of PAGE_SIZE or less do not cross page boundaries.
    // Memory allocations of less than PAGE_SIZE are not necessarily page-aligned 
    //     but are aligned to 8-byte boundaries in 32-bit systems and to 16-byte boundaries in 64-bit systems.
    //

    if (to_underlying(alignment) <= MEMORY_ALLOCATION_ALIGNMENT) {
        reserved_size = 0;
    } else if (to_underlying(alignment) == PAGE_SIZE) {
        size = std::max<std::size_t>(size, PAGE_SIZE);     // over allocate if `size` less than page size.
        reserved_size = 0;
    } else {
        reserved_size = to_underlying(alignment) - 1 + sizeof(void*);
    }

    void* p;

    // NTDDI_WIN10_VB, where VB stands for Vibranium which is the codename of `Windows 10, version 2004`
    // Source: https://en.wikipedia.org/wiki/List_of_Microsoft_codenames
#if NTDDI_VERSION >= NTDDI_WIN10_VB
    POOL_FLAGS pool_flags = paged ? POOL_FLAG_PAGED : POOL_FLAG_NON_PAGED;

    // use ExAllocatePool2 for ExAllocatePoolWithTag has been deprecated since `Windows 10, version 2004`.
    //
    // Memory that ExAllocatePool2 allocates is zero initialized.
    // Kernel-mode drivers should not opt-out of zeroing for allocations 
    //     that will be copied to an untrusted location (user-mode, over the network, etc.) to avoid disclosing sensitive information.
    p = ExAllocatePool2(pool_flags, reserved_size + size, siren::pool_tag_v);
#else
    POOL_TYPE pool_type = paged ? PagedPool : NonPagedPoolNx;

    // Memory that ExAllocatePoolWithTag allocates is uninitialized. 
    // A kernel-mode driver must first zero this memory if it is going to make it visible to user-mode software (to avoid leaking potentially privileged contents).
    p = ExAllocatePoolWithTag(pool_type, reserved_size + size, siren::pool_tag_v);
    if (p) {
        RtlZeroMemory(p, reserved_size + size);
    }
#endif

    if (p && reserved_size) {
        auto pp = reinterpret_cast<void**>((reinterpret_cast<uintptr_t>(p) + reserved_size) & ~(to_underlying(alignment) - 1));
        pp[-1] = p;
        p = pp;
    }

    // assert aligned
    NT_ASSERT((reinterpret_cast<uintptr_t>(p) & (to_underlying(alignment) - 1)) == 0);

    return p;
}

//[[nodiscard]]
//void* operator new[](std::size_t size, bool paged, std::align_val_t alignment, const std::nothrow_t& nothrow_v) noexcept {
//    return operator new(size, paged, alignment, nothrow_v);
//}

void operator delete(void* p) noexcept {
    if (p) {
        ExFreePoolWithTag(p, siren::pool_tag_v);
    }
}

void operator delete(void* p, std::size_t size) noexcept {
    UNREFERENCED_PARAMETER(size);
    operator delete(p);
}

void operator delete(void* p, std::align_val_t alignment) noexcept {
    if (p) {
        if (to_underlying(alignment) <= MEMORY_ALLOCATION_ALIGNMENT || to_underlying(alignment) == PAGE_SIZE) {
            ExFreePoolWithTag(p, siren::pool_tag_v);
        } else {
            ExFreePoolWithTag(reinterpret_cast<void**>(p)[-1], siren::pool_tag_v);
        }
    }
}

void operator delete(void* p, std::size_t size, std::align_val_t alignment) noexcept {
    UNREFERENCED_PARAMETER(size);
    operator delete(p, alignment);
}

//void operator delete[](void* p) noexcept {
//    operator delete(p);
//}
//
//void operator delete[](void* p, std::size_t size) noexcept {
//    operator delete(p, size);
//}
//
//void operator delete[](void* p, std::align_val_t alignment) noexcept {
//    operator delete(p, alignment);
//}
//
//void operator delete[](void* p, std::size_t size, std::align_val_t alignment) noexcept {
//    operator delete(p, size, alignment);
//}
