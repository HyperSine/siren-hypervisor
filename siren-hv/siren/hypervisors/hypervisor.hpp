#pragma once
#include "../siren_global.hpp"

namespace siren::hypervisors {

    enum class implementation_e {
        X86_SVM_NATURAL,
        X86_VMX_NATURAL,
        X86_VMX_MICROSOFT_HV
    };

    struct abstract_hypervisor;
    struct abstract_virtual_cpu;

    struct abstract_hypervisor {
        virtual ~abstract_hypervisor() noexcept = default;

        [[nodiscard]]
        virtual implementation_e get_implementation() const noexcept = 0;

        [[nodiscard]]
        virtual abstract_virtual_cpu& get_virtual_cpu(size_t i) noexcept = 0;

        [[nodiscard]]
        virtual const abstract_virtual_cpu& get_virtual_cpu(size_t i) const noexcept = 0;

        [[nodiscard]]
        virtual size_t get_virtual_cpu_count() const noexcept = 0;

        virtual void start() noexcept = 0;

        virtual void stop() noexcept = 0;
    };

    struct abstract_virtual_cpu {
        static constexpr size_t stack_region_size_v = 1_mb_size_v;
        static constexpr size_t stack_region_reserved_size_v = 1_kb_size_v;

        virtual ~abstract_virtual_cpu() noexcept = default;

        [[nodiscard]]
        virtual abstract_hypervisor& get_hypervisor() noexcept = 0;

        [[nodiscard]]
        virtual const abstract_hypervisor& get_hypervisor() const noexcept = 0;

        [[nodiscard]]
        virtual uint32_t get_index() const noexcept = 0;

        virtual void start() noexcept = 0;

        virtual void stop() noexcept = 0;
    };

}
