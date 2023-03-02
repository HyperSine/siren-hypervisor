#pragma once
#include <atomic>

namespace siren {
    class spin_lock_t {
    private:
        std::atomic_int m_state;

    public:
        static constexpr int unlocked_state_v = 0;
        static constexpr int locked_state_v = 1;

        spin_lock_t() noexcept : m_state{ unlocked_state_v } {}

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
}
