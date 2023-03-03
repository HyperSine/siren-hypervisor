#pragma once
#include <algorithm>
#include <atomic>
#include <type_traits>

namespace siren {
    class once_flag_t {
    private:
        std::atomic_int m_state;

        static void yield() noexcept;

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
                for (unsigned i = 0; i < wait; ++i) {
                    yield();
                }

                // Don't call "pause" too many times. If the wait becomes too big,
                // clamp it to the max_wait.
                wait = std::min<unsigned>(wait * 2, max_wait);
            }
        }
    };
}
