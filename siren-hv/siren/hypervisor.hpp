#pragma once
#include <stdint.h>

namespace siren {
    struct hypervisor;
    struct virtual_cpu;

    enum class implementation_e {
        X86_SVM_NATURAL,
        X86_VMX_NATURAL,
        X86_VMX_MICROSOFT_HV,
    };

    struct hypervisor {
        virtual ~hypervisor() noexcept = default;

        [[nodiscard]]
        virtual implementation_e get_implementation() const noexcept = 0;

        [[nodiscard]]
        virtual virtual_cpu* get_virtual_cpu(uint32_t cpu_index) noexcept = 0;

        [[nodiscard]]
        virtual const virtual_cpu* get_virtual_cpu(uint32_t cpu_index) const noexcept = 0;

        [[nodiscard]]
        virtual uint32_t get_virtual_cpu_count() const noexcept = 0;

        virtual void start() noexcept = 0;

        virtual void stop() noexcept = 0;
    };
}
