#pragma once
#include "../siren_global.hpp"

namespace siren::x86 {

    [[nodiscard]]
    uint32_t active_cpu_count() noexcept;

    [[nodiscard]]
    uint32_t current_cpu_index() noexcept;

    using cpu_callback_t = uintptr_t(*)(uintptr_t arg) noexcept;

    uintptr_t ipi_broadcast(cpu_callback_t fn, uintptr_t arg) noexcept;

    template<typename _CallableTy>
        requires std::is_nothrow_invocable_r_v<uintptr_t, _CallableTy>
    auto ipi_broadcast(_CallableTy&& fn) noexcept {
        auto fn_address = reinterpret_cast<uintptr_t>(std::addressof(fn));
        return ipi_broadcast([](uintptr_t arg) noexcept -> uintptr_t { return (*reinterpret_cast<_CallableTy*>(arg))(); }, fn_address);
    }

    template<typename _CallableTy>
        requires(std::is_nothrow_invocable_v<_CallableTy> && std::is_same_v<void, std::invoke_result_t<_CallableTy>>)
    void ipi_broadcast(_CallableTy&& fn) noexcept {
        auto fn_address = reinterpret_cast<uintptr_t>(std::addressof(fn));
        ipi_broadcast([](uintptr_t arg) noexcept { (*reinterpret_cast<_CallableTy*>(arg))(); return uintptr_t{}; }, fn_address);
    }

    uintptr_t run_for_cpu(uint32_t cpu_index, cpu_callback_t fn, uintptr_t arg) noexcept;

    template<typename _CallableTy>
        requires(std::is_nothrow_invocable_r_v<uintptr_t, _CallableTy>)
    auto run_for_cpu(uint32_t cpu_index, _CallableTy&& fn) noexcept {
        auto fn_address = reinterpret_cast<uintptr_t>(std::addressof(fn));
        return run_for_cpu(cpu_index, [](uintptr_t arg) noexcept -> uintptr_t { return (*reinterpret_cast<_CallableTy*>(arg))(); }, fn_address);
    }

    template<typename _CallableTy>
        requires(std::is_nothrow_invocable_v<_CallableTy> && std::is_same_v<void, std::invoke_result_t<_CallableTy>>)
    void run_for_cpu(uint32_t cpu_index, _CallableTy&& fn) noexcept {
        auto fn_address = reinterpret_cast<uintptr_t>(std::addressof(fn));
        run_for_cpu(cpu_index, [](uintptr_t arg) noexcept { (*reinterpret_cast<_CallableTy*>(arg))(); return uintptr_t{}; }, fn_address);
    }

    template<typename _CallableTy, typename... _ArgTy>
        requires(std::is_nothrow_invocable_v<_CallableTy>&& std::is_same_v<void, std::invoke_result_t<_CallableTy, _ArgTy...>>)
    void cpu_call_once(std::atomic_int& once_flag, _CallableTy&& fn, _ArgTy&&... args) noexcept {
        int once_flag_not_executed = 0;
        int once_flag_executing = 1;
        int once_flag_already_executed = 2;

        if (once_flag == once_flag_not_executed && once_flag.compare_exchange_strong(once_flag_not_executed, once_flag_executing, std::memory_order_acquire)) [[unlikely]] {
            fn(std::forward<_ArgTy>(args)...);
            once_flag.store(once_flag_already_executed, std::memory_order_release);
        }

        constexpr unsigned max_wait = 65536;    // inspired by https://rayanfam.com/topics/hypervisor-from-scratch-part-8/#designing-a-spinlock

        for (unsigned wait = 0; once_flag != once_flag_already_executed;) [[unlikely]] {
            for (unsigned i = 0; i < wait; ++i) {
                _mm_pause();
            }

            // Don't call "pause" too many times. If the wait becomes too big,
            // clamp it to the max_wait.
            wait = std::min(wait * 2, max_wait);
        }
    }

}
