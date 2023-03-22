#pragma once
#include <new>
#include <memory>
#include <utility>
#include "concepts.hpp"
#include "expected.hpp"
#include "nt_status.hpp"
#include "irql_annotations.hpp"

namespace siren {
    constexpr uint32_t pool_tag_v = 'vhrs';

    template<typename Ty>
    struct paged_allocator;

    template<>
    struct paged_allocator<void> {
        _IRQL_requires_max_(APC_LEVEL)
        [[nodiscard]]
        static expected<void*, nt_status> allocate(std::size_t size, std::align_val_t alignment) noexcept;

        _IRQL_requires_max_(APC_LEVEL)
        [[nodiscard]]
        static expected<void*, nt_status> allocate(std::size_t size, std::size_t count, std::align_val_t alignment) noexcept;

        _IRQL_requires_max_(APC_LEVEL)
        static void deallocate(void* ptr, std::align_val_t alignment) noexcept;
    };

    template<satisfy_none<std::is_void, std::is_const, std::is_array> Ty>
    struct paged_allocator<Ty> {
        using value_type = Ty;

        using pointer = Ty*;
        using const_pointer = const Ty*;

        using reference = Ty&;
        using const_reference = const Ty&;

        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using propagate_on_container_move_assignment = std::true_type;
        using is_always_equal = std::true_type;

        paged_allocator() noexcept = default;

        paged_allocator(const paged_allocator&) noexcept = default;

        template <typename Ty2>
        paged_allocator(const paged_allocator<Ty2>&) noexcept {}

        _IRQL_requires_max_(APC_LEVEL)
        [[nodiscard]]
        expected<Ty*, nt_status> allocate(std::size_t n) {
            auto r = paged_allocator<void>::allocate(sizeof(Ty), n, std::align_val_t{ alignof(Ty) });
            if (r.has_value()) {
                return static_cast<Ty*>(r.value());
            } else {
                return unexpected{ r.error() };
            }
        }

        _IRQL_requires_max_(APC_LEVEL)
        void deallocate(Ty* p, std::size_t) {
            return paged_allocator<void>::deallocate(p, std::align_val_t{ alignof(Ty) });
        }
    };

    template<typename Ty>
    struct npaged_allocator;

    template<>
    struct npaged_allocator<void> {
        _IRQL_requires_max_(DISPATCH_LEVEL)
        [[nodiscard]]
        static expected<void*, nt_status> allocate(std::size_t size, std::align_val_t alignment) noexcept;

        _IRQL_requires_max_(DISPATCH_LEVEL)
        [[nodiscard]]
        static expected<void*, nt_status> allocate(std::size_t size, std::size_t count, std::align_val_t alignment) noexcept;

        _IRQL_requires_max_(DISPATCH_LEVEL)
        static void deallocate(void* ptr, std::align_val_t alignment) noexcept;
    };

    template<satisfy_none<std::is_void, std::is_const, std::is_array> Ty>
    struct npaged_allocator<Ty> {
        using value_type = Ty;

        using pointer = Ty*;
        using const_pointer = const Ty*;

        using reference = Ty&;
        using const_reference = const Ty&;

        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using propagate_on_container_move_assignment = std::true_type;
        using is_always_equal = std::true_type;

        npaged_allocator() noexcept = default;

        npaged_allocator(const npaged_allocator&) noexcept = default;

        template <typename Ty2>
        npaged_allocator(const npaged_allocator<Ty2>&) noexcept {}

        _IRQL_requires_max_(DISPATCH_LEVEL)
        [[nodiscard]]
        expected<Ty*, nt_status> allocate(std::size_t n) {
            auto r = npaged_allocator<void>::allocate(sizeof(Ty), n, std::align_val_t{ alignof(Ty) });
            if (r.has_value()) {
                return static_cast<Ty*>(r.value());
            } else {
                return unexpected{ r.error() };
            }
        }

        _IRQL_requires_max_(DISPATCH_LEVEL)
        void deallocate(Ty* ptr, std::size_t) {
            return npaged_allocator<void>::deallocate(ptr, std::align_val_t{ alignof(Ty) });
        }
    };

    template<typename Ty>
    struct contiguous_allocator;

    template<>
    struct contiguous_allocator<void> {
        _IRQL_requires_max_(DISPATCH_LEVEL)
        [[nodiscard]]
        static expected<void*, nt_status> allocate(std::size_t size, uint64_t highest_acceptable_physical_address);

