#pragma once
#include <atomic>
#include <type_traits>
#include "multiprocessor.hpp"

namespace siren {
    class spin_lock {
    private:
        std::atomic_int m_state;

    public:
        static constexpr int state_unlocked_v = 0;

        constexpr spin_lock() noexcept : m_state{ state_unlocked_v } {}

        [[nodiscard]]
        bool is_locked() const noexcept {
            return m_state.load(std::memory_order_relaxed) < state_unlocked_v;
        }

        [[nodiscard]]
        bool is_locked_shared() const noexcept {
            return m_state.load(std::memory_order_relaxed) > state_unlocked_v;
        }

        [[nodiscard]]
        bool is_unlocked() const noexcept {
            return m_state.load(std::memory_order_relaxed) == state_unlocked_v;
        }

        [[nodiscard]]
        bool try_lock() noexcept {
            int state = m_state.load(std::memory_order_relaxed);
            int expected = state_unlocked_v;
            return state == expected && m_state.compare_exchange_weak(expected, -1, std::memory_order_acquire);
        }

        [[nodiscard]]
        bool try_lock_shared() noexcept {
            int state = m_state.load(std::memory_order_relaxed);
            int next_state = state + 1;
            return state < next_state && state >= state_unlocked_v && m_state.compare_exchange_weak(state, next_state, std::memory_order_acquire);
        }

        void lock() noexcept {
            unsigned wait = 0;
            constexpr unsigned max_wait = 65536;    // inspired by https://rayanfam.com/topics/hypervisor-from-scratch-part-8/#designing-a-spinlock

            while (!try_lock()) {
                yield_cpu(wait);

                // Don't call `yield_cpu` too many times. If the `wait` becomes too big,
                // clamp it to the max_wait.
                wait = std::min<unsigned>(wait * 2, max_wait);
            }
        }

        void lock_shared() noexcept {
            unsigned wait = 0;
            constexpr unsigned max_wait = 65536;    // inspired by https://rayanfam.com/topics/hypervisor-from-scratch-part-8/#designing-a-spinlock

            while (!try_lock_shared()) {
                yield_cpu(wait);

                // Don't call `yield_cpu` too many times. If the `wait` becomes too big,
                // clamp it to the max_wait.
                wait = std::min<unsigned>(wait * 2, max_wait);
            }
        }

        void unlock() noexcept {
            m_state.store(state_unlocked_v, std::memory_order_release);
        }
        
        void unlock_shared() noexcept {
            m_state.fetch_sub(1, std::memory_order_release);
        }
    };

    class once_flag {
    private:
        std::atomic_int m_state;

    public:
        static constexpr int state_not_executed_v = 0;
        static constexpr int state_executing_v = 1;
        static constexpr int state_already_executed_v = 2;

        constexpr once_flag() noexcept : m_state{ state_not_executed_v } {}

        template<typename CallableTy, typename... ArgsTy>
            requires std::is_same_v<void, std::invoke_result_t<CallableTy, ArgsTy&&...>>
        void call_once(CallableTy&& fn, ArgsTy&&... args) noexcept(std::is_nothrow_invocable_v<CallableTy, ArgsTy&&...>) {
            int expected = state_not_executed_v;

            if (m_state.compare_exchange_strong(expected, state_executing_v, std::memory_order_acquire)) [[unlikely]] {
                fn(std::forward<ArgsTy>(args)...);
                m_state.store(state_already_executed_v, std::memory_order_release);
            }

            constexpr unsigned max_wait = 65536;    // inspired by https://rayanfam.com/topics/hypervisor-from-scratch-part-8/#designing-a-spinlock

            for (unsigned wait = 0; m_state.load(std::memory_order_relaxed) != state_already_executed_v;) [[likely]] {
                yield_cpu(wait);

                // Don't call "pause" too many times. If the wait becomes too big,
                // clamp it to the max_wait.
                wait = std::min<unsigned>(wait * 2, max_wait);
            }
        }
    };

    struct adopt_lock_t {};
    struct defer_lock_t {};
    struct try_to_lock_t {};

    template<typename LockerTy>
    class lock_guard {
    private:
        LockerTy& m_locker;
        bool m_owns;

    public:
        lock_guard(LockerTy& locker) noexcept 
            : m_locker{ locker }, m_owns{ false }
        {
            m_locker.lock();
            m_owns = true;
        }

        lock_guard(LockerTy& locker, adopt_lock_t) noexcept
            : m_locker{ locker }, m_owns{ true } {}

        lock_guard(LockerTy& locker, defer_lock_t) noexcept
            : m_locker{ locker }, m_owns{ false } {}

        lock_guard(LockerTy& locker, try_to_lock_t) noexcept
            : m_locker{ locker }, m_owns{ m_locker.try_lock() } {}

        lock_guard(const lock_guard&) = delete;

        lock_guard(lock_guard&&) noexcept = delete;

        lock_guard& operator=(const lock_guard&) = delete;

        lock_guard& operator=(lock_guard&&) noexcept = delete;

        ~lock_guard() noexcept {
            if (m_owns) {
                m_locker.unlock();
            }
        }

        void lock() noexcept {
            if (!m_owns) {
                m_locker.lock();
                m_owns = true;
            }
        }

        bool try_lock() noexcept {
            if (!m_owns) {
                m_owns = m_locker.try_lock();
            }
            return m_owns;
        }

        void unlock() noexcept {
            if (m_owns) {
                m_locker.unlock();
                m_owns = false;
            }
        }

        bool owns_lock() const noexcept {
            return m_owns;
        }
    };

    template<typename LockerTy>
    class shared_lock_guard {
    private:
        LockerTy& m_locker;
        bool m_owns;

    public:
        shared_lock_guard(LockerTy& locker) noexcept
            : m_locker{ locker }, m_owns{ false }
        {
            m_locker.lock();
            m_owns = true;
        }

        shared_lock_guard(LockerTy& locker, adopt_lock_t) noexcept
            : m_locker{ locker }, m_owns{ true } {}

        shared_lock_guard(LockerTy& locker, defer_lock_t) noexcept
            : m_locker{ locker }, m_owns{ false } {}

        shared_lock_guard(LockerTy& locker, try_to_lock_t) noexcept
            : m_locker{ locker }, m_owns{ m_locker.try_lock_shared() } {}

        shared_lock_guard(const shared_lock_guard&) = delete;

        shared_lock_guard(shared_lock_guard&&) noexcept = delete;

        shared_lock_guard& operator=(const shared_lock_guard&) = delete;

        shared_lock_guard& operator=(shared_lock_guard&&) noexcept = delete;

        ~shared_lock_guard() noexcept {
            if (m_owns) {
                m_locker.unlock_shared();
            }
        }

        void lock() noexcept {
            if (!m_owns) {
                m_locker.lock_shared();
                m_owns = true;
            }
        }

        bool try_lock() noexcept {
            if (!m_owns) {
                m_owns = m_locker.try_lock_shared();
            }
            return m_owns;
        }

        void unlock() noexcept {
            if (m_owns) {
                m_locker.unlock_shared();
                m_owns = false;
            }
        }

        bool owns_lock() const noexcept {
            return m_owns;
        }
    };
}
