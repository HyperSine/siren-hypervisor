#pragma once
#include "siren_global.hpp"
#include "siren_new.hpp"
#include <memory>

namespace siren {

    // new = allocate + construct
    // delete = destruct + deallocate

    template<typename _Ty>
    void sequence_destruct_at(_Ty* p) noexcept {
        return std::destroy_at(p);
    }

    template<typename _Ty>
    void reverse_destruct_at(_Ty* p) noexcept {
        if constexpr (std::is_array_v<_Ty>) {
            for (auto it = std::rbegin(*p); it != std::rend(*p); ++it) {
                reverse_destruct(std::addressof(*it));
            }
        } else {
            std::destroy_at(p);
        }
    }

    template<typename _Ty>
    struct generic_deallocate {
        void operator()(_Ty* p) const noexcept {
            operator delete(p, std::align_val_t{ alignof(_Ty) });
        }
    };

    template<typename _Ty>
    struct generic_deallocate<_Ty[]> {
        void operator()(_Ty* p, size_t l) const noexcept {
            operator delete(p, l * sizeof(_Ty), std::align_val_t{ alignof(_Ty) });
        }
    };

    template<typename _Ty>
    struct generic_delete {
        void operator()(_Ty* p) const noexcept {
            reverse_destruct_at(p);
            operator delete(p, std::align_val_t{ alignof(_Ty) });
        }
    };

    template<typename _Ty>
    struct generic_delete<_Ty[]> {
        void operator()(_Ty* p, size_t l) const noexcept {
            for (size_t i = 1; i <= l; ++i) {
                reverse_destruct_at(std::addressof(p[l - i]));
            }
            operator delete(p, l * sizeof(_Ty), std::align_val_t{ alignof(_Ty) });
        }
    };

    template<typename _Ty, typename _DestroyerTy>
    class managed_ptr {
    public:
        using element_t = _Ty;
        using pointer_t = std::add_pointer_t<_Ty>;
        using destroyer_t = _DestroyerTy;

    private:
        pointer_t m_ptr;

    public:
        managed_ptr() noexcept : m_ptr(nullptr) {}

        managed_ptr(std::nullptr_t) noexcept : m_ptr(nullptr) {}

        managed_ptr(pointer_t ptr) noexcept : m_ptr(ptr) {};

        // copy constructor is not allowed
        managed_ptr(const managed_ptr& other) = delete;

        // move constructor
        managed_ptr(managed_ptr&& other) noexcept : m_ptr(other.m_ptr) {
            other.m_ptr = nullptr;
        }

        // copy assignment operator is not allowed
        managed_ptr& operator=(const managed_ptr& other) = delete;

        // move assignment operator
        managed_ptr& operator=(managed_ptr&& other) noexcept {
            destroy();
            std::swap(m_ptr, other.m_ptr);
            return *this;
        }

        ~managed_ptr() noexcept {
            destroy();
        }

        [[nodiscard]]
        pointer_t operator->() const noexcept {
            return m_ptr;
        }

        [[nodiscard]]
        operator bool() const noexcept {
            return m_ptr != nullptr;
        }

        template<typename _CastTy>
        [[nodiscard]]
        _CastTy static_cast_to() const noexcept {
            return static_cast<_CastTy>(m_ptr);
        }

        template<typename _CastTy>
        [[nodiscard]]
        _CastTy reinterpret_cast_to() const noexcept {
            return reinterpret_cast<_CastTy>(m_ptr);
        }

        [[nodiscard]]
        pointer_t get() const noexcept {
            return m_ptr;
        }

        void set(pointer_t new_ptr) noexcept {
            destroy();
            m_ptr = new_ptr;
        }

        void swap(managed_ptr& other) noexcept {
            std::swap(m_ptr, other.m_ptr);
        }

        void discard() noexcept {
            if (m_ptr) {
                m_ptr = nullptr;
            }
        }

        [[nodiscard]]
        pointer_t transfer() noexcept {
            pointer_t ptr = m_ptr;
            m_ptr = nullptr;
            return ptr;
        }

        void destroy() noexcept {
            if (m_ptr) {
                destroyer_t{}(m_ptr);
                m_ptr = nullptr;
            }
        }
    };

    template<typename _Ty, typename _DestroyerTy>
    class managed_ptr<_Ty[], _DestroyerTy> {
    public:
        using element_t = _Ty;
        using pointer_t = std::add_pointer_t<_Ty>;
        using destroyer_t = _DestroyerTy;

    private:
        size_t m_len;
        pointer_t m_ptr;
        
    public:
        managed_ptr() noexcept : m_len(0), m_ptr(nullptr) {}

        managed_ptr(std::nullptr_t) noexcept : m_len(0), m_ptr(nullptr) {}

        managed_ptr(pointer_t ptr, size_t len) noexcept : m_len(len), m_ptr(ptr) {};

        // move constructor
        managed_ptr(managed_ptr&& other) noexcept : m_len(other.m_len), m_ptr(other.m_ptr) {
            other.m_len = 0;
            other.m_ptr = nullptr;
        }

        // copy constructor is not allowed
        managed_ptr(const managed_ptr&) = delete;

        // move assignment
        managed_ptr& operator=(managed_ptr&& other) noexcept {
            destroy();
            std::swap(m_len, other.m_len);
            std::swap(m_ptr, other.m_ptr);
            return *this;
        }

        // move assignment is not allowed
        managed_ptr& operator=(const managed_ptr& other) = delete;

        ~managed_ptr() noexcept {
            destroy();
        }

        [[nodiscard]]
        operator bool() const noexcept {
            return m_ptr != nullptr;
        }

        [[nodiscard]]
        element_t& operator[](size_t i) const noexcept {
            return m_ptr[i];
        }

