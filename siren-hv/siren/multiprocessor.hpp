#pragma once
#include <stddef.h>
#include <stdint.h>
#include <memory>       // use std::addressof
#include <type_traits>

namespace siren {
    void yield_cpu() noexcept;

    void yield_cpu(size_t cycles) noexcept;

    [[nodiscard]]
    uint32_t active_cpu_count() noexcept;

    [[nodiscard]]
    uint32_t current_cpu_index() noexcept;

    using cpu_callback_t = uintptr_t(*)(uintptr_t arg) noexcept;

    uintptr_t ipi_broadcast(cpu_callback_t fn, uintptr_t arg) noexcept;

    template<typename CallableTy>
        requires std::is_nothrow_invocable_r_v<uintptr_t, CallableTy>
    auto ipi_broadcast(CallableTy&& fn) noexcept {
        auto fn_address = reinterpret_cast<uintptr_t>(std::addressof(fn));
        return ipi_broadcast([](uintptr_t arg) noexcept -> uintptr_t { return (*reinterpret_cast<CallableTy*>(arg))(); }, fn_address);
    }

    template<typename CallableTy>
        requires std::conjunction_v<std::is_nothrow_invocable<CallableTy>, std::is_same<void, std::invoke_result_t<CallableTy>>>
    void ipi_broadcast(CallableTy&& fn) noexcept {
        auto fn_address = reinterpret_cast<uintptr_t>(std::addressof(fn));
        ipi_broadcast([](uintptr_t arg) noexcept { (*reinterpret_cast<CallableTy*>(arg))(); return uintptr_t{}; }, fn_address);
    }

    uintptr_t run_at_cpu(uint32_t cpu_index, cpu_callback_t fn, uintptr_t arg) noexcept;

    template<typename CallableTy>
        requires std::is_nothrow_invocable_r_v<uintptr_t, CallableTy>
    auto run_at_cpu(uint32_t cpu_index, CallableTy&& fn) noexcept {
        auto fn_address = reinterpret_cast<uintptr_t>(std::addressof(fn));
        return run_at_cpu(cpu_index, [](uintptr_t arg) noexcept -> uintptr_t { return (*reinterpret_cast<CallableTy*>(arg))(); }, fn_address);
    }

    template<typename CallableTy>
        requires std::conjunction_v<std::is_nothrow_invocable<CallableTy>, std::is_same<void, std::invoke_result_t<CallableTy>>>
    void run_at_cpu(uint32_t cpu_index, CallableTy&& fn) noexcept {
        auto fn_address = reinterpret_cast<uintptr_t>(std::addressof(fn));
        run_at_cpu(cpu_index, [](uintptr_t arg) noexcept { (*reinterpret_cast<CallableTy*>(arg))(); return uintptr_t{}; }, fn_address);
    }
}
