#pragma once
#include "guest_state.hpp"
#include "../../microsoft_hv/tlfs.hypercalls.hpp"

namespace siren::hypervisors::vmx {

    class mshv_hypervisor;
    class mshv_virtual_cpu;

    class mshv_vmexit_handler {
    private:
        static void advance_rip(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        static microsoft_hv::hypercalls::result_value_t microsoft_hypercall(const void* hypercall_page, microsoft_hv::hypercalls::input_value_t input_value, microsoft_hv::gpa_t input_param_address, microsoft_hv::gpa_t output_param_address) noexcept;

        static microsoft_hv::hypercalls::result_value_t microsoft_fast_hypercall_ex(const void* hypercall_page, microsoft_hv::hypercalls::input_value_t input_value, void* input_output_param_block) noexcept;

        [[nodiscard]]
        static bool siren_hypercall_echo(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool siren_hypercall_turn_off_vm(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool siren_hypercall_ept_commit_1gb_page(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool siren_hypercall_ept_commit_2mb_page(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool siren_hypercall_ept_commit_4kb_page(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool siren_hypercall_ept_uncommit_1gb_page(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool siren_hypercall_ept_uncommit_2mb_page(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool siren_hypercall_ept_uncommit_4kb_page(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool siren_hypercall_ept_flush(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool siren_hypercall_not_implemented(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool dispatcher(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool handle_cr_access(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool handle_cpuid(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool handle_hlt(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool handle_invlpg(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool handle_wbinvd(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool handle_rdmsr(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool handle_wrmsr(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

        [[nodiscard]]
        static bool handle_vmcall(mshv_virtual_cpu* virtual_cpu, guest_registers_t* guest_registers) noexcept;

    public:
        static void entry() noexcept;
    };

}
