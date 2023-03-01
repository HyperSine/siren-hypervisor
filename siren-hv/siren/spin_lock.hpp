#pragma once
#include "siren_global.hpp"

namespace siren {
    class spin_lock_t {
    private:
        std::atomic_int m_state;

    public:
        static constexpr int locked_state_v = 1;
        static constexpr int unlocked_state_v = 0;

        spin_lock_t() noexcept;

        [[nodiscard]]
        bool is_locked() const noexcept;

        [[nodiscard]]
        bool is_unlocked() const noexcept;

        [[nodiscard]]
        bool try_lock() noexcept;

        void lock() noexcept;

        void unlock() noexcept;
    };
}