        [[nodiscard]]
        size_t get_length() const noexcept {
            return m_len;
        }

        [[nodiscard]]
        pointer_t get() const noexcept {
            return m_ptr;
        }

        void set(pointer_t new_ptr, size_t new_len) noexcept {
            destroy();
            m_len = new_len;
            m_ptr = new_ptr;
        }

        void swap(managed_ptr& other) noexcept {
            std::swap(m_len, other.m_len);
            std::swap(m_ptr, other.m_ptr);
        }

        void discard() noexcept {
            if (m_ptr) {
                m_len = 0;
                m_ptr = nullptr;
            }
        }

        [[nodiscard]]
        std::pair<pointer_t, size_t> transfer() noexcept {
            auto retval = std::make_pair(m_ptr, m_len);

            m_len = 0;
            m_ptr = nullptr;

            return retval;
        }

        void destroy() noexcept {
            if (m_ptr) {
                destroyer_t{}(m_ptr, m_len);
                m_ptr = nullptr;
            }
        }
    };

    template<typename _Ty>
    using managed_memory_ptr = managed_ptr<_Ty, generic_deallocate<_Ty>>;

    template<typename _Ty>
    using managed_object_ptr = managed_ptr<_Ty, generic_delete<_Ty>>;

    struct skip_construct_t {};

    template<bool _Paged, typename _Ty>
        requires(!std::is_array_v<_Ty>)
    [[nodiscard]]
    managed_memory_ptr<_Ty> make_managed_memory(size_t size = sizeof(_Ty)) noexcept {
        return managed_memory_ptr{ static_cast<_Ty*>(operator new(size, _Paged, std::align_val_t{ alignof(_Ty) }, std::nothrow)) };
    }

    template<bool _Paged, typename _Ty>
        requires(std::is_unbounded_array_v<_Ty>)
    [[nodiscard]]
    managed_memory_ptr<_Ty> make_managed_memory(size_t n) noexcept {
        using _ElemTy = std::remove_extent_t<_Ty>;
        return managed_memory_ptr<_Ty>{ static_cast<_ElemTy*>(operator new(n * sizeof(_ElemTy), _Paged, std::align_val_t{ alignof(_ElemTy) }, std::nothrow)), n };
    }

    template<bool _Paged, typename _Ty, typename... _ArgTys>
        requires(!std::is_array_v<_Ty>)
    [[nodiscard]]
    managed_object_ptr<_Ty> make_managed_object(_ArgTys&&... args) noexcept {
        void* storage = operator new(sizeof(_Ty), _Paged, std::align_val_t{ alignof(_Ty) }, std::nothrow);
        if (storage) {
            return managed_object_ptr<_Ty>{ std::construct_at(static_cast<_Ty*>(storage), std::forward<_ArgTys>(args)...) };
        } else {
            return managed_object_ptr<_Ty>{};
        }
    }

    template<bool _Paged, typename _Ty>
        requires(std::is_unbounded_array_v<_Ty>)
    [[nodiscard]]
    managed_object_ptr<_Ty> make_managed_object(size_t n) noexcept {
        using _ElemTy = std::remove_extent_t<_Ty>;
        void* storage = operator new(n * sizeof(_ElemTy), _Paged, std::align_val_t{ alignof(_ElemTy) }, std::nothrow);
        if (storage) {
            auto* p = std::launder(static_cast<_ElemTy*>(storage));

            for (size_t i = 0; i < n; ++i) {
                std::construct_at(std::addressof(p[i]));
            }

            return managed_ptr<_Ty>{ p, n };
        } else {
            return managed_ptr<_Ty>{};
        }
    }

    template<bool _Paged, typename _Ty>
        requires(std::is_unbounded_array_v<_Ty>)
    [[nodiscard]]
    managed_object_ptr<_Ty> make_managed_object(size_t n, skip_construct_t) noexcept {
        using _ElemTy = std::remove_extent_t<_Ty>;
        void* storage = operator new(n * sizeof(_ElemTy), _Paged, std::align_val_t{ alignof(_ElemTy) }, std::nothrow);
        if (storage) {
            auto* p = std::launder(static_cast<_ElemTy*>(storage));
            return managed_ptr<_Ty>{ p, n };
        } else {
            return managed_ptr<_Ty>{};
        }
    }

    template<typename _Ty>
    class reference_ptr {
    public:
        using element_t = _Ty;
        using pointer_t = std::add_pointer_t<_Ty>;

    private:
        pointer_t m_ptr;

    public:
        reference_ptr() noexcept : m_ptr(nullptr) {}

        reference_ptr(std::nullptr_t) noexcept : m_ptr(nullptr) {};

        reference_ptr(pointer_t ptr) noexcept : m_ptr(ptr) {};

        // copy constructor
        reference_ptr(const reference_ptr& other) noexcept = default;

        // move constructor
        reference_ptr(reference_ptr&& other) noexcept = default;

        // copy assignment operator
        reference_ptr& operator=(const reference_ptr& other) noexcept = default;

        // move assignment operator
        reference_ptr& operator=(reference_ptr&& other) noexcept = default;

        ~reference_ptr() noexcept = default;

        [[nodiscard]]
        pointer_t operator->() const noexcept {
            return m_ptr;
        }

        [[nodiscard]]
        operator bool() const noexcept {
            return m_ptr != nullptr;
        }

        template<typename _CastTy>
        [[nodiscard]]
        _CastTy static_cast_to() const noexcept {
            return static_cast<_CastTy>(m_ptr);
        }

        template<typename _CastTy>
        [[nodiscard]]
        _CastTy reinterpret_cast_to() const noexcept {
            return reinterpret_cast<_CastTy>(m_ptr);
        }

        [[nodiscard]]
        pointer_t get() const noexcept {
            return m_ptr;
        }
    };

}
