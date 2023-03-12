#pragma once
#include <atomic>
#include <mutex>
#include <type_traits>
#include "multiprocessor.hpp"

namespace siren {
    class spin_lock {
    private:
        std::atomic_int m_state;

    public:
        static constexpr int state_unlocked_v = 0;
        static constexpr int state_locked_v = 1;

        constexpr spin_lock() noexcept : m_state{ state_unlocked_v } {}

        [[nodiscard]]
        bool is_locked() const noexcept {
            return m_state == state_locked_v;
        }

        [[nodiscard]]
        bool is_unlocked() const noexcept {
            return m_state == state_unlocked_v;
        }

        [[nodiscard]]
        bool try_lock() noexcept {
            int expected = state_unlocked_v;
            return m_state == state_unlocked_v && m_state.compare_exchange_weak(expected, state_locked_v, std::memory_order_acquire);
        }

        void lock() noexcept {
            unsigned wait = 0;
            constexpr unsigned max_wait = 65536;    // inspired by https://rayanfam.com/topics/hypervisor-from-scratch-part-8/#designing-a-spinlock

            while (!try_lock()) {
                for (unsigned i = 0; i < wait; ++i) {
                    yield_cpu();
                }

                // Don't call "pause" too many times. If the wait becomes too big,
                // clamp it to the max_wait.
                wait = std::min<unsigned>(wait * 2, max_wait);
            }
        }

        void unlock() noexcept {
            m_state.store(state_unlocked_v, std::memory_order_release);
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

            if (m_state == state_not_executed_v && m_state.compare_exchange_strong(expected, state_executing_v, std::memory_order_acquire)) [[unlikely]] {
                fn(std::forward<ArgsTy>(args)...);
                m_state.store(state_already_executed_v, std::memory_order_release);
            }

            constexpr unsigned max_wait = 65536;    // inspired by https://rayanfam.com/topics/hypervisor-from-scratch-part-8/#designing-a-spinlock

            for (unsigned wait = 0; m_state != state_already_executed_v;) [[unlikely]] {
                yield_cpu(wait);

                // Don't call "pause" too many times. If the wait becomes too big,
                // clamp it to the max_wait.
                wait = std::min<unsigned>(wait * 2, max_wait);
            }
        }
    };
}
