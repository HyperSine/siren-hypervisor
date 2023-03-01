#pragma once
#include "siren_global.hpp"
#include "siren_memory.hpp"

namespace siren {

    struct lookaside_pool {
        [[nodiscard]]
        static managed_object_ptr<lookaside_pool> create(bool paged, size_t size, std::align_val_t alignment) noexcept;

        template<typename _Ty>
        [[nodiscard]]
        static managed_object_ptr<lookaside_pool> create(bool paged, size_t size = sizeof(_Ty)) noexcept {
            return create(paged, size, std::align_val_t{ alignof(_Ty) });
        }

        virtual ~lookaside_pool() noexcept = default;

        [[nodiscard]]
        virtual void* allocate(bool from_preallocated_list_only = false) noexcept = 0;

        template<typename _Ty>
        [[nodiscard]]
        _Ty* allocate(bool from_preallocated_list_only = false) noexcept {
            return static_cast<_Ty*>(allocate(from_preallocated_list_only));
        }

        virtual void deallocate(void* ptr, bool to_preallocated_list = false) noexcept = 0;

        [[nodiscard]]
        virtual bool preallocate() noexcept = 0;

        [[nodiscard]]
        virtual size_t preallocated_count() const noexcept = 0;

        virtual void preallocated_shrink(size_t n = 0) noexcept = 0;
    };

}
