#pragma once
#include "../hypervisor.hpp"
#include "../../siren_memory.hpp"
#include "../../microsoft_hv/tlfs.hpp"

namespace siren::hypervisors::vmx {

    class mshv_hypervisor;

    class mshv_virtual_cpu : public abstract_virtual_cpu {
    private:
        mshv_hypervisor& m_hypervisor;

        uint32_t m_index;
        bool m_running;

        std::byte* m_hypercall_page;
        x86::physical_address_t m_hypercall_page_physical_address;

        microsoft_hv::vp_assist_page_t* m_vp_assist_page;
        x86::physical_address_t m_vp_assist_page_physical_address;

        managed_memory_ptr<microsoft_hv::partition_assist_page_t> m_partition_assist_page;
        x86::physical_address_t m_partition_assist_page_physical_address;

        managed_memory_ptr<microsoft_hv::vmx_enlightened_vmcs_t> m_vmxon_region;
        x86::physical_address_t m_vmxon_region_physical_address;

        managed_memory_ptr<microsoft_hv::vmx_enlightened_vmcs_t> m_evmcs_region;
        x86::physical_address_t m_evmcs_region_physical_address;

        managed_memory_ptr<std::byte[]> m_msr_bitmap;
        x86::physical_address_t m_msr_bitmap_physical_address;

        managed_memory_ptr<std::byte[]> m_stack_region;
        x86::physical_address_t m_stack_region_physical_address;

        void evmcs_setup_guest() noexcept;

        void evmcs_setup_host() noexcept;

        void evmcs_setup_controls_execution() noexcept;

        void evmcs_setup_controls_exit() noexcept;

        void evmcs_setup_controls_entry() noexcept;

        [[msvc::noinline]]
        void launch() noexcept;

    public:
        mshv_virtual_cpu(mshv_hypervisor& hypervisor, uint32_t index) noexcept;

        virtual ~mshv_virtual_cpu() noexcept override;

        [[nodiscard]]
        status_t init() noexcept;

        [[nodiscard]]
        virtual abstract_hypervisor& get_hypervisor() noexcept override;

        [[nodiscard]]
        virtual const abstract_hypervisor& get_hypervisor() const noexcept override;

        [[nodiscard]]
        virtual uint32_t get_index() const noexcept override;

        virtual void start() noexcept override;

        virtual void stop() noexcept override;

        [[nodiscard]]
        microsoft_hv::vmx_enlightened_vmcs_t* get_enlightened_vmcs() noexcept;

        [[nodiscard]]
        const microsoft_hv::vmx_enlightened_vmcs_t* get_enlightened_vmcs() const noexcept;

        [[nodiscard]]
        const std::byte* get_hypercall_page() const noexcept;

        void inject_bp_exception() noexcept;

        void inject_ud_exception() noexcept;

        void inject_gp_exception(uint32_t error_code = 0) noexcept;
    };

}
