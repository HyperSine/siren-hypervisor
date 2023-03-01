#include "lookaside_pool.hpp"

#include <mutex>
#include "spin_lock.hpp"

#include <wdm.h>

namespace siren {

    template<bool _Paged>
    struct Xpaged_lookaside_pool : public lookaside_pool {
    public:
        struct chunk_t {
            union {
                SINGLE_LIST_ENTRY linker;
                std::byte user_data[ANYSIZE_ARRAY];
            };
        };

        static_assert(sizeof(chunk_t) <= sizeof(void*));

    private:
        size_t m_size;
        size_t m_alignment;

        mutable spin_lock_t m_preallocated_lock;

        SINGLE_LIST_ENTRY m_preallocated_list_head;
        SIZE_T m_preallocated_list_length;

        std::conditional_t<_Paged, PAGED_LOOKASIDE_LIST, NPAGED_LOOKASIDE_LIST> m_allocator_impl;

        void preallocated_push(chunk_t* chunk) noexcept {
            auto lock_guard = std::lock_guard<spin_lock_t>{ m_preallocated_lock };

            PushEntryList(&m_preallocated_list_head, &chunk->linker);
            ++m_preallocated_list_length;
        }

        [[nodiscard]]
        chunk_t* preallocated_pop() noexcept {
            auto lock_guard = std::lock_guard<spin_lock_t>{ m_preallocated_lock };

            if (auto* chunk_linker = PopEntryList(&m_preallocated_list_head)) {
                --m_preallocated_list_length;
                return CONTAINING_RECORD(chunk_linker, chunk_t, linker);
            } else {
                return nullptr;
            }
        }

    public:
        Xpaged_lookaside_pool(size_t size, std::align_val_t alignment) noexcept :
            m_size(size),
            m_alignment(std::to_underlying(alignment)),
            m_preallocated_list_head{},
            m_preallocated_list_length(0),
            m_allocator_impl{}
        {
            size_t pad_size;
            size_t usable_size;

            if (m_alignment <= MEMORY_ALLOCATION_ALIGNMENT) {
                pad_size = 0;
                usable_size = std::max(m_size, sizeof(chunk_t));
            } else if (m_alignment == PAGE_SIZE) {
                pad_size = 0;
                usable_size = std::max(size, std::size_t{ PAGE_SIZE });
            } else {
                pad_size = std::to_underlying(alignment) - 1 + sizeof(void*);
                usable_size = std::max(m_size, sizeof(chunk_t));
            }

            if constexpr (_Paged) {
                ExInitializePagedLookasideList(&m_allocator_impl, nullptr, nullptr, 0, pad_size + usable_size, pool_tag_v, 0);
            } else {
                ExInitializeNPagedLookasideList(&m_allocator_impl, nullptr, nullptr, POOL_NX_ALLOCATION, pad_size + usable_size, pool_tag_v, 0);
            }
        }

        virtual ~Xpaged_lookaside_pool() noexcept override {
            preallocated_shrink(0);
            if constexpr (_Paged) {
                ExDeletePagedLookasideList(&m_allocator_impl);
            } else {
                ExDeleteNPagedLookasideList(&m_allocator_impl);
            }
        }

        [[nodiscard]]
        virtual void* allocate(bool use_preallocated_only) noexcept override {
            void* p = preallocated_pop();

            if (p == nullptr && !use_preallocated_only) {
                size_t pad_size;

                if (m_alignment <= MEMORY_ALLOCATION_ALIGNMENT || m_alignment == PAGE_SIZE) {
                    pad_size = 0;
                } else {
                    pad_size = m_alignment - 1 + sizeof(void*);
                }

                if constexpr (_Paged) {
                    p = ExAllocateFromPagedLookasideList(&m_allocator_impl);
                } else {
                    p = ExAllocateFromNPagedLookasideList(&m_allocator_impl);
                }

                if (p && pad_size) {
                    void** pp = reinterpret_cast<void**>((reinterpret_cast<uintptr_t>(p) + pad_size) & ~(m_alignment - 1));
                    pp[-1] = p;
                    p = pp;
                }

                // assert aligned
                NT_ASSERT((reinterpret_cast<uintptr_t>(p) & (m_alignment - 1)) == 0);
            }

            if (p) {
                RtlZeroMemory(p, m_size);
            }

            return p;
        }

        virtual void deallocate(void* p, bool to_preallocated_list) noexcept override {
            if (p) {
                if (to_preallocated_list) {
                    preallocated_push(static_cast<chunk_t*>(p));
                } else {
                    if (m_alignment <= MEMORY_ALLOCATION_ALIGNMENT || m_alignment == PAGE_SIZE) {
                        // no adjust to `p`
                    } else {
                        p = reinterpret_cast<void**>(p)[-1];
                    }

                    if constexpr (_Paged) {
                        ExFreeToPagedLookasideList(&m_allocator_impl, p);
                    } else {
                        ExFreeToNPagedLookasideList(&m_allocator_impl, p);
                    }
                }
            }
        }

        [[nodiscard]]
        virtual bool preallocate() noexcept override {
            void* p;
            size_t pad_size;

            if (m_alignment <= MEMORY_ALLOCATION_ALIGNMENT || m_alignment == PAGE_SIZE) {
                pad_size = 0;
            } else {
                pad_size = m_alignment - 1 + sizeof(void*);
            }

            if constexpr (_Paged) {
                p = ExAllocateFromPagedLookasideList(&m_allocator_impl);
            } else {
                p = ExAllocateFromNPagedLookasideList(&m_allocator_impl);
            }

            if (p) {
                if (pad_size) {
                    void** pp = reinterpret_cast<void**>((reinterpret_cast<uintptr_t>(p) + pad_size) & ~(m_alignment - 1));
                    pp[-1] = p;
                    p = pp;
                }

                // assert aligned
                NT_ASSERT((reinterpret_cast<uintptr_t>(p) & (m_alignment - 1)) == 0);

                preallocated_push(static_cast<chunk_t*>(p));
                return true;
            } else {
                return false;
            }
        }

        [[nodiscard]]
        virtual size_t preallocated_count() const noexcept override {
            std::lock_guard<spin_lock_t> lock_guard{ m_preallocated_lock };
            return m_preallocated_list_length;
        }

        virtual void preallocated_shrink(size_t n) noexcept override {
            std::lock_guard<spin_lock_t> lock_guard{ m_preallocated_lock };

            while (n < m_preallocated_list_length) {
                auto* chunk_linker = PopEntryList(&m_preallocated_list_head);
                --m_preallocated_list_length;

                auto* chunk = CONTAINING_RECORD(chunk_linker, chunk_t, linker);
                deallocate(chunk, false);
            }
        }
    };

    [[nodiscard]]
    managed_object_ptr<lookaside_pool> lookaside_pool::create(bool paged, size_t size, std::align_val_t alignment) noexcept {
        if (paged) {
            return make_managed_object<false, Xpaged_lookaside_pool<true>>(size, alignment).transfer();
        } else {
            return make_managed_object<false, Xpaged_lookaside_pool<false>>(size, alignment).transfer();
        }
    }

}