        _IRQL_requires_max_(DISPATCH_LEVEL)
        [[nodiscard]]
        static expected<void*, nt_status> allocate(std::size_t size, std::size_t count, uint64_t highest_acceptable_physical_address);

        _IRQL_requires_max_(DISPATCH_LEVEL)
        static void deallocate(void* p) noexcept;
    };

    template<satisfy_none<std::is_void, std::is_const, std::is_array> Ty>
        requires aligned_most<Ty, 0x1000>
    struct contiguous_allocator<Ty> {
        using value_type = Ty;

        using pointer = Ty*;
        using const_pointer = const Ty*;

        using reference = Ty&;
        using const_reference = const Ty&;

        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using propagate_on_container_move_assignment = std::true_type;
        using is_always_equal = std::true_type;

        contiguous_allocator() noexcept = default;

        contiguous_allocator(const contiguous_allocator&) noexcept = default;

        template <typename Ty2>
        contiguous_allocator(const contiguous_allocator<Ty2>&) noexcept {}

        _IRQL_requires_max_(DISPATCH_LEVEL)
        [[nodiscard]]
        expected<Ty*, nt_status> allocate(std::size_t n) {
            auto r = contiguous_allocator<void>::allocate(sizeof(Ty), n, std::numeric_limits<uint64_t>::max());
            if (r.has_value()) {
                return static_cast<Ty*>(r.value());
            } else {
                return unexpected{ r.error() };
            }
        }

        _IRQL_requires_max_(DISPATCH_LEVEL)
        void deallocate(Ty* p, std::size_t) {
            contiguous_allocator<void>::deallocate(p);
        }
    };

    inline paged_allocator<std::byte> paged_pool;
    inline npaged_allocator<std::byte> npaged_pool;
    inline contiguous_allocator<std::byte> contiguous_pool;

    template<non_dimensional Ty, same_after<std::remove_cvref> AllocatorTy, typename... ArgTys>
        requires std::is_constructible_v<Ty, ArgTys&&...>
    [[nodiscard]]
    expected<Ty*, nt_status> allocator_new(AllocatorTy& allocator, ArgTys&&... args) noexcept {
        if constexpr (std::is_same_v<Ty, typename std::allocator_traits<AllocatorTy>::value_type>) {
            expected<Ty*, nt_status> r = allocator.allocate(1);
            if (r.has_value()) {
                std::construct_at(r.value(), std::forward<ArgTys>(args)...);
            }
            return r;
        } else {
            using RebindAllocatorTy = typename std::allocator_traits<AllocatorTy>::template rebind_alloc<Ty>;
            RebindAllocatorTy rebind_allocator{ allocator };
            expected<Ty*, nt_status> r = rebind_allocator.allocate(1);
            if (r.has_value()) {
                std::construct_at(r.value(), std::forward<ArgTys>(args)...);
            }
            return r;
        }
    }

    template<one_dimensional Ty, same_after<std::remove_cvref> AllocatorTy>
        requires std::is_unbounded_array_v<Ty> && std::is_default_constructible_v<std::remove_extent_t<Ty>>
    [[nodiscard]]
    expected<std::remove_extent_t<Ty>*, nt_status> allocator_new(AllocatorTy& allocator, std::size_t count) noexcept {
        using ElementTy = std::remove_extent_t<Ty>;
        if constexpr (std::is_same_v<ElementTy, typename std::allocator_traits<AllocatorTy>::value_type>) {
            expected<ElementTy*, nt_status> r = allocator.allocate(count);
            if (r.has_value()) {
                for (size_t i = 0; i < count; ++i) {
                    std::construct_at(std::addressof(r.value()[i]));
                }
            }
            return r;
        } else {
            using RebindAllocatorTy = typename std::allocator_traits<AllocatorTy>::template rebind_alloc<ElementTy>;
            RebindAllocatorTy rebind_allocator{ allocator };
            expected<ElementTy*, nt_status> r = rebind_allocator.allocate(count);
            if (r.has_value()) {
                for (size_t i = 0; i < count; ++i) {
                    std::construct_at(std::addressof(r.value()[i]));
                }
            }
            return r;
        }
    }

