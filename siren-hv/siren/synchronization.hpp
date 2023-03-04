#pragma once
#include <atomic>
#include <type_traits>
#include <algorithm>
#include "multiprocessor.hpp"

namespace siren {
    struct adopt_lock_t {};

    template<typename Ty>
    class lock_guard_t {
    private:
        Ty& m_locker;

    public:
        explicit lock_guard_t(Ty& locker) noexcept : m_locker{ locker } { m_locker.lock(); }

        lock_guard_t(Ty& locker, adopt_lock_t) noexcept : m_locker{ locker } {}

        // copy constructor is not allowed
        lock_guard_t(const lock_guard_t&) = delete;

        ~lock_guard_t() noexcept { m_locker.unlock(); }

        // copy assignment is not allowed
        lock_guard_t& operator=(const lock_guard_t&) = delete;
    };

    class spin_lock_t {
    private:
        std::atomic_int m_state;

    public:
        static constexpr int unlocked_state_v = 0;
        static constexpr int locked_state_v = 1;

        constexpr spin_lock_t() noexcept : m_state{ unlocked_state_v } {}

        [[nodiscard]]
        bool is_locked() const noexcept {
            return m_state == locked_state_v;
        }

        [[nodiscard]]
        bool is_unlocked() const noexcept {
            return m_state == unlocked_state_v;
        }

        [[nodiscard]]
        bool try_lock() noexcept {
            int expected = unlocked_state_v;
            return m_state == unlocked_state_v && m_state.compare_exchange_weak(expected, locked_state_v, std::memory_order_acquire);
        }

        void lock() noexcept;

        void unlock() noexcept {
            m_state.store(unlocked_state_v, std::memory_order_release);
        }
    };

    class once_flag_t {
    private:
        std::atomic_int m_state;

    public:
        static constexpr int not_executed_state_v = 0;
        static constexpr int executing_state_v = 1;
        static constexpr int already_executed_state_v = 2;

        constexpr once_flag_t() noexcept : m_state{ not_executed_state_v } {}

        template<typename CallableTy, typename... ArgsTy>
            requires std::conjunction_v<std::is_nothrow_invocable<CallableTy>, std::is_same<void, std::invoke_result_t<CallableTy, ArgsTy...>>>
        void call_once(CallableTy&& fn, ArgsTy&&... args) noexcept {
            int expected = not_executed_state_v;

            if (m_state == not_executed_state_v && m_state.compare_exchange_strong(expected, executing_state_v, std::memory_order_acquire)) [[unlikely]] {
                fn(std::forward<ArgsTy>(args)...);
                m_state.store(already_executed_state_v, std::memory_order_release);
            }

            constexpr unsigned max_wait = 65536;    // inspired by https://rayanfam.com/topics/hypervisor-from-scratch-part-8/#designing-a-spinlock

            for (unsigned wait = 0; m_state != already_executed_state_v;) [[unlikely]] {
                yield_cpu(wait);

                // Don't call "pause" too many times. If the wait becomes too big,
                // clamp it to the max_wait.
                wait = std::min<unsigned>(wait * 2, max_wait);
            }
        }
    };
}
