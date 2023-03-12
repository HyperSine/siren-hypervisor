#pragma once
#include <stdint.h>
#include "concepts.hpp"
#include "utility.hpp"

namespace siren {
    struct nt_status {
        static constexpr uint32_t severity_success_v = 0b00;
        static constexpr uint32_t severity_informational_v = 0b01;
        static constexpr uint32_t severity_warning_v = 0b10;
        static constexpr uint32_t severity_error_v = 0b11;

        static constexpr uint32_t facility_debugger_v = 0x1;
        static constexpr uint32_t facility_rpc_runtime_v = 0x2;
        static constexpr uint32_t facility_rpc_stubs_v = 0x3;
        static constexpr uint32_t facility_io_error_code_v = 0x4;
        static constexpr uint32_t facility_codclass_error_code_v = 0x6;
        static constexpr uint32_t facility_ntwin32_v = 0x7;
        static constexpr uint32_t facility_ntcert_v = 0x8;
        static constexpr uint32_t facility_ntsspi_v = 0x9;
        static constexpr uint32_t facility_terminal_server_v = 0xa;
        static constexpr uint32_t facility_mui_error_code_v = 0xb;
        static constexpr uint32_t facility_usb_error_code_v = 0x10;
        static constexpr uint32_t facility_hid_error_code_v = 0x11;
        static constexpr uint32_t facility_firewire_error_code_v = 0x12;
        static constexpr uint32_t facility_cluster_error_code_v = 0x13;
        static constexpr uint32_t facility_acpi_error_code_v = 0x14;
        static constexpr uint32_t facility_sxs_error_code_v = 0x15;
        static constexpr uint32_t facility_transaction_v = 0x19;
        static constexpr uint32_t facility_commonlog_v = 0x1a;
        static constexpr uint32_t facility_video_v = 0x1b;
        static constexpr uint32_t facility_filter_manager_v = 0x1c;
        static constexpr uint32_t facility_monitor_v = 0x1d;
        static constexpr uint32_t facility_graphics_kernel_v = 0x1e;
        static constexpr uint32_t facility_driver_framework_v = 0x20;
        static constexpr uint32_t facility_fve_error_code_v = 0x21;
        static constexpr uint32_t facility_fwp_error_code_v = 0x22;
        static constexpr uint32_t facility_ndis_error_code_v = 0x23;
        static constexpr uint32_t facility_quic_error_code_v = 0x24;
        static constexpr uint32_t facility_tpm_v = 0x29;
        static constexpr uint32_t facility_rtpm_v = 0x2a;
        static constexpr uint32_t facility_hypervisor_v = 0x35;
        static constexpr uint32_t facility_ipsec_v = 0x36;
        static constexpr uint32_t facility_virtualization_v = 0x37;
        static constexpr uint32_t facility_volmgr_v = 0x38;
        static constexpr uint32_t facility_bcd_error_code_v = 0x39;
        static constexpr uint32_t facility_win32k_ntuser_v = 0x3e;
        static constexpr uint32_t facility_win32k_ntgdi_v = 0x3f;
        static constexpr uint32_t facility_resume_key_filter_v = 0x40;
        static constexpr uint32_t facility_rdbss_v = 0x41;
        static constexpr uint32_t facility_bth_att_v = 0x42;
        static constexpr uint32_t facility_secureboot_v = 0x43;
        static constexpr uint32_t facility_audio_kernel_v = 0x44;
        static constexpr uint32_t facility_vsm_v = 0x45;
        static constexpr uint32_t facility_nt_ioring_v = 0x46;
        static constexpr uint32_t facility_volsnap_v = 0x50;
        static constexpr uint32_t facility_sdbus_v = 0x51;
        static constexpr uint32_t facility_shared_vhdx_v = 0x5c;
        static constexpr uint32_t facility_smb_v = 0x5d;
        static constexpr uint32_t facility_xvs_v = 0x5e;
        static constexpr uint32_t facility_interix_v = 0x99;
        static constexpr uint32_t facility_spaces_v = 0xe7;
        static constexpr uint32_t facility_security_core_v = 0xe8;
        static constexpr uint32_t facility_system_integrity_v = 0xe9;
        static constexpr uint32_t facility_licensing_v = 0xea;
        static constexpr uint32_t facility_platform_manifest_v = 0xeb;
        static constexpr uint32_t facility_app_exec_v = 0xec;

        uint32_t value;

        [[nodiscard]]
        friend constexpr bool operator==(const nt_status& lhs, const nt_status& rhs) noexcept = default;

        [[nodiscard]]
        friend constexpr bool operator!=(const nt_status& lhs, const nt_status& rhs) noexcept = default;

        [[nodiscard]]
        constexpr uint32_t code() const noexcept {
            return range_bits_t<uint32_t, 0, 16>::extract_from(value).value();
        }

        [[nodiscard]]
        constexpr uint32_t facility() const noexcept {
            return range_bits_t<uint32_t, 16, 28>::extract_from(value).value();
        }

        [[nodiscard]]
        constexpr uint32_t customer_flag() const noexcept {
            return range_bits_t<uint32_t, 29, 30>::extract_from(value).value();
        }

        [[nodiscard]]
        constexpr uint32_t severity() const noexcept {
            return range_bits_t<uint32_t, 30, 32>::extract_from(value).value();
        }

        constexpr void code(uint32_t c) noexcept {
            range_bits_t<uint32_t, 0, 16>{ c }.assign_to(value);
        }

        constexpr void facility(uint32_t f) noexcept {
            range_bits_t<uint32_t, 16, 28>{ f }.assign_to(value);
        }

        constexpr void customer_flag(uint32_t cf) noexcept {
            range_bits_t<uint32_t, 29, 30>{ cf }.assign_to(value);
        }

        constexpr void severity(uint32_t s) noexcept {
            range_bits_t<uint32_t, 30, 32>{ s }.assign_to(value);
        }

        [[nodiscard]]
        constexpr bool is_success() const noexcept {
            return range_bits_t<uint32_t, 31, 32>::extract_from(value).value() == 0;
        }

        template<integral_exact<32> Ty>
        [[nodiscard]]
        static constexpr nt_status from(Ty value) noexcept {
            return { static_cast<uint32_t>(value) };
        }
    };

    static_assert(std::is_aggregate_v<nt_status>);

    constexpr nt_status nt_status_success_v = { 0x00000000u };
    constexpr nt_status nt_status_not_implemented_v = { 0xc0000002u };
    constexpr nt_status nt_status_invalid_parameter_v = { 0xc000000du };
}
