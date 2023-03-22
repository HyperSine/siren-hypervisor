#pragma once
#include <stdint.h>

namespace siren {
    struct hypervisor;
    struct virtual_cpu;

    struct virtual_cpu {
        virtual ~virtual_cpu() noexcept = default;

        [[nodiscard]]
        virtual hypervisor* get_hypervisor() noexcept = 0;

        [[nodiscard]]
        virtual const hypervisor* get_hypervisor() const noexcept = 0;

        [[nodiscard]]
        virtual uint32_t get_index() const noexcept = 0;

        virtual void start() noexcept = 0;

        virtual void stop() noexcept = 0;
    };
}
