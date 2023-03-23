#pragma once
#include "../memory.hpp"
#include "../expected.hpp"
#include "../nt_status.hpp"

#include "../hypervisor.hpp"
#include "../virtual_cpu.hpp"

#include "../x86/intel_vmx.hpp"

#include "msr_bitmap.hpp"
#include "dynamic_ept.hpp"

namespace siren::vmx {
    class mshv_hypervisor;
    class mshv_virtual_cpu;

    class mshv_hypervisor : public hypervisor {
        friend class mshv_virtual_cpu;
    private:
        msr_bitmap m_msr_bitmap;
        dynamic_ept m_dynamic_ept;
        unique_npaged<mshv_virtual_cpu[]> m_virtual_cpus;

        expected<void, nt_status> setup_ept_identity_map(int level, x86::paddr_t start_hpa, x86::paddr_t max_physical_address) noexcept;

    public:
        mshv_hypervisor() noexcept;

        mshv_hypervisor(const mshv_hypervisor&) = delete;

        mshv_hypervisor(mshv_hypervisor&& other) noexcept = default;

        mshv_hypervisor& operator=(const mshv_hypervisor&) = delete;

        mshv_hypervisor& operator=(mshv_hypervisor&&) noexcept = delete;

        virtual ~mshv_hypervisor() noexcept override = default;

        [[nodiscard]]
        expected<void, nt_status> intialize() noexcept;

        [[nodiscard]]
        virtual implementation_e get_implementation() const noexcept;

        [[nodiscard]]
        virtual virtual_cpu* get_virtual_cpu(uint32_t cpu_index) noexcept override;

        [[nodiscard]]
        virtual const virtual_cpu* get_virtual_cpu(uint32_t cpu_index) const noexcept override;

        [[nodiscard]]
        virtual uint32_t get_virtual_cpu_count() const noexcept override;

        virtual void start() noexcept override;

        virtual void stop() noexcept override;
    };
}
