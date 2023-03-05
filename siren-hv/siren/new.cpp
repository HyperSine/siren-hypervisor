#include "new.hpp"
#include "utility.hpp"
#include <wdm.h>
#include <ntintsafe.h>

static_assert(__STDCPP_DEFAULT_NEW_ALIGNMENT__ == MEMORY_ALLOCATION_ALIGNMENT);

_When_(paged == true, _IRQL_requires_max_(APC_LEVEL))
_When_(paged == false, _IRQL_requires_max_(DISPATCH_LEVEL))
void* operator new(std::size_t size, bool paged, std::align_val_t alignment) {
    NTSTATUS nt_status = STATUS_SUCCESS;

    if (std::to_underlying(alignment) == 0) {
        alignment = std::align_val_t{ 1 };
    }

    NT_ASSERT(std::has_single_bit(std::to_underlying(alignment)));

    //
    // If NumberOfBytes is PAGE_SIZE or greater, a page-aligned buffer is allocated.
    // Memory allocations of PAGE_SIZE or less do not cross page boundaries.
    // Memory allocations of less than PAGE_SIZE are not necessarily page-aligned 
    //     but are aligned to 8-byte boundaries in 32-bit systems and to 16-byte boundaries in 64-bit systems.
    //
    std::size_t reserved_size;
    std::size_t allocate_size;

    if (std::to_underlying(alignment) <= MEMORY_ALLOCATION_ALIGNMENT) {
        reserved_size = 0;
    } else if (std::to_underlying(alignment) == PAGE_SIZE) {
        size = std::max<std::size_t>(size, PAGE_SIZE);     // over allocate if `size` less than page size.
        reserved_size = 0;
    } else {
        nt_status = RtlSizeTAdd(std::to_underlying(alignment) - 1, sizeof(void*), &reserved_size);  // `alignment - 1` never overflows
        if (!NT_SUCCESS(nt_status)) {
            ExRaiseStatus(nt_status);
        }
    }

    nt_status = RtlSizeTAdd(reserved_size, size, &allocate_size);
    if (!NT_SUCCESS(nt_status)) {
        ExRaiseStatus(nt_status);
    }

    void* p;
    // NTDDI_WIN10_VB, where VB stands for Vibranium which is the codename of `Windows 10, version 2004`
    // Source: https://en.wikipedia.org/wiki/List_of_Microsoft_codenames
#if NTDDI_VERSION >= NTDDI_WIN10_VB
    POOL_FLAGS pool_flags = paged ? POOL_FLAG_PAGED : POOL_FLAG_NON_PAGED;
    pool_flags |= POOL_FLAG_UNINITIALIZED;
    pool_flags |= POOL_FLAG_RAISE_ON_FAILURE;

    // use ExAllocatePool2 for ExAllocatePoolWithTag has been deprecated since `Windows 10, version 2004`.
    //
    // Memory that ExAllocatePool2 allocates is zero initialized.
    // Kernel-mode drivers should not opt-out of zeroing for allocations 
    //     that will be copied to an untrusted location (user-mode, over the network, etc.) to avoid disclosing sensitive information.
    p = ExAllocatePool2(pool_flags, allocate_size, siren::pool_tag_v);
#else
    POOL_TYPE pool_type = paged ? PagedPool : NonPagedPoolNx;
    pool_type |= POOL_RAISE_IF_ALLOCATION_FAILURE;

    // Memory that ExAllocatePoolWithTag allocates is uninitialized. 
    // A kernel-mode driver must first zero this memory if it is going to make it visible to user-mode software (to avoid leaking potentially privileged contents).
    p = ExAllocatePoolWithTag(pool_type, allocate_size, siren::pool_tag_v);
#endif

    NT_ASSERT(p != nullptr);

    if (reserved_size) {
        auto pp = reinterpret_cast<void**>(siren::align_down(reinterpret_cast<uintptr_t>(p) + reserved_size, alignment));
        pp[-1] = p;
        p = pp;
    }

    // assert aligned
    NT_ASSERT(siren::align_check(p, alignment));

    return p;
}

_When_(paged == true, _IRQL_requires_max_(APC_LEVEL))
_When_(paged == false, _IRQL_requires_max_(DISPATCH_LEVEL))
[[nodiscard]]
void* operator new(std::size_t size, std::size_t count, bool paged, std::align_val_t alignment) {
    std::size_t total_size;
    NTSTATUS nt_status = RtlSizeTMult(size, count, &total_size);
    if (NT_SUCCESS(nt_status)) {
        return operator new(total_size, paged, alignment);
    } else {
        ExRaiseStatus(nt_status);
    }
}

