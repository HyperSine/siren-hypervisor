#pragma once
#include "../expected.hpp"
#include "../nt_status.hpp"
#include "../memory.hpp"

#include "../hypervisor.hpp"
#include "../x86/paging.hpp"
#include "../x86/segmentation.hpp"
#include "../microsoft_hv/tlfs.hpp"

#include "msr_bitmap.hpp"

namespace siren::vmx {
    class mshv_hypervisor_t;
    class mshv_virtual_cpu_t;

    class mshv_hypervisor_t : public hypervisor_t {
    private:
        unique_npaged<mshv_virtual_cpu_t[]> m_vcpus;

    public:
        mshv_hypervisor_t() noexcept;

        mshv_hypervisor_t(const mshv_hypervisor_t&) = delete;

        mshv_hypervisor_t(mshv_hypervisor_t&& other) noexcept = default;

        mshv_hypervisor_t& operator=(const mshv_hypervisor_t&) = delete;

        mshv_hypervisor_t& operator=(mshv_hypervisor_t&&) noexcept = delete;

        virtual ~mshv_hypervisor_t() noexcept override;

        [[nodiscard]]
        nt_status intialize() noexcept;

        [[nodiscard]]
        virtual implementation_e get_implementation() const noexcept;

        [[nodiscard]]
        virtual virtual_cpu_t* get_virtual_cpu(uint32_t cpu_index) noexcept override;

        [[nodiscard]]
        virtual const virtual_cpu_t* get_virtual_cpu(uint32_t cpu_index) const noexcept override;

        [[nodiscard]]
        virtual uint32_t get_virtual_cpu_count() const noexcept override;

        virtual void start() noexcept override;

        virtual void stop() noexcept override;
    };

    class mshv_virtual_cpu_t : public virtual_cpu_t {
    public:
        struct alignas(uintptr_t) vmexit_stack_t {
            uint8_t in_use[1_MiB_size_v - 1_KiB_size_v];
            mshv_virtual_cpu_t* self;
            uint8_t reserved[1_KiB_size_v - sizeof(self)];
        };

        static_assert(sizeof(vmexit_stack_t) == 1_MiB_size_v);
        static_assert(alignof(vmexit_stack_t) == alignof(uintptr_t));

    private:
        mshv_hypervisor_t& m_hypervisor;

        uint32_t m_index;
        bool m_running;

        void* m_hypercall_page;
        x86::paddr_t m_hypercall_page_physical_address;

        microsoft_hv::vp_assist_page_t* m_vp_assist_page;
        x86::paddr_t m_vp_assist_page_physical_address;

        unique_npaged<microsoft_hv::partition_assist_page_t> m_partition_assist_page;
        x86::paddr_t m_partition_assist_page_physical_address;

        unique_npaged<microsoft_hv::vmx_enlightened_vmcs_t> m_vmxon_region;
        x86::paddr_t m_vmxon_region_physical_address;

        unique_npaged<microsoft_hv::vmx_enlightened_vmcs_t> m_evmcs_region;
        x86::paddr_t m_evmcs_region_physical_address;

        unique_npaged<msr_bitmap_t> m_msr_bitmap;
        x86::paddr_t m_msr_bitmap_physical_address;

        unique_npaged<vmexit_stack_t> m_vmexit_stack;
        x86::paddr_t m_vmexit_stack_physical_address;

        template<x86::segment_register_e SegmentReg>
        void evmcs_setup_segment(const x86::gdtr_t& gdtr, const x86::segment_selector_t& ldtr, auto seg_selector, auto seg_base, auto seg_limit, auto seg_access_rights) noexcept;

        void evmcs_setup_guest() noexcept;

        void evmcs_setup_host() noexcept;

        void evmcs_setup_controls_execution() noexcept;

        void evmcs_setup_controls_exit() noexcept;

        void evmcs_setup_controls_entry() noexcept;

        [[msvc::noinline]]
        void launch() noexcept;

    public:
        mshv_virtual_cpu_t(mshv_hypervisor_t& hypervisor, uint32_t index) noexcept;

        // copy constructor is not allowed
        mshv_virtual_cpu_t(const mshv_virtual_cpu_t&) = delete;

        // move constructor
        mshv_virtual_cpu_t(mshv_virtual_cpu_t&& other) noexcept = default;

        // copy assignment is not allowed
        mshv_virtual_cpu_t& operator=(const mshv_virtual_cpu_t&) = delete;

        // move assignment is not allowed
        mshv_virtual_cpu_t& operator=(mshv_virtual_cpu_t&&) noexcept = delete;

        virtual ~mshv_virtual_cpu_t() noexcept override;

        [[nodiscard]]
        nt_status intialize() noexcept;

        [[nodiscard]]
        virtual hypervisor_t& get_hypervisor() noexcept override;

        [[nodiscard]]
        virtual const hypervisor_t& get_hypervisor() const noexcept override;

        [[nodiscard]]
        virtual uint32_t get_index() const noexcept override;

        virtual void start() noexcept override;

        virtual void stop() noexcept override;

        [[nodiscard]]
        microsoft_hv::vmx_enlightened_vmcs_t* get_enlightened_vmcs() noexcept;

        [[nodiscard]]
        const microsoft_hv::vmx_enlightened_vmcs_t* get_enlightened_vmcs() const noexcept;

        [[nodiscard]]
        const void* get_hypercall_page() const noexcept;

        void inject_bp_exception() noexcept;

        void inject_ud_exception() noexcept;

        void inject_gp_exception(uint32_t error_code = 0) noexcept;
    };
}