    template<non_dimensional Ty, same_after<std::remove_cvref> AllocatorTy>
    [[nodiscard]]
    expected<Ty*, nt_status> allocator_new_uninitialized(AllocatorTy& allocator) noexcept {
        if constexpr (std::is_same_v<Ty, typename std::allocator_traits<AllocatorTy>::value_type>) {
            return allocator.allocate(1);
        } else {
            using RebindAllocatorTy = typename std::allocator_traits<AllocatorTy>::template rebind_alloc<Ty>;
            RebindAllocatorTy rebind_allocator{ allocator };
            return rebind_allocator.allocate(1);
        }
    }

    template<one_dimensional Ty, same_after<std::remove_cvref> AllocatorTy>
        requires std::is_unbounded_array_v<Ty>
    [[nodiscard]]
    expected<std::remove_extent_t<Ty>*, nt_status> allocator_new_uninitialized(AllocatorTy& allocator, std::size_t count) noexcept {
        using ElementTy = std::remove_extent_t<Ty>;
        if constexpr (std::is_same_v<ElementTy, typename std::allocator_traits<AllocatorTy>::value_type>) {
            return allocator.allocate(count);
        } else {
            using RebindAllocatorTy = typename std::allocator_traits<AllocatorTy>::template rebind_alloc<ElementTy>;
            RebindAllocatorTy rebind_allocator{ allocator };
            return rebind_allocator.allocate(count);
        }
    }

    template<non_dimensional Ty, same_after<std::remove_cvref> AllocatorTy>
    void allocator_delete(AllocatorTy& allocator, Ty* ptr) noexcept {
        if constexpr (std::is_same_v<Ty, typename std::allocator_traits<AllocatorTy>::value_type>) {
            std::destroy_at(ptr);
            allocator.deallocate(ptr, 1);
        } else {
            using RebindAllocatorTy = typename std::allocator_traits<AllocatorTy>::template rebind_alloc<Ty>;
            RebindAllocatorTy rebind_allocator{ allocator };
            std::destroy_at(ptr);
            rebind_allocator.deallocate(ptr, 1);
        }
    }

    template<one_dimensional Ty, same_after<std::remove_cvref> AllocatorTy>
        requires std::is_unbounded_array_v<Ty>
    void allocator_delete(AllocatorTy& allocator, std::remove_extent_t<Ty>* ptr, std::size_t count) noexcept {
        using ElementTy = std::remove_extent_t<Ty>;
        if constexpr (std::is_same_v<ElementTy, typename std::allocator_traits<AllocatorTy>::value_type>) {
            for (size_t i = count; 0 < i; --i) {
                std::destroy_at(std::addressof(ptr[i - 1]));
            }
            allocator.deallocate(ptr, count);
        } else {
            using RebindAllocatorTy = typename std::allocator_traits<AllocatorTy>::template rebind_alloc<ElementTy>;
            RebindAllocatorTy rebind_allocator{ allocator };
            for (size_t i = count; 0 < i; --i) {
                std::destroy_at(std::addressof(ptr[i - 1]));
            }
            rebind_allocator.deallocate(ptr, count);
        }
    }

    template<typename Ty, typename AllocatorTy>
    struct allocation_delete;

    template<non_dimensional Ty, same_after<std::remove_cvref> AllocatorTy>
    struct allocation_delete<Ty, AllocatorTy> : private AllocatorTy {
        allocation_delete() noexcept = default;

        template<typename OtherAllocatorTy>
            requires std::is_constructible_v<AllocatorTy, OtherAllocatorTy&&>
        allocation_delete(OtherAllocatorTy&& allocator) noexcept
            : AllocatorTy{ std::forward<OtherAllocatorTy>(allocator) } {}

        void operator()(Ty* ptr) noexcept {
            allocator_delete<Ty>(static_cast<AllocatorTy&>(*this), ptr);
        }
    };

    template<one_dimensional Ty, same_after<std::remove_cvref> AllocatorTy>
        requires std::is_unbounded_array_v<Ty>
    struct allocation_delete<Ty, AllocatorTy> : private AllocatorTy {
        std::size_t count;

        allocation_delete() noexcept
            : count{} {}

        template<typename OtherAllocatorTy>
            requires std::is_constructible_v<AllocatorTy, OtherAllocatorTy&&>
        allocation_delete(OtherAllocatorTy&& allocator, std::size_t n) noexcept
            : AllocatorTy{ std::forward<OtherAllocatorTy>(allocator) }, count{ n } {}

