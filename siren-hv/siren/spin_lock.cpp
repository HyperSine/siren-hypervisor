#include "spin_lock.hpp"
#include <wdm.h>
#include <algorithm>

namespace siren {
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
}
