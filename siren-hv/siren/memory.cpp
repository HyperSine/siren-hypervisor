#include "memory.hpp"
#include <bit>
#include <wdm.h>
#include <ntintsafe.h>

namespace siren {
    template<bool Paged>
    struct Xpaged_allocator {
        _When_(Paged, _IRQL_requires_max_(APC_LEVEL))
        _When_(!Paged, _IRQL_requires_max_(DISPATCH_LEVEL))
        [[nodiscard]]
        static expected<void*, nt_status> allocate(std::size_t size, std::align_val_t alignment) noexcept {
            nt_status status{};

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
                status = nt_status::cast_from(RtlSizeTAdd(std::to_underlying(alignment) - 1, sizeof(void*), &reserved_size));  // `alignment - 1` never overflows
                if (!status.is_success()) {
                    return unexpected{ status };
                }
            }

            status = nt_status::cast_from(RtlSizeTAdd(reserved_size, size, &allocate_size));
            if (!status.is_success()) {
                return unexpected{ status };
            }

            void* p;
            // NTDDI_WIN10_VB, where VB stands for Vibranium which is the codename of `Windows 10, version 2004`
            // Source: https://en.wikipedia.org/wiki/List_of_Microsoft_codenames
#if NTDDI_VERSION >= NTDDI_WIN10_VB
            POOL_FLAGS pool_flags = Paged ? POOL_FLAG_PAGED : POOL_FLAG_NON_PAGED;
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

        _When_(Paged, _IRQL_requires_max_(APC_LEVEL))
        _When_(!Paged, _IRQL_requires_max_(DISPATCH_LEVEL))
        [[nodiscard]]
        static expected<void*, nt_status> allocate(std::size_t size, std::size_t count, std::align_val_t alignment) noexcept {
            std::size_t total_size;
            auto status = nt_status::cast_from(RtlSizeTMult(size, count, &total_size));
            if (status.is_success()) {
                return allocate(total_size, alignment);
            } else {
                return unexpected{ status };
            }
        }

        _IRQL_requires_max_(DISPATCH_LEVEL)
        static void deallocate(void* ptr, std::align_val_t alignment) noexcept {
            if (ptr) {
                if (std::to_underlying(alignment) <= MEMORY_ALLOCATION_ALIGNMENT || std::to_underlying(alignment) == PAGE_SIZE) {
                    ExFreePoolWithTag(ptr, siren::pool_tag_v);
                } else {
                    ExFreePoolWithTag(reinterpret_cast<void**>(ptr)[-1], siren::pool_tag_v);
                }
            }
        }
    };

    _IRQL_requires_max_(APC_LEVEL)
    expected<void*, nt_status> paged_allocator<void>::allocate(std::size_t size, std::align_val_t alignment) noexcept {
        return Xpaged_allocator<true>::allocate(size, alignment);
    }

    _IRQL_requires_max_(APC_LEVEL)
    expected<void*, nt_status> paged_allocator<void>::allocate(std::size_t size, std::size_t count, std::align_val_t alignment) noexcept {
        return Xpaged_allocator<true>::allocate(size, count, alignment);
    }

    _IRQL_requires_max_(APC_LEVEL)
    void paged_allocator<void>::deallocate(void* ptr, std::align_val_t alignment) noexcept {
        return Xpaged_allocator<true>::deallocate(ptr, alignment);
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
    expected<void*, nt_status> npaged_allocator<void>::allocate(std::size_t size, std::align_val_t alignment) noexcept {
        return Xpaged_allocator<false>::allocate(size, alignment);
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
    expected<void*, nt_status> npaged_allocator<void>::allocate(std::size_t size, std::size_t count, std::align_val_t alignment) noexcept {
        return Xpaged_allocator<false>::allocate(size, count, alignment);
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
    void npaged_allocator<void>::deallocate(void* ptr, std::align_val_t alignment) noexcept {
        return Xpaged_allocator<false>::deallocate(ptr, alignment);
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
    expected<void*, nt_status> contiguous_allocator<void>::allocate(std::size_t size, uint64_t highest_acceptable_physical_address) {
        void* p = MmAllocateContiguousMemory(size, PHYSICAL_ADDRESS{ .QuadPart = static_cast<LONGLONG>(highest_acceptable_physical_address) });
        if (p) {
            return expected<void*, nt_status>{ p };
        } else {
            return unexpected{ nt_status{ static_cast<uint32_t>(STATUS_INSUFFICIENT_RESOURCES) } };
        }
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
    expected<void*, nt_status> contiguous_allocator<void>::allocate(std::size_t size, std::size_t count, uint64_t highest_acceptable_physical_address) {
        std::size_t total_size;
        nt_status status = { static_cast<uint32_t>(RtlSizeTMult(size, count, &total_size)) };
        if (status.is_success()) {
            return allocate(total_size, highest_acceptable_physical_address);
        } else {
            return unexpected{ status };
        }
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
    void contiguous_allocator<void>::deallocate(void* p) noexcept {
        if (p) {
            MmFreeContiguousMemory(p);
        }
    }
}

void operator delete(void* p) noexcept {
    UNREFERENCED_PARAMETER(p);
    __fastfail(FAST_FAIL_UNEXPECTED_CALL);
}

void operator delete(void* p, std::size_t size) noexcept {
    UNREFERENCED_PARAMETER(p);
    UNREFERENCED_PARAMETER(size);
    FAST_FAIL_UNHANDLED_LSS_EXCEPTON;
    __fastfail(FAST_FAIL_UNEXPECTED_CALL);
}

void operator delete(void* p, std::align_val_t alignment) noexcept {
    UNREFERENCED_PARAMETER(p);
    UNREFERENCED_PARAMETER(alignment);
    __fastfail(FAST_FAIL_UNEXPECTED_CALL);
}

void operator delete(void* p, std::size_t size, std::align_val_t alignment) noexcept {
    UNREFERENCED_PARAMETER(p);
    UNREFERENCED_PARAMETER(size);
    UNREFERENCED_PARAMETER(alignment);
    __fastfail(FAST_FAIL_UNEXPECTED_CALL);
}
