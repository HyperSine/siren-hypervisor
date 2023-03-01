#pragma once
#include "siren_global.hpp"
#include "kernel_new.hpp"
#include <memory>

namespace siren {

    template<bool _Paged>
    struct default_pool {
        [[nodiscard]]
        void* allocate(size_t size, std::align_val_t alignment) noexcept {
            return operator new(size, _Paged, alignment, std::nothrow);
        }

        template<typename _Ty>
            requires(!std::is_array_v<_Ty> || std::extent_v<_Ty> != 0)
        [[nodiscard]]
        auto allocate(size_t size = sizeof(_Ty)) noexcept {
            return static_cast<std::add_pointer_t<_Ty>>(operator new(size, _Paged, std::align_val_t{ alignof(_Ty) }, std::nothrow));
        }

        template<typename _Ty>
            requires(std::is_array_v<_Ty> && std::extent_v<_Ty> == 0)
        [[nodiscard]]
        auto allocate(size_t n) noexcept {
            using _ElemTy = std::remove_extent_t<_Ty>;
            return static_cast<std::add_pointer_t<_ElemTy>>(operator new(sizeof(_ElemTy) * n, _Paged, std::align_val_t{ alignof(_ElemTy) }, std::nothrow));
        }

        template<typename _Ty, typename... _ArgTys>
        [[nodiscard]]
        auto construct(_ArgTys&&... args) noexcept {
            return new(_Paged, std::align_val_t{ alignof(_Ty) }, std::nothrow) _Ty(std::forward<_ArgTys>(args)...);
        }

        template<typename _Ty>
            requires(std::is_array_v<_Ty> && std::extent_v<_Ty> == 0)
        [[nodiscard]]
        auto construct(size_t n) noexcept {
            using _ElemTy = std::remove_extent_t<_Ty>;
            return new(_Paged, std::align_val_t{ alignof(_ElemTy) }, std::nothrow) _ElemTy[n]();
        }

        template<typename _Ty>
            requires(!std::is_array_v<_Ty> || std::extent_v<_Ty> == 0)
        void destroy(std::remove_extent_t<_Ty>* p) noexcept {
            if constexpr (!std::is_array_v<_Ty>) {
                delete p;
            } else {
                delete[] p;
            }
        }

        void deallocate(void* ptr, std::align_val_t alignment) noexcept {
            return operator delete(ptr, alignment);
        }

        template<typename _Ty>
        void deallocate(_Ty* ptr) noexcept {
            return operator delete(ptr, std::align_val_t{ alignof(_Ty) });
        }
    };

    using default_paged_pool = default_pool<true>;
    using default_npaged_pool = default_pool<false>;

    struct lookaside_pool {
        static lookaside_pool* new_pool(bool paged, size_t size, std::align_val_t alignment) noexcept;

        template<typename _Ty>
        [[nodiscard]]
        static lookaside_pool* new_pool_for(bool paged, size_t size = sizeof(_Ty)) noexcept {
            return new_pool(paged, size, std::align_val_t{ alignof(_Ty) });
        }

        static void delete_pool(lookaside_pool* pool) noexcept;

        virtual ~lookaside_pool() noexcept = default;

        [[nodiscard]]
        virtual void* allocate(bool use_preallocated_only = false) noexcept = 0;

        template<typename _Ty>
        [[nodiscard]]
        std::add_pointer_t<_Ty> allocate(bool use_preallocated_only = false) noexcept {
            return static_cast<std::add_pointer_t<_Ty>>(allocate(use_preallocated_only));
        }

        virtual void deallocate(void* ptr, bool back_to_preallocated = false) noexcept = 0;

        [[nodiscard]]
        virtual bool preallocate() noexcept = 0;

        [[nodiscard]]
        virtual size_t preallocated_count() const noexcept = 0;

        virtual void preallocated_shrink(size_t n = 0) noexcept = 0;
    };

}