_When_(paged == true, _IRQL_requires_max_(APC_LEVEL))
_When_(paged == false, _IRQL_requires_max_(DISPATCH_LEVEL))
void* operator new(std::size_t size, bool paged, std::align_val_t alignment, const std::nothrow_t&) noexcept {
    if (std::to_underlying(alignment) == 0) {
        alignment = std::align_val_t{ 1 };
    }

    NT_ASSERT(std::has_single_bit(std::to_underlying(alignment)));

    //
    // If NumberOfBytes is PAGE_SIZE or greater, a page-aligned buffer is allocated.
    // Memory allocations of PAGE_SIZE or less do not cross page boundaries.
    // Memory allocations of less than PAGE_SIZE are not necessarily page-aligned 
    //     but are aligned to 8-byte boundaries in 32-bit systems and to 16-byte boundaries in 64-bit systems.
    //
    std::size_t reserved_size;
    std::size_t allocate_size;

    if (std::to_underlying(alignment) <= MEMORY_ALLOCATION_ALIGNMENT) {
        reserved_size = 0;
    } else if (std::to_underlying(alignment) == PAGE_SIZE) {
        size = std::max<std::size_t>(size, PAGE_SIZE);     // over allocate if `size` less than page size.
        reserved_size = 0;
    } else {
        reserved_size = std::to_underlying(alignment) - 1 + sizeof(void*);
    }

    if (!NT_SUCCESS(RtlSizeTAdd(reserved_size, size, &allocate_size))) {
        return nullptr;
    }

    void* p;
    // NTDDI_WIN10_VB, where VB stands for Vibranium which is the codename of `Windows 10, version 2004`
    // Source: https://en.wikipedia.org/wiki/List_of_Microsoft_codenames
#if NTDDI_VERSION >= NTDDI_WIN10_VB
    POOL_FLAGS pool_flags = paged ? POOL_FLAG_PAGED : POOL_FLAG_NON_PAGED;
    pool_flags |= POOL_FLAG_UNINITIALIZED;

    // use ExAllocatePool2 for ExAllocatePoolWithTag has been deprecated since `Windows 10, version 2004`.
    //
    // Memory that ExAllocatePool2 allocates is zero initialized.
    // Kernel-mode drivers should not opt-out of zeroing for allocations 
    //     that will be copied to an untrusted location (user-mode, over the network, etc.) to avoid disclosing sensitive information.
    p = ExAllocatePool2(pool_flags, allocate_size, siren::pool_tag_v);
#else
    POOL_TYPE pool_type = paged ? PagedPool : NonPagedPoolNx;

    // Memory that ExAllocatePoolWithTag allocates is uninitialized. 
    // A kernel-mode driver must first zero this memory if it is going to make it visible to user-mode software (to avoid leaking potentially privileged contents).
    p = ExAllocatePoolWithTag(pool_type, allocate_size, siren::pool_tag_v);
#endif

    if (p && reserved_size) {
        auto pp = reinterpret_cast<void**>(siren::align_down(reinterpret_cast<uintptr_t>(p) + reserved_size, alignment));
        pp[-1] = p;
        p = pp;
    }

    // assert aligned
    NT_ASSERT(siren::align_check(p, alignment));

    return p;
}

_When_(paged == true, _IRQL_requires_max_(APC_LEVEL))
_When_(paged == false, _IRQL_requires_max_(DISPATCH_LEVEL))
void* operator new(std::size_t size, std::size_t count, bool paged, std::align_val_t alignment, const std::nothrow_t& nothrow) noexcept {
    std::size_t total_size;
    if (NT_SUCCESS(RtlSizeTMult(size, count, &total_size))) {
        return operator new(total_size, paged, alignment, nothrow);
    } else {
        return nullptr;
    }
}

//[[nodiscard]]
//void* operator new[](std::size_t size, bool paged, std::align_val_t alignment, const std::nothrow_t& nothrow_v) noexcept {
//    return operator new(size, paged, alignment, nothrow_v);
//}

_IRQL_requires_max_(DISPATCH_LEVEL)
void operator delete(void* p) noexcept {
    if (p) {
        ExFreePoolWithTag(p, siren::pool_tag_v);
    }
}

_IRQL_requires_max_(DISPATCH_LEVEL)
void operator delete(void* p, std::size_t size) noexcept {
    UNREFERENCED_PARAMETER(size);
    operator delete(p);
}

_IRQL_requires_max_(DISPATCH_LEVEL)
void operator delete(void* p, std::align_val_t alignment) noexcept {
    if (p) {
        if (std::to_underlying(alignment) <= MEMORY_ALLOCATION_ALIGNMENT || std::to_underlying(alignment) == PAGE_SIZE) {
            ExFreePoolWithTag(p, siren::pool_tag_v);
        } else {
            ExFreePoolWithTag(reinterpret_cast<void**>(p)[-1], siren::pool_tag_v);
        }
    }
}

_IRQL_requires_max_(DISPATCH_LEVEL)
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
