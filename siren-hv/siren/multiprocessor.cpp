#include "multiprocessor.hpp"
#include <wdm.h>

namespace siren {
    void yield_cpu() noexcept {
        YieldProcessor();
    }

    void yield_cpu(size_t cycles) noexcept {
        for (size_t i = 0; i < cycles; ++i) {
            YieldProcessor();
        }
    }

    uint32_t active_cpu_count() noexcept {
        return KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
    }

    uint32_t current_cpu_index() noexcept {
        return KeGetCurrentProcessorNumberEx(nullptr);
    }

    uintptr_t ipi_broadcast(cpu_callback_t fn, uintptr_t arg) noexcept {
        return KeIpiGenericCall(fn, arg);
    }

    uintptr_t run_at_cpu(uint32_t cpu_index, cpu_callback_t fn, uintptr_t arg) noexcept {
        uintptr_t retval;

        NTSTATUS nt_status;
        PROCESSOR_NUMBER processor_number;

        nt_status = KeGetProcessorNumberFromIndex(cpu_index, &processor_number);
        NT_VERIFY(NT_SUCCESS(nt_status));

        GROUP_AFFINITY group_affinity = {};
        GROUP_AFFINITY previous_group_affinity;

        group_affinity.Mask = KAFFINITY{ 1 } << processor_number.Number;
        group_affinity.Group = processor_number.Group;

        NT_VERIFY(KeGetCurrentIrql() <= APC_LEVEL);
        KeSetSystemGroupAffinityThread(&group_affinity, &previous_group_affinity);
        retval = fn(arg);
        KeRevertToUserGroupAffinityThread(&previous_group_affinity);

        return retval;
    }
}