        void operator()(std::remove_extent_t<Ty>* ptr) noexcept {
            allocator_delete<Ty>(static_cast<AllocatorTy&>(*this), ptr, count);
        }
    };

    template <non_dimensional Ty, same_after<std::remove_cvref> AllocatorTy, typename... ArgTys>
    [[nodiscard]]
    auto allocate_unique(AllocatorTy& allocator, ArgTys&&... args) noexcept {
        using RebindAllocatorTy = typename std::allocator_traits<AllocatorTy>::template rebind_alloc<Ty>;
        using DeleterTy = allocation_delete<Ty, RebindAllocatorTy>;
        using ReturnTy = expected<std::unique_ptr<Ty, DeleterTy>, nt_status>;

        expected<Ty*, nt_status> r = allocator_new<Ty>(allocator, std::forward<ArgTys>(args)...);
        if (r.has_value()) {
            return ReturnTy{ std::unique_ptr<Ty, DeleterTy>{ r.value(), DeleterTy{allocator} } };
        } else {
            return ReturnTy{ unexpected{ r.error() } };
        }
    }

    template <one_dimensional Ty, same_after<std::remove_cvref> AllocatorTy>
        requires std::is_unbounded_array_v<Ty>
    [[nodiscard]]
    auto allocate_unique(AllocatorTy& allocator, std::size_t count) noexcept {
        using ElementTy = std::remove_extent_t<Ty>;
        using RebindAllocatorTy = typename std::allocator_traits<AllocatorTy>::template rebind_alloc<ElementTy>;
        using DeleterTy = allocation_delete<Ty, RebindAllocatorTy>;
        using ReturnTy = expected<std::unique_ptr<Ty, DeleterTy>, nt_status>;

        expected<ElementTy*, nt_status> r = allocator_new<Ty>(allocator, count);
        if (r.has_value()) {
            return ReturnTy{ std::unique_ptr<Ty, DeleterTy>{ r.value(), DeleterTy{ allocator, count } } };
        } else {
            return ReturnTy{ unexpected{ r.error() }};
        }
    }

    template <non_dimensional Ty, same_after<std::remove_cvref> AllocatorTy>
    [[nodiscard]]
    auto allocate_unique_uninitialized(AllocatorTy& allocator) noexcept {
        using DeleterTy = allocation_delete<Ty, typename std::allocator_traits<AllocatorTy>::template rebind_alloc<Ty>>;
        using ReturnTy = expected<std::unique_ptr<Ty, DeleterTy>, nt_status>;

        expected<Ty*, nt_status> r = allocator_new_uninitialized<Ty>(allocator);
        if (r.has_value()) {
            return ReturnTy{ std::in_place, r.value(), DeleterTy{ allocator } };
        } else {
            return ReturnTy{ unexpected{ r.error() } };
        }
    }

    template <one_dimensional Ty, same_after<std::remove_cvref> AllocatorTy>
        requires std::is_unbounded_array_v<Ty>
    [[nodiscard]]
    auto allocate_unique_uninitialized(AllocatorTy& allocator, std::size_t count) noexcept {
        using ElementTy = std::remove_extent_t<Ty>;
        using DeleterTy = allocation_delete<Ty, typename std::allocator_traits<AllocatorTy>::template rebind_alloc<ElementTy>>;
        using ReturnTy = expected<std::unique_ptr<Ty, DeleterTy>, nt_status>;

        expected<ElementTy*, nt_status> r = allocator_new_uninitialized<Ty>(allocator, count);
        if (r.has_value()) {
            return ReturnTy{ std::in_place, r.value(), DeleterTy{ allocator, count } };
        } else {
            return ReturnTy{ unexpected{ r.error() }};
        }
    }

    template<typename Ty>
    using unique_paged = std::unique_ptr<Ty, allocation_delete<Ty, paged_allocator<std::remove_extent_t<Ty>>>>;

    template<typename Ty>
    using unique_npaged = std::unique_ptr<Ty, allocation_delete<Ty, npaged_allocator<std::remove_extent_t<Ty>>>>;

    template<typename Ty>
    using unique_contiguous = std::unique_ptr<Ty, allocation_delete<Ty, contiguous_allocator<std::remove_extent_t<Ty>>>>;
}
