#include "spin_lock.hpp"
#include <wdm.h>

namespace siren {

    spin_lock_t::spin_lock_t() noexcept : m_state(unlocked_state_v) {}

    [[nodiscard]]
    bool spin_lock_t::is_locked() const noexcept {
        return m_state == locked_state_v;
    }

    [[nodiscard]]
    bool spin_lock_t::is_unlocked() const noexcept {
        return m_state == unlocked_state_v;
    }

    [[nodiscard]]
    bool spin_lock_t::try_lock() noexcept {
        int unlocked_state = unlocked_state_v;
        return m_state == unlocked_state_v && m_state.compare_exchange_weak(unlocked_state, locked_state_v, std::memory_order_acquire);
    }

    void spin_lock_t::lock() noexcept {
        unsigned wait = 0;
        constexpr unsigned max_wait = 65536;    // inspired by https://rayanfam.com/topics/hypervisor-from-scratch-part-8/#designing-a-spinlock
        
        while (!try_lock()) {
            for (unsigned i = 0; i < wait; ++i) {
                YieldProcessor();
            }

            // Don't call "pause" too many times. If the wait becomes too big,
            // clamp it to the max_wait.
            wait = std::min<unsigned>(wait * 2, max_wait);
        }
    }

    void spin_lock_t::unlock() noexcept {
        m_state.store(unlocked_state_v, std::memory_order_release);
    }

}
