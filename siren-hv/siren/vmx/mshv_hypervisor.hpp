#pragma once
#include "../hypervisor.hpp"
#include "../../siren_memory.hpp"
#include "../../x86/paging.hpp"

namespace siren::vmx {
    class mshv_hypervisor_t;
    class mshv_virtual_cpu_t;

    class mshv_hypervisor_t final : public hypervisor_t {
    private:
        std::byte* m_hypercall_page;
        x86::physical_address_t m_hypercall_page_physical_address;

        managed_object_ptr<mshv_virtual_cpu[]> m_virtual_cpus;

    public:
        mshv_hypervisor_t() noexcept;

        virtual ~mshv_hypervisor_t() noexcept override;

        [[nodiscard]]
        status_t intialize() noexcept;

        [[nodiscard]]
        virtual implementation_e get_implementation() const noexcept;

        [[nodiscard]]
        virtual abstract_virtual_cpu& get_virtual_cpu(size_t i) noexcept override;

        [[nodiscard]]
        virtual const abstract_virtual_cpu& get_virtual_cpu(size_t i) const noexcept override;

        [[nodiscard]]
        virtual size_t get_virtual_cpu_count() const noexcept override;

        virtual void start() noexcept override;

        virtual void stop() noexcept override;
    };
}
