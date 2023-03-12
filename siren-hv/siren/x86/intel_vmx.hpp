#pragma once
#include <intrin.h>
#include "paging.hpp"
#include "../literals.hpp"
#include "../utility.hpp"

namespace siren::x86 {
    using namespace ::siren::size_literals;

    // Defined in 
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.11 Software Use of the VMCS and Related Structures
    //      |-> 24.11.5 VMXON Region
    struct alignas(4_KiB_size_v) vmxon_region_t {
        uint32_t revision : 31;
        uint32_t reserved : 1;
        uint8_t used_by_cpu[1];     // ANYSIZE_ARRAY
    };

    static_assert(alignof(vmxon_region_t) == 4_KiB_size_v);

    // Defined in 
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.2 Format of the VMCS Region
    //      |-> Table 24-1. Format of the VMCS Region
    struct alignas(4_KiB_size_v) vmcs_region_t {
        uint32_t revision : 31;
        uint32_t shadow_vmcs_indicator : 1;
        uint32_t vmx_abort_indicator;
        uint8_t used_by_cpu[1];     // ANYSIZE_ARRAY
    };

    static_assert(alignof(vmcs_region_t) == 4_KiB_size_v);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Appendix C VMX Basic Exit Reasons
    //    |-> Table C-1. Basic Exit Reasons
    enum class vmx_exit_reason_e : uint16_t {
        EXCEPTION_OR_SMI = 0,
        EXTERNAL_INTERRIPT = 1,
        TRIPLE_FAULT = 2,
        INIT_SIGNAL = 3,
        STARTUP_IPI = 4,
        IO_SMI = 5,
        OTHER_SMI = 6,
        INTERRUPT_WINDOW = 7,
        NMI_WINDOW = 8,
        TASK_SWITCH = 9,
        INSTRUCTION_CPUID = 10,
        INSTRUCTION_GETSEC = 11,
        INSTRUCTION_HLT = 12,
        INSTRUCTION_INVD = 13,
        INSTRUCTION_INVLPG = 14,
        INSTRUCTION_RDPMC = 15,
        INSTRUCTION_RDTSC = 16,
        INSTRUCTION_RSM = 17,
        INSTRUCTION_VMCALL = 18,
        INSTRUCTION_VMCLEAR = 19,
        INSTRUCTION_VMLAUNCH = 20,
        INSTRUCTION_VMPTRLD = 21,
        INSTRUCTION_VMPTRST = 22,
        INSTRUCTION_VMREAD = 23,
        INSTRUCTION_VMRESUME = 24,
        INSTRUCTION_VMWRITE = 25,
        INSTRUCTION_VMXOFF = 26,
        INSTRUCTION_VMXON = 27,
        CR_ACCESS = 28,
        MOV_DR = 29,
        IO_INSTRUCTION = 30,
        INSTRUCTION_RDMSR = 31,
        INSTRUCTION_WRMSR = 32,
        VMENTRY_FAILURE_INVALID_GUEST_STATE = 33,
        VMENTRY_FAILURE_MSR_LOADING = 34,
        INSTRUCTION_MWAIT = 36,
        MONITOR_TRAP_FLAG = 37,
        INSTRUCTION_MONITOR = 39,
        INSTRUCTION_PAUSE = 40,
        VMENTRY_FAILURE_MACHINE_CHECK_EVENT = 41,
        TPR_BELOW_THRESHOLD = 43,
        APIC_ACCESS = 44,
        VIRTUALIZED_EOI = 45,
        GDTR_OR_IDTR_ACCESS = 46,
        LDTR_OR_TR_ACCESS = 47,
        EPT_VIOLATION = 48,
        EPT_MISCONFIGURATION = 49,
        INSTRUCTION_INVEPT = 50,
        INSTRUCTION_RDTSCP = 51,
        VMX_PREEMPTION_TIMER_EXPIRED = 52,
        INSTRUCTION_INVVPID = 53,
        INSTRUCTION_WBINVD_OR_WBNOINVD = 54,
        INSTRUCTION_XSETBV = 55,
        APIC_WRITE = 56,
        INSTRUCTION_RDRAND = 57,
        INSTRUCTION_INVPCID = 58,
        INSTRUCTION_VMFUNC = 59,
        INSTRUCTION_ENCLS = 60,
        INSTRUCTION_RDSEED = 61,
        INSTRUCTION_PAGE_MODIFICATION_LOG_FULL = 62,
        INSTRUCTION_XSAVES = 63,
        INSTRUCTION_XRSTORS = 64,
        INSTRUCTION_PCONFIG = 65,
        SPP_RELATED_EVENT = 66,
        INSTRUCTION_UMWAIT = 67,
        INSTRUCTION_TPAUSE = 68,
        INSTRUCTION_LOADIWKEY = 69
    };

    // ---------------------
    // vmcs field encoding 
    // ---------------------

    enum class vmcsf_access_e : uint32_t { FULL = 0, HIGH = 1 };

    enum class vmcsf_type_e : uint32_t { CONTROL = 0, INFORMATION = 1, GUEST = 2, HOST = 3 };

    enum class vmcsf_width_e : uint32_t { WORD = 0, QWORD = 1, DWORD = 2, NATURAL = 3 };

    // Defined in 
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.11 Software Use of the VMCS and Related Structures
    //      |-> 24.11.2 VMREAD, VMWRITE, and Encodings of VMCS fields
    //        |-> Table 24-20. Structure of VMCS Component Encoding
    struct vmcsf_encoding_t {
        union {
            uint32_t storage;
            struct {
                uint32_t access : 1;
                uint32_t index : 9;
                uint32_t type : 2;
                uint32_t reserved0 : 1;
                uint32_t width : 2;
                uint32_t reserved1 : 17;
            } semantics;
        };

        [[nodiscard]]
        static constexpr vmcsf_encoding_t from(uint32_t value) noexcept {
            return vmcsf_encoding_t{
                .semantics = { 
                    .access = range_bits_t<uint32_t, 0, 1>::extract_from(value).value(),
                    .index  = range_bits_t<uint32_t, 1, 10>::extract_from(value).value(),
                    .type   = range_bits_t<uint32_t, 10, 12>::extract_from(value).value(),
                    .width  = range_bits_t<uint32_t, 13, 15>::extract_from(value).value()
                }
            };
        }

        [[nodiscard]]
        static constexpr vmcsf_encoding_t from(vmcsf_access_e access, uint32_t index, vmcsf_type_e type, vmcsf_width_e width) noexcept {
            return vmcsf_encoding_t{
                .storage = 
                    range_bits_t<uint32_t, 0, 1>{ std::to_underlying(access) }.as_integral() |
                    range_bits_t<uint32_t, 1, 10>{ index }.as_integral() |
                    range_bits_t<uint32_t, 10, 12>{ std::to_underlying(type) }.as_integral() |
                    range_bits_t<uint32_t, 13, 15>{ std::to_underlying(width) }.as_integral()
            };
        }
    };

    static_assert(sizeof(vmcsf_encoding_t) == 4);
    static_assert(sizeof(vmcsf_encoding_t::storage) == sizeof(vmcsf_encoding_t::semantics));

#define SIREN_ENCODE_VMCS_FIELD(A, I, T, W) vmcsf_encoding_t::from(vmcsf_access_e::A, I, vmcsf_type_e::T, vmcsf_width_e::W).storage

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Appendix B Field Encoding in VMCS

    // -------------------------------
    //   B.1.1 16-Bit CONTROL Fields
    // -------------------------------

    constexpr uint32_t VMCSF_CTRL_VPID                                  = SIREN_ENCODE_VMCS_FIELD(FULL, 0, CONTROL, WORD);
    constexpr uint32_t VMCSF_CTRL_POSTED_INTERRUPT_NOTIFICATION_VECTOR  = SIREN_ENCODE_VMCS_FIELD(FULL, 1, CONTROL, WORD);
    constexpr uint32_t VMCSF_CTRL_EPTP_INDEX                            = SIREN_ENCODE_VMCS_FIELD(FULL, 2, CONTROL, WORD);
    constexpr uint32_t VMCSF_CTRL_HLAT_PREFIX_SIZE                      = SIREN_ENCODE_VMCS_FIELD(FULL, 3, CONTROL, WORD);

    // -----------------------------------
    //   B.1.2 16-Bit GUEST-State Fields
    // -----------------------------------

    constexpr uint32_t VMCSF_GUEST_ES_SELECTOR       = SIREN_ENCODE_VMCS_FIELD(FULL, 0, GUEST, WORD);
    constexpr uint32_t VMCSF_GUEST_CS_SELECTOR       = SIREN_ENCODE_VMCS_FIELD(FULL, 1, GUEST, WORD);
    constexpr uint32_t VMCSF_GUEST_SS_SELECTOR       = SIREN_ENCODE_VMCS_FIELD(FULL, 2, GUEST, WORD);
    constexpr uint32_t VMCSF_GUEST_DS_SELECTOR       = SIREN_ENCODE_VMCS_FIELD(FULL, 3, GUEST, WORD);
    constexpr uint32_t VMCSF_GUEST_FS_SELECTOR       = SIREN_ENCODE_VMCS_FIELD(FULL, 4, GUEST, WORD);
    constexpr uint32_t VMCSF_GUEST_GS_SELECTOR       = SIREN_ENCODE_VMCS_FIELD(FULL, 5, GUEST, WORD);
    constexpr uint32_t VMCSF_GUEST_LDTR_SELECTOR     = SIREN_ENCODE_VMCS_FIELD(FULL, 6, GUEST, WORD);
    constexpr uint32_t VMCSF_GUEST_TR_SELECTOR       = SIREN_ENCODE_VMCS_FIELD(FULL, 7, GUEST, WORD);
    constexpr uint32_t VMCSF_GUEST_INTERRUPT_STATUS  = SIREN_ENCODE_VMCS_FIELD(FULL, 8, GUEST, WORD);
    constexpr uint32_t VMCSF_GUEST_PML_INDEX         = SIREN_ENCODE_VMCS_FIELD(FULL, 9, GUEST, WORD);

    // ----------------------------------
    //   B.1.3 16-Bit HOST-State Fields
    // ----------------------------------

    constexpr uint32_t VMCSF_HOST_ES_SELECTOR = SIREN_ENCODE_VMCS_FIELD(FULL, 0, HOST, WORD);
    constexpr uint32_t VMCSF_HOST_CS_SELECTOR = SIREN_ENCODE_VMCS_FIELD(FULL, 1, HOST, WORD);
    constexpr uint32_t VMCSF_HOST_SS_SELECTOR = SIREN_ENCODE_VMCS_FIELD(FULL, 2, HOST, WORD);
    constexpr uint32_t VMCSF_HOST_DS_SELECTOR = SIREN_ENCODE_VMCS_FIELD(FULL, 3, HOST, WORD);
    constexpr uint32_t VMCSF_HOST_FS_SELECTOR = SIREN_ENCODE_VMCS_FIELD(FULL, 4, HOST, WORD);
    constexpr uint32_t VMCSF_HOST_GS_SELECTOR = SIREN_ENCODE_VMCS_FIELD(FULL, 5, HOST, WORD);
    constexpr uint32_t VMCSF_HOST_TR_SELECTOR = SIREN_ENCODE_VMCS_FIELD(FULL, 6, HOST, WORD);

    // -------------------------------
    //   B.2.1 64-Bit CONTROL Fields
    // -------------------------------

    constexpr uint32_t VMCSF_CTRL_IO_BITMAP_A_ADDRESS                                 = SIREN_ENCODE_VMCS_FIELD(FULL, 0, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_IO_BITMAP_A_ADDRESS_HIGH                            = SIREN_ENCODE_VMCS_FIELD(HIGH, 0, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_IO_BITMAP_B_ADDRESS                                 = SIREN_ENCODE_VMCS_FIELD(FULL, 1, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_IO_BITMAP_B_ADDRESS_HIGH                            = SIREN_ENCODE_VMCS_FIELD(HIGH, 1, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_MSR_BITMAP_ADDRESS                                  = SIREN_ENCODE_VMCS_FIELD(FULL, 2, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_MSR_BITMAP_ADDRESS_HIGH                             = SIREN_ENCODE_VMCS_FIELD(HIGH, 2, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VMEXIT_MSR_STORE_ADDRESS                            = SIREN_ENCODE_VMCS_FIELD(FULL, 3, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VMEXIT_MSR_STORE_ADDRESS_HIGH                       = SIREN_ENCODE_VMCS_FIELD(HIGH, 3, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VMEXIT_MSR_LOAD_ADDRESS                             = SIREN_ENCODE_VMCS_FIELD(FULL, 4, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VMEXIT_MSR_LOAD_ADDRESS_HIGH                        = SIREN_ENCODE_VMCS_FIELD(HIGH, 4, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VMENTRY_MSR_LOAD_ADDRESS                            = SIREN_ENCODE_VMCS_FIELD(FULL, 5, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VMENTRY_MSR_LOAD_ADDRESS_HIGH                       = SIREN_ENCODE_VMCS_FIELD(HIGH, 5, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_EXECUTIVE_VMCS_POINTER                              = SIREN_ENCODE_VMCS_FIELD(FULL, 6, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_EXECUTIVE_VMCS_POINTER_HIGH                         = SIREN_ENCODE_VMCS_FIELD(HIGH, 6, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_PML_ADDRESS                                         = SIREN_ENCODE_VMCS_FIELD(FULL, 7, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_PML_ADDRESS_HIGH                                    = SIREN_ENCODE_VMCS_FIELD(HIGH, 7, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_TSC_OFFSET                                          = SIREN_ENCODE_VMCS_FIELD(FULL, 8, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_TSC_OFFSET_HIGH                                     = SIREN_ENCODE_VMCS_FIELD(HIGH, 8, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VIRTUAL_APIC_ADDRESS                                = SIREN_ENCODE_VMCS_FIELD(FULL, 9, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VIRTUAL_APIC_ADDRESS_HIGH                           = SIREN_ENCODE_VMCS_FIELD(HIGH, 9, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_APIC_ACCESS_ADDRESS                                 = SIREN_ENCODE_VMCS_FIELD(FULL, 10, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_APIC_ACCESS_ADDRESS_HIGH                            = SIREN_ENCODE_VMCS_FIELD(HIGH, 10, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_POSTED_INTERRUPT_DESCRIPTOR_ADDRESS                 = SIREN_ENCODE_VMCS_FIELD(FULL, 11, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_POSTED_INTERRUPT_DESCRIPTOR_ADDRESS_HIGH            = SIREN_ENCODE_VMCS_FIELD(HIGH, 11, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VM_FUNCTION_CONTROLS                                = SIREN_ENCODE_VMCS_FIELD(FULL, 12, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VM_FUNCTION_CONTROLS_HIGH                           = SIREN_ENCODE_VMCS_FIELD(HIGH, 12, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_EPT_POINTER                                         = SIREN_ENCODE_VMCS_FIELD(FULL, 13, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_EPT_POINTER_HIGH                                    = SIREN_ENCODE_VMCS_FIELD(HIGH, 13, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_EOI_EXIT0_BITMAP                                    = SIREN_ENCODE_VMCS_FIELD(FULL, 14, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_EOI_EXIT0_BITMAP_HIGH                               = SIREN_ENCODE_VMCS_FIELD(HIGH, 14, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_EOI_EXIT1_BITMAP                                    = SIREN_ENCODE_VMCS_FIELD(FULL, 15, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_EOI_EXIT1_BITMAP_HIGH                               = SIREN_ENCODE_VMCS_FIELD(HIGH, 15, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_EOI_EXIT2_BITMAP                                    = SIREN_ENCODE_VMCS_FIELD(FULL, 16, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_EOI_EXIT2_BITMAP_HIGH                               = SIREN_ENCODE_VMCS_FIELD(HIGH, 16, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_EOI_EXIT3_BITMAP                                    = SIREN_ENCODE_VMCS_FIELD(FULL, 17, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_EOI_EXIT3_BITMAP_HIGH                               = SIREN_ENCODE_VMCS_FIELD(HIGH, 17, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_EPTP_LIST_ADDRESS                                   = SIREN_ENCODE_VMCS_FIELD(FULL, 18, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_EPTP_LIST_ADDRESS_HIGH                              = SIREN_ENCODE_VMCS_FIELD(HIGH, 18, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VMREAD_BITMAP_ADDRESS                               = SIREN_ENCODE_VMCS_FIELD(FULL, 19, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VMREAD_BITMAP_ADDRESS_HIGH                          = SIREN_ENCODE_VMCS_FIELD(HIGH, 19, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VMWRITE_BITMAP_ADDRESS                              = SIREN_ENCODE_VMCS_FIELD(FULL, 20, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VMWRITE_BITMAP_ADDRESS_HIGH                         = SIREN_ENCODE_VMCS_FIELD(HIGH, 20, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VIRTUALIZATION_EXCEPTION_INFO_ADDRESS               = SIREN_ENCODE_VMCS_FIELD(FULL, 21, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_VIRTUALIZATION_EXCEPTION_INFO_ADDRESS_HIGH          = SIREN_ENCODE_VMCS_FIELD(HIGH, 21, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_XSS_EXITING_BITMAP                                  = SIREN_ENCODE_VMCS_FIELD(FULL, 22, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_XSS_EXITING_BITMAP_HIGH                             = SIREN_ENCODE_VMCS_FIELD(HIGH, 22, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_ENCLS_EXITING_BITMAP                                = SIREN_ENCODE_VMCS_FIELD(FULL, 23, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_ENCLS_EXITING_BITMAP_HIGH                           = SIREN_ENCODE_VMCS_FIELD(HIGH, 23, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_SPPTP                                               = SIREN_ENCODE_VMCS_FIELD(FULL, 24, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_SPPTP_HIGH                                          = SIREN_ENCODE_VMCS_FIELD(HIGH, 24, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_TSC_MULTIPLIER                                      = SIREN_ENCODE_VMCS_FIELD(FULL, 25, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_TSC_MULTIPLIER_HIGH                                 = SIREN_ENCODE_VMCS_FIELD(HIGH, 25, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_TERTIARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS      = SIREN_ENCODE_VMCS_FIELD(FULL, 26, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_TERTIARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS_HIGH = SIREN_ENCODE_VMCS_FIELD(HIGH, 26, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_ENCLV_EXITING_BITMAP                                = SIREN_ENCODE_VMCS_FIELD(FULL, 27, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_ENCLV_EXITING_BITMAP_HIGH                           = SIREN_ENCODE_VMCS_FIELD(HIGH, 27, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_PCONFIG_EXITING_BITMAP                              = SIREN_ENCODE_VMCS_FIELD(FULL, 31, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_PCONFIG_EXITING_BITMAP_HIGH                         = SIREN_ENCODE_VMCS_FIELD(HIGH, 31, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_HLATP                                               = SIREN_ENCODE_VMCS_FIELD(FULL, 32, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_HLATP_HIGH                                          = SIREN_ENCODE_VMCS_FIELD(HIGH, 32, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_SECONDARY_VMEXIT_CONTROLS                           = SIREN_ENCODE_VMCS_FIELD(FULL, 34, CONTROL, QWORD);
    constexpr uint32_t VMCSF_CTRL_SECONDARY_VMEXIT_CONTROLS_HIGH                      = SIREN_ENCODE_VMCS_FIELD(HIGH, 34, CONTROL, QWORD);

    // --------------------------------------
    //   B.2.2 64-Bit Read-Only Data Fields
    // --------------------------------------
    constexpr uint32_t VMCSF_INFO_GUEST_PHYSICAL_ADDRESS      = SIREN_ENCODE_VMCS_FIELD(FULL, 0, INFORMATION, QWORD);
    constexpr uint32_t VMCSF_INFO_GUEST_PHYSICAL_ADDRESS_HIGH = SIREN_ENCODE_VMCS_FIELD(FULL, 0, INFORMATION, QWORD);

    // -----------------------------------
    //   B.2.3 64-Bit GUEST-State Fields
    // -----------------------------------

    constexpr uint32_t VMCSF_GUEST_VMCS_LINK_POINTER             = SIREN_ENCODE_VMCS_FIELD(FULL, 0, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_VMCS_LINK_POINTER_HIGH        = SIREN_ENCODE_VMCS_FIELD(HIGH, 0, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_DEBUGCTL                 = SIREN_ENCODE_VMCS_FIELD(FULL, 1, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_DEBUGCTL_HIGH            = SIREN_ENCODE_VMCS_FIELD(HIGH, 1, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_PAT                      = SIREN_ENCODE_VMCS_FIELD(FULL, 2, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_PAT_HIGH                 = SIREN_ENCODE_VMCS_FIELD(HIGH, 2, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_EFER                     = SIREN_ENCODE_VMCS_FIELD(FULL, 3, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_EFER_HIGH                = SIREN_ENCODE_VMCS_FIELD(HIGH, 3, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_PERF_GLOBAL_CTRL         = SIREN_ENCODE_VMCS_FIELD(FULL, 4, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_PERF_GLOBAL_CTRL_HIGH    = SIREN_ENCODE_VMCS_FIELD(HIGH, 4, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_PDPTE0                        = SIREN_ENCODE_VMCS_FIELD(FULL, 5, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_PDPTE0_HIGH                   = SIREN_ENCODE_VMCS_FIELD(HIGH, 5, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_PDPTE1                        = SIREN_ENCODE_VMCS_FIELD(FULL, 6, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_PDPTE1_HIGH                   = SIREN_ENCODE_VMCS_FIELD(HIGH, 6, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_PDPTE2                        = SIREN_ENCODE_VMCS_FIELD(FULL, 7, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_PDPTE2_HIGH                   = SIREN_ENCODE_VMCS_FIELD(HIGH, 7, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_PDPTE3                        = SIREN_ENCODE_VMCS_FIELD(FULL, 8, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_PDPTE3_HIGH                   = SIREN_ENCODE_VMCS_FIELD(HIGH, 8, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_BNDCFGS                  = SIREN_ENCODE_VMCS_FIELD(FULL, 9, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_BNDCFGS_HIGH             = SIREN_ENCODE_VMCS_FIELD(HIGH, 9, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_RTIT_CTL                 = SIREN_ENCODE_VMCS_FIELD(FULL, 10, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_RTIT_CTL_HIGH            = SIREN_ENCODE_VMCS_FIELD(HIGH, 10, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_LBR_CTL                  = SIREN_ENCODE_VMCS_FIELD(FULL, 11, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_LBR_CTL_HIGH             = SIREN_ENCODE_VMCS_FIELD(HIGH, 11, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_PKRS                     = SIREN_ENCODE_VMCS_FIELD(FULL, 12, GUEST, QWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_PKRS_HIGH                = SIREN_ENCODE_VMCS_FIELD(HIGH, 12, GUEST, QWORD);

    // ----------------------------------
    //   B.2.4 64-Bit HOST-State Fields
    // ----------------------------------

    constexpr uint32_t VMCSF_HOST_IA32_PAT                   = SIREN_ENCODE_VMCS_FIELD(FULL, 0, HOST, QWORD);
    constexpr uint32_t VMCSF_HOST_IA32_PAT_HIGH              = SIREN_ENCODE_VMCS_FIELD(HIGH, 0, HOST, QWORD);
    constexpr uint32_t VMCSF_HOST_IA32_EFER                  = SIREN_ENCODE_VMCS_FIELD(FULL, 1, HOST, QWORD);
    constexpr uint32_t VMCSF_HOST_IA32_EFER_HIGH             = SIREN_ENCODE_VMCS_FIELD(HIGH, 1, HOST, QWORD);
    constexpr uint32_t VMCSF_HOST_IA32_PERF_GLOBAL_CTRL      = SIREN_ENCODE_VMCS_FIELD(FULL, 2, HOST, QWORD);
    constexpr uint32_t VMCSF_HOST_IA32_PERF_GLOBAL_CTRL_HIGH = SIREN_ENCODE_VMCS_FIELD(HIGH, 2, HOST, QWORD);
    constexpr uint32_t VMCSF_HOST_IA32_PKRS                  = SIREN_ENCODE_VMCS_FIELD(FULL, 3, HOST, QWORD);
    constexpr uint32_t VMCSF_HOST_IA32_PKRS_HIGH             = SIREN_ENCODE_VMCS_FIELD(HIGH, 3, HOST, QWORD);

    // -------------------------------
    //   B.3.1 32-Bit CONTROL Fields
    // -------------------------------

    constexpr uint32_t VMCSF_CTRL_PIN_BASED_VM_EXECUTION_CONTROLS                 = SIREN_ENCODE_VMCS_FIELD(FULL, 0, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS   = SIREN_ENCODE_VMCS_FIELD(FULL, 1, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_EXCEPTION_BITMAP                                = SIREN_ENCODE_VMCS_FIELD(FULL, 2, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_PAGEFAULT_ERROR_CODE_MASK                       = SIREN_ENCODE_VMCS_FIELD(FULL, 3, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_PAGEFAULT_ERROR_CODE_MATCH                      = SIREN_ENCODE_VMCS_FIELD(FULL, 4, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_CR3_TARGET_COUNT                                = SIREN_ENCODE_VMCS_FIELD(FULL, 5, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_PRIMARY_VMEXIT_CONTROLS                         = SIREN_ENCODE_VMCS_FIELD(FULL, 6, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_VMEXIT_MSR_STORE_COUNT                          = SIREN_ENCODE_VMCS_FIELD(FULL, 7, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_VMEXIT_MSR_LOAD_COUNT                           = SIREN_ENCODE_VMCS_FIELD(FULL, 8, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_VMENTRY_CONTROLS                                = SIREN_ENCODE_VMCS_FIELD(FULL, 9, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_VMENTRY_MSR_LOAD_COUNT                          = SIREN_ENCODE_VMCS_FIELD(FULL, 10, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_VMENTRY_INTERRUPTION_INFO                       = SIREN_ENCODE_VMCS_FIELD(FULL, 11, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_VMENTRY_EXCEPTION_ERROR_CODE                    = SIREN_ENCODE_VMCS_FIELD(FULL, 12, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_VMENTRY_INSTRUCTION_LENGTH                      = SIREN_ENCODE_VMCS_FIELD(FULL, 13, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_TPR_THRESHOLD                                   = SIREN_ENCODE_VMCS_FIELD(FULL, 14, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS = SIREN_ENCODE_VMCS_FIELD(FULL, 15, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_PLE_GAP                                         = SIREN_ENCODE_VMCS_FIELD(FULL, 16, CONTROL, DWORD);
    constexpr uint32_t VMCSF_CTRL_PLE_WINDOW                                      = SIREN_ENCODE_VMCS_FIELD(FULL, 17, CONTROL, DWORD);

    // --------------------------------------
    //   B.3.2 32-Bit Read-Only Data Fields
    // --------------------------------------

    constexpr uint32_t VMCSF_INFO_VM_INSTRUCTION_ERROR           = SIREN_ENCODE_VMCS_FIELD(FULL, 0, INFORMATION, DWORD);
    constexpr uint32_t VMCSF_INFO_EXIT_REASON                    = SIREN_ENCODE_VMCS_FIELD(FULL, 1, INFORMATION, DWORD);
    constexpr uint32_t VMCSF_INFO_VMEXIT_INTERRUPTION_INFO       = SIREN_ENCODE_VMCS_FIELD(FULL, 2, INFORMATION, DWORD);
    constexpr uint32_t VMCSF_INFO_VMEXIT_INTERRUPTION_ERROR_CODE = SIREN_ENCODE_VMCS_FIELD(FULL, 3, INFORMATION, DWORD);
    constexpr uint32_t VMCSF_INFO_IDT_VECTORING_INFO             = SIREN_ENCODE_VMCS_FIELD(FULL, 4, INFORMATION, DWORD);
    constexpr uint32_t VMCSF_INFO_IDT_VECTORING_ERROR_CODE       = SIREN_ENCODE_VMCS_FIELD(FULL, 5, INFORMATION, DWORD);
    constexpr uint32_t VMCSF_INFO_VMEXIT_INSTRUCTION_LENGTH      = SIREN_ENCODE_VMCS_FIELD(FULL, 6, INFORMATION, DWORD);
    constexpr uint32_t VMCSF_INFO_VMEXIT_INSTRUCTION_INFO        = SIREN_ENCODE_VMCS_FIELD(FULL, 7, INFORMATION, DWORD);

    // -----------------------------------
    //   B.3.3 32-Bit GUEST-State Fields
    // -----------------------------------

    constexpr uint32_t VMCSF_GUEST_ES_LIMIT                      = SIREN_ENCODE_VMCS_FIELD(FULL, 0, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_CS_LIMIT                      = SIREN_ENCODE_VMCS_FIELD(FULL, 1, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_SS_LIMIT                      = SIREN_ENCODE_VMCS_FIELD(FULL, 2, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_DS_LIMIT                      = SIREN_ENCODE_VMCS_FIELD(FULL, 3, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_FS_LIMIT                      = SIREN_ENCODE_VMCS_FIELD(FULL, 4, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_GS_LIMIT                      = SIREN_ENCODE_VMCS_FIELD(FULL, 5, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_LDTR_LIMIT                    = SIREN_ENCODE_VMCS_FIELD(FULL, 6, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_TR_LIMIT                      = SIREN_ENCODE_VMCS_FIELD(FULL, 7, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_GDTR_LIMIT                    = SIREN_ENCODE_VMCS_FIELD(FULL, 8, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_IDTR_LIMIT                    = SIREN_ENCODE_VMCS_FIELD(FULL, 9, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_ES_ACCESS_RIGHTS              = SIREN_ENCODE_VMCS_FIELD(FULL, 10, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_CS_ACCESS_RIGHTS              = SIREN_ENCODE_VMCS_FIELD(FULL, 11, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_SS_ACCESS_RIGHTS              = SIREN_ENCODE_VMCS_FIELD(FULL, 12, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_DS_ACCESS_RIGHTS              = SIREN_ENCODE_VMCS_FIELD(FULL, 13, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_FS_ACCESS_RIGHTS              = SIREN_ENCODE_VMCS_FIELD(FULL, 14, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_GS_ACCESS_RIGHTS              = SIREN_ENCODE_VMCS_FIELD(FULL, 15, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_LDTR_ACCESS_RIGHTS            = SIREN_ENCODE_VMCS_FIELD(FULL, 16, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_TR_ACCESS_RIGHTS              = SIREN_ENCODE_VMCS_FIELD(FULL, 17, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_INTERRUPTIBILITY_STATE        = SIREN_ENCODE_VMCS_FIELD(FULL, 18, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_ACTIVITY_STATE                = SIREN_ENCODE_VMCS_FIELD(FULL, 19, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_SMBASE                        = SIREN_ENCODE_VMCS_FIELD(FULL, 20, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_IA32_SYSENTER_CS              = SIREN_ENCODE_VMCS_FIELD(FULL, 21, GUEST, DWORD);
    constexpr uint32_t VMCSF_GUEST_VMX_PREEMPTION_TIMER_VALUE    = SIREN_ENCODE_VMCS_FIELD(FULL, 23, GUEST, DWORD);

    // ---------------------------------
    //   B.3.4 32-Bit HOST-State Field
    // ---------------------------------

    constexpr uint32_t VMCSF_HOST_IA32_SYSENTER_CS = SIREN_ENCODE_VMCS_FIELD(FULL, 0, HOST, DWORD);

    // --------------------------------------
    //   B.4.1 NATURAL-Width CONTROL Fields
    // --------------------------------------

    constexpr uint32_t VMCSF_CTRL_CR0_GUEST_HOST_MASK = SIREN_ENCODE_VMCS_FIELD(FULL, 0, CONTROL, NATURAL);
    constexpr uint32_t VMCSF_CTRL_CR4_GUEST_HOST_MASK = SIREN_ENCODE_VMCS_FIELD(FULL, 1, CONTROL, NATURAL);
    constexpr uint32_t VMCSF_CTRL_CR0_READ_SHADOW     = SIREN_ENCODE_VMCS_FIELD(FULL, 2, CONTROL, NATURAL);
    constexpr uint32_t VMCSF_CTRL_CR4_READ_SHADOW     = SIREN_ENCODE_VMCS_FIELD(FULL, 3, CONTROL, NATURAL);
    constexpr uint32_t VMCSF_CTRL_CR3_TARGET_VALUE0   = SIREN_ENCODE_VMCS_FIELD(FULL, 4, CONTROL, NATURAL);
    constexpr uint32_t VMCSF_CTRL_CR3_TARGET_VALUE1   = SIREN_ENCODE_VMCS_FIELD(FULL, 5, CONTROL, NATURAL);
    constexpr uint32_t VMCSF_CTRL_CR3_TARGET_VALUE2   = SIREN_ENCODE_VMCS_FIELD(FULL, 6, CONTROL, NATURAL);
    constexpr uint32_t VMCSF_CTRL_CR3_TARGET_VALUE3   = SIREN_ENCODE_VMCS_FIELD(FULL, 7, CONTROL, NATURAL);

    // ---------------------------------------------
    //   B.4.2 NATURAL-Width Read-Only Data Fields
    // ---------------------------------------------

    constexpr uint32_t VMCSF_INFO_EXIT_QUALIFICATION     = SIREN_ENCODE_VMCS_FIELD(FULL, 0, INFORMATION, NATURAL);
    constexpr uint32_t VMCSF_INFO_IO_RCX                 = SIREN_ENCODE_VMCS_FIELD(FULL, 1, INFORMATION, NATURAL);
    constexpr uint32_t VMCSF_INFO_IO_RSI                 = SIREN_ENCODE_VMCS_FIELD(FULL, 2, INFORMATION, NATURAL);
    constexpr uint32_t VMCSF_INFO_IO_RDI                 = SIREN_ENCODE_VMCS_FIELD(FULL, 3, INFORMATION, NATURAL);
    constexpr uint32_t VMCSF_INFO_IO_RIP                 = SIREN_ENCODE_VMCS_FIELD(FULL, 4, INFORMATION, NATURAL);
    constexpr uint32_t VMCSF_INFO_GUEST_LINEAR_ADDRESS   = SIREN_ENCODE_VMCS_FIELD(FULL, 5, INFORMATION, NATURAL);

    // ------------------------------------------
    //   B.4.3 NATURAL-Width GUEST-State Fields
    // ------------------------------------------

    constexpr uint32_t VMCSF_GUEST_CR0                           = SIREN_ENCODE_VMCS_FIELD(FULL, 0, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_CR3                           = SIREN_ENCODE_VMCS_FIELD(FULL, 1, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_CR4                           = SIREN_ENCODE_VMCS_FIELD(FULL, 2, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_ES_BASE                       = SIREN_ENCODE_VMCS_FIELD(FULL, 3, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_CS_BASE                       = SIREN_ENCODE_VMCS_FIELD(FULL, 4, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_SS_BASE                       = SIREN_ENCODE_VMCS_FIELD(FULL, 5, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_DS_BASE                       = SIREN_ENCODE_VMCS_FIELD(FULL, 6, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_FS_BASE                       = SIREN_ENCODE_VMCS_FIELD(FULL, 7, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_GS_BASE                       = SIREN_ENCODE_VMCS_FIELD(FULL, 8, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_LDTR_BASE                     = SIREN_ENCODE_VMCS_FIELD(FULL, 9, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_TR_BASE                       = SIREN_ENCODE_VMCS_FIELD(FULL, 10, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_GDTR_BASE                     = SIREN_ENCODE_VMCS_FIELD(FULL, 11, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_IDTR_BASE                     = SIREN_ENCODE_VMCS_FIELD(FULL, 12, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_DR7                           = SIREN_ENCODE_VMCS_FIELD(FULL, 13, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_RSP                           = SIREN_ENCODE_VMCS_FIELD(FULL, 14, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_RIP                           = SIREN_ENCODE_VMCS_FIELD(FULL, 15, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_RFLAGS                        = SIREN_ENCODE_VMCS_FIELD(FULL, 16, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_PENDING_DEBUG_EXCEPTIONS      = SIREN_ENCODE_VMCS_FIELD(FULL, 17, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_IA32_SYSENTER_ESP             = SIREN_ENCODE_VMCS_FIELD(FULL, 18, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_IA32_SYSENTER_EIP             = SIREN_ENCODE_VMCS_FIELD(FULL, 19, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_IA32_S_CET                    = SIREN_ENCODE_VMCS_FIELD(FULL, 20, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_SSP                           = SIREN_ENCODE_VMCS_FIELD(FULL, 21, GUEST, NATURAL);
    constexpr uint32_t VMCSF_GUEST_IA32_INTERRUPT_SSP_TABLE_ADDR = SIREN_ENCODE_VMCS_FIELD(FULL, 22, GUEST, NATURAL);

    // -----------------------------------------
    //   B.4.4 NATURAL-Width HOST-State Fields
    // -----------------------------------------
    constexpr uint32_t VMCSF_HOST_CR0                            = SIREN_ENCODE_VMCS_FIELD(FULL, 0, HOST, NATURAL);
    constexpr uint32_t VMCSF_HOST_CR3                            = SIREN_ENCODE_VMCS_FIELD(FULL, 1, HOST, NATURAL);
    constexpr uint32_t VMCSF_HOST_CR4                            = SIREN_ENCODE_VMCS_FIELD(FULL, 2, HOST, NATURAL);
    constexpr uint32_t VMCSF_HOST_FS_BASE                        = SIREN_ENCODE_VMCS_FIELD(FULL, 3, HOST, NATURAL);
    constexpr uint32_t VMCSF_HOST_GS_BASE                        = SIREN_ENCODE_VMCS_FIELD(FULL, 4, HOST, NATURAL);
    constexpr uint32_t VMCSF_HOST_TR_BASE                        = SIREN_ENCODE_VMCS_FIELD(FULL, 5, HOST, NATURAL);
    constexpr uint32_t VMCSF_HOST_GDTR_BASE                      = SIREN_ENCODE_VMCS_FIELD(FULL, 6, HOST, NATURAL);
    constexpr uint32_t VMCSF_HOST_IDTR_BASE                      = SIREN_ENCODE_VMCS_FIELD(FULL, 7, HOST, NATURAL);
    constexpr uint32_t VMCSF_HOST_IA32_SYSENTER_ESP              = SIREN_ENCODE_VMCS_FIELD(FULL, 8, HOST, NATURAL);
    constexpr uint32_t VMCSF_HOST_IA32_SYSENTER_EIP              = SIREN_ENCODE_VMCS_FIELD(FULL, 9, HOST, NATURAL);
    constexpr uint32_t VMCSF_HOST_RSP                            = SIREN_ENCODE_VMCS_FIELD(FULL, 10, HOST, NATURAL);
    constexpr uint32_t VMCSF_HOST_RIP                            = SIREN_ENCODE_VMCS_FIELD(FULL, 11, HOST, NATURAL);
    constexpr uint32_t VMCSF_HOST_IA32_S_CET                     = SIREN_ENCODE_VMCS_FIELD(FULL, 12, HOST, NATURAL);
    constexpr uint32_t VMCSF_HOST_SSP                            = SIREN_ENCODE_VMCS_FIELD(FULL, 13, HOST, NATURAL);
    constexpr uint32_t VMCSF_HOST_IA32_INTERRUPT_SSP_TABLE_ADDR  = SIREN_ENCODE_VMCS_FIELD(FULL, 14, HOST, NATURAL);

#undef SIREN_ENCODE_VMCS_FIELD

    template<uint32_t V, vmcsf_encoding_t E = vmcsf_encoding_t::from(V)>
    using vmcsf_storage_t = 
        std::conditional_t<E.semantics.width == std::to_underlying(vmcsf_width_e::WORD), uint16_t,
            std::conditional_t<E.semantics.width == std::to_underlying(vmcsf_width_e::DWORD), uint32_t,
                std::conditional_t<E.semantics.width == std::to_underlying(vmcsf_width_e::QWORD), std::conditional_t<E.semantics.access == std::to_underlying(vmcsf_access_e::HIGH), uint32_t, uint64_t>,
                    std::conditional_t<E.semantics.width == std::to_underlying(vmcsf_width_e::NATURAL), uintptr_t, void>
                >
            >
        >;

    template<uint32_t V>
    struct vmcsf_t {
        vmcsf_storage_t<V> storage;
    };

#define SIREN_VMCSF_SIZE_GUARD(V)                                               \
    static_assert(sizeof(vmcsf_t<V>) == sizeof(vmcsf_storage_t<V>));            \
    static_assert(sizeof(vmcsf_t<V>::storage) == sizeof(vmcsf_t<V>::semantics));

    template<>
    struct vmcsf_t<VMCSF_CTRL_VPID> {
        union {
            uint16_t storage;
            struct {
                uint16_t vpid;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_VPID);

    template<>
    struct vmcsf_t<VMCSF_GUEST_ES_SELECTOR> {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_ES_SELECTOR);

    template<>
    struct vmcsf_t<VMCSF_GUEST_CS_SELECTOR> {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_CS_SELECTOR);

    template<>
    struct vmcsf_t<VMCSF_GUEST_SS_SELECTOR> {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_SS_SELECTOR);

    template<>
    struct vmcsf_t<VMCSF_GUEST_DS_SELECTOR> {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_DS_SELECTOR);

    template<>
    struct vmcsf_t<VMCSF_GUEST_FS_SELECTOR> {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_FS_SELECTOR);

    template<>
    struct vmcsf_t<VMCSF_GUEST_GS_SELECTOR> {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_GS_SELECTOR);

    template<>
    struct vmcsf_t<VMCSF_GUEST_LDTR_SELECTOR> {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_LDTR_SELECTOR);

    template<>
    struct vmcsf_t<VMCSF_GUEST_TR_SELECTOR> {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_TR_SELECTOR);

    template<>
    struct vmcsf_t<VMCSF_HOST_ES_SELECTOR> {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_HOST_ES_SELECTOR);

    template<>
    struct vmcsf_t<VMCSF_HOST_CS_SELECTOR> {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_HOST_CS_SELECTOR);

    template<>
    struct vmcsf_t<VMCSF_HOST_SS_SELECTOR> {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_HOST_SS_SELECTOR);

    template<>
    struct vmcsf_t<VMCSF_HOST_DS_SELECTOR> {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_HOST_DS_SELECTOR);

    template<>
    struct vmcsf_t<VMCSF_HOST_FS_SELECTOR> {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_HOST_FS_SELECTOR);

    template<>
    struct vmcsf_t<VMCSF_HOST_GS_SELECTOR> {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_HOST_GS_SELECTOR);

    template<>
    struct vmcsf_t<VMCSF_HOST_TR_SELECTOR> {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_HOST_TR_SELECTOR);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.6 VM-Execution CONTROL Fields
    //      |-> 24.6.1 Pin-Based VM-Execution CONTROLs
    //        |-> Table 24-5. Definitions of Pin-Based VM-Execution CONTROLs
    template<>
    struct vmcsf_t<VMCSF_CTRL_PIN_BASED_VM_EXECUTION_CONTROLS> {
        union {
            uint32_t storage;
            struct {
                uint32_t external_interrupt_exiting : 1;
                uint32_t reserved0 : 2;
                uint32_t nmi_exiting : 1;
                uint32_t reserved1 : 1;
                uint32_t virtual_nmis : 1;
                uint32_t activate_vmx_preemption_timer : 1;
                uint32_t process_posted_interrupts : 1;
                uint32_t reserved2 : 24;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_PIN_BASED_VM_EXECUTION_CONTROLS);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.6 VM-Execution CONTROL Fields
    //      |-> 24.6.2 Processor-Based VM-Execution CONTROLs
    //        |-> Table 24-6. Definitions of Primary Processor-Based VM-Execution CONTROLs
    template<>
    struct vmcsf_t<VMCSF_CTRL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS> {
        union {
            uint32_t storage;
            struct {
                uint32_t reserved0 : 2;
                uint32_t interrupt_window_exiting : 1;
                uint32_t use_tsc_offsetting : 1;
                uint32_t reserved1 : 3;
                uint32_t hlt_exiting : 1;
                uint32_t reserved2 : 1;
                uint32_t invlpg_exiting : 1;
                uint32_t mwait_exiting : 1;
                uint32_t rdpmc_exiting : 1;
                uint32_t rdtsc_exiting : 1;
                uint32_t reserved3 : 2;
                uint32_t cr3_load_exiting : 1;
                uint32_t cr3_store_exiting : 1;
                uint32_t activate_tertiary_controls : 1;
                uint32_t reserved4 : 1;
                uint32_t cr8_load_exiting : 1;
                uint32_t cr8_store_exiting : 1;
                uint32_t use_tpr_shadow : 1;
                uint32_t nmi_window_exiting : 1;
                uint32_t mov_dr_exiting : 1;
                uint32_t unconditional_io_exiting : 1;
                uint32_t use_io_bitmaps : 1;
                uint32_t reserved5 : 1;
                uint32_t monitor_trap_flag : 1;
                uint32_t use_msr_bitmaps : 1;
                uint32_t monitor_exiting : 1;
                uint32_t pause_exiting : 1;
                uint32_t activate_secondary_controls : 1;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.6 VM-Execution CONTROL Fields
    //      |-> 24.6.2 Processor-Based VM-Execution CONTROLs
    //        |-> Table 24-7. Definitions of Secondary Processor-Based VM-Execution CONTROLs
    template<>
    struct vmcsf_t<VMCSF_CTRL_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS> {
        union {
            uint32_t storage;
            struct {
                uint32_t virtualize_apic_accesses : 1;
                uint32_t enable_ept : 1;
                uint32_t descriptor_table_exiting : 1;
                uint32_t enable_rdtscp : 1;
                uint32_t virtualize_x2apic_mode : 1;
                uint32_t enable_vpid : 1;
                uint32_t wbinvd_exiting : 1;
                uint32_t unrestricted_guest : 1;
                uint32_t apic_register_virtualization : 1;
                uint32_t virtual_interrupt_delivery : 1;
                uint32_t pause_loop_exiting : 1;
                uint32_t rdrand_exiting : 1;
                uint32_t enable_invpcid : 1;
                uint32_t enable_vm_functions : 1;
                uint32_t vmcs_shadowing : 1;
                uint32_t enable_encls_exiting : 1;
                uint32_t rdseed_exiting : 1;
                uint32_t enable_pml : 1;
                uint32_t raise_ve_exception_when_ept_violation : 1;
                uint32_t conceal_vmx_from_pt : 1;
                uint32_t enable_xsave_xrstors : 1;
                uint32_t reserved0 : 1;
                uint32_t mode_based_execute_control_for_ept : 1;
                uint32_t sub_page_write_permissions_for_ept : 1;
                uint32_t intel_pt_uses_guest_physical_addresses : 1;
                uint32_t use_tsc_scaling : 1;
                uint32_t enable_user_wait_and_pause : 1;
                uint32_t enable_pconfig : 1;
                uint32_t enable_enclv_exiting : 1;
                uint32_t reserved1 : 3;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.6 VM-Execution CONTROL Fields
    //      |-> 24.6.2 Processor-Based VM-Execution CONTROLs
    //        |-> Table 24-8. Definitions of Tertiary Processor-Based VM-Execution CONTROLs
    template<>
    struct vmcsf_t<VMCSF_CTRL_TERTIARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS> {
        union {
            uint64_t storage;
            struct {
                uint64_t loadiwkey_exiting : 1;
                uint64_t enable_hlat : 1;
                uint64_t ept_paging_write_control : 1;
                uint64_t guest_paging_verification : 1;
                uint64_t reserved : 60;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_TERTIARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.6 VM-Execution CONTROL Fields
    //      |-> 24.6.11 Extended-Page-Table Pointer (EPTP)
    //        |-> Table 24-9. Format of Extended-Page-Table Pointer
    template<>
    struct vmcsf_t<VMCSF_CTRL_EPT_POINTER> {
        union {
            uint64_t storage;
            struct {
                uint64_t memory_type : 3;
                uint64_t page_walk_length_minus_one : 3;
                uint64_t enable_accessed_and_dirty_flag : 1;
                uint64_t supervisor_shadow_stack_pages_access_rights_enforcement : 1;
                uint64_t reserved0 : 4;
                uint64_t pml4_physical_address : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_EPT_POINTER);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.7 VM-Exit CONTROL Fields
    //      |-> 24.7.1 VM-Exit CONTROLs
    //        |-> Table 24-13. Definitions of Primary VM-Exit CONTROLs
    template<>
    struct vmcsf_t<VMCSF_CTRL_PRIMARY_VMEXIT_CONTROLS> {
        union {
            uint32_t storage;
            struct {
                uint32_t reserved0 : 2;
                uint32_t save_debug_controls : 1;
                uint32_t reserved1 : 6;
                uint32_t host_address_space_size : 1;
                uint32_t reserved2 : 2;
                uint32_t load_ia32_perf_global_ctrl : 1;
                uint32_t reserved3 : 2;
                uint32_t acknowledge_interrupt_on_exit : 1;
                uint32_t reserved4 : 2;
                uint32_t save_ia32_pat : 1;
                uint32_t load_ia32_pat : 1;
                uint32_t save_ia32_efer : 1;
                uint32_t load_ia32_efer : 1;
                uint32_t save_vmx_preemption_timer_value : 1;
                uint32_t clear_ia32_bndcfgs : 1;
                uint32_t conceal_vmx_from_pt : 1;
                uint32_t clear_ia32_rtit_ctl : 1;
                uint32_t clear_ia32_lbr_ctl : 1;
                uint32_t load_cet_state : 1;
                uint32_t load_pkrs : 1;
                uint32_t save_ia32_perf_global_ctrl : 1;
                uint32_t activate_secondary_controls : 1;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_PRIMARY_VMEXIT_CONTROLS);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.7 VM-Exit CONTROL Fields
    //      |-> 24.7.1 VM-Exit CONTROLs
    //        |-> Table 24-13. Definitions of Primary VM-Exit CONTROLs
    template<>
    struct vmcsf_t<VMCSF_CTRL_SECONDARY_VMEXIT_CONTROLS> {
        union {
            uint64_t storage;
            struct {
                uint64_t reserved;  // Currently, no secondary VM-exit controls are defined, and all bits in this field are reserved to 0
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_SECONDARY_VMEXIT_CONTROLS);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.7 VM-Exit CONTROL Fields
    //      |-> 24.7.2 VM-Exit CONTROLs for MSRs
    template<>
    struct vmcsf_t<VMCSF_CTRL_VMEXIT_MSR_STORE_COUNT> {
        union {
            uint32_t storage;
            struct {
                uint32_t count;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_VMEXIT_MSR_STORE_COUNT);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.7 VM-Exit CONTROL Fields
    //      |-> 24.7.2 VM-Exit CONTROLs for MSRs
    template<>
    struct vmcsf_t<VMCSF_CTRL_VMEXIT_MSR_STORE_ADDRESS> {
        union {
            uint64_t storage;
            struct {
                uint64_t physical_address;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_VMEXIT_MSR_STORE_ADDRESS);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.7 VM-Exit CONTROL Fields
    //      |-> 24.7.2 VM-Exit CONTROLs for MSRs
    template<>
    struct vmcsf_t<VMCSF_CTRL_VMEXIT_MSR_LOAD_COUNT> {
        union {
            uint32_t storage;
            struct {
                uint32_t count;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_VMEXIT_MSR_LOAD_COUNT);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.7 VM-Exit CONTROL Fields
    //      |-> 24.7.2 VM-Exit CONTROLs for MSRs
    template<>
    struct vmcsf_t<VMCSF_CTRL_VMEXIT_MSR_LOAD_ADDRESS> {
        union {
            uint64_t storage;
            struct {
                uint64_t physical_address;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_VMEXIT_MSR_LOAD_ADDRESS);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.8 VM-Entry CONTROL Fields
    //      |-> 24.8.1 VM-Entry CONTROLs
    //        |-> Table 24-15. Definitions of VM-Entry CONTROLs
    template<>
    struct vmcsf_t<VMCSF_CTRL_VMENTRY_CONTROLS> {
        union {
            uint32_t storage;
            struct {
                uint32_t reserved0 : 2;
                uint32_t load_debug_controls : 1;
                uint32_t reserved1 : 6;
                uint32_t ia32e_mode_guest : 1;
                uint32_t entry_to_smm : 1;
                uint32_t deactivate_dual_monitor_treatment : 1;
                uint32_t reserved2 : 1;
                uint32_t load_ia32_perf_global_ctrl : 1;
                uint32_t load_ia32_pat : 1;
                uint32_t load_ia32_efer : 1;
                uint32_t load_ia32_bndcfgs : 1;
                uint32_t conceal_vmx_from_pt : 1;
                uint32_t load_ia32_rtit_ctl : 1;
                uint32_t reserved3 : 1;
                uint32_t load_cet_state : 1;
                uint32_t load_guest_ia32_lbr_ctl : 1;
                uint32_t load_pkrs : 1;
                uint32_t reserved4 : 9;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_VMENTRY_CONTROLS);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.8 VM-Entry CONTROL Fields
    //      |-> 24.8.2 VM-Entry CONTROLs for MSRs
    template<>
    struct vmcsf_t<VMCSF_CTRL_VMENTRY_MSR_LOAD_COUNT> {
        union {
            uint32_t storage;
            struct {
                uint32_t count;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_VMENTRY_MSR_LOAD_COUNT);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.8 VM-Entry CONTROL Fields
    //      |-> 24.8.2 VM-Entry CONTROLs for MSRs
    template<>
    struct vmcsf_t<VMCSF_CTRL_VMENTRY_MSR_LOAD_ADDRESS> {
        union {
            uint64_t storage;
            struct {
                uint64_t physical_address;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_VMENTRY_MSR_LOAD_ADDRESS);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.8 VM-Entry CONTROL Fields
    //      |-> 24.8.2 VM-Entry CONTROLs for Event Injection
    template<>
    struct vmcsf_t<VMCSF_CTRL_VMENTRY_INTERRUPTION_INFO> {
        union {
            uint32_t storage;
            struct {
                uint32_t vector_index : 8;

                // 0: external interrupt
                // 1: reserved
                // 2: non-maskable interrupt
                // 3: hardware exception
                // 4: software interrupt
                // 5: privileged software exception
                // 6: software exception
                // 7: other event
                uint32_t interruption_type : 3;

                uint32_t deliver_error_code : 1;
                uint32_t reserved : 19;
                uint32_t valid : 1;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_CTRL_VMENTRY_INTERRUPTION_INFO);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.9 VM-Exit INFORMATION Fields
    //      |-> 24.9.1 Basic VM-Exit INFORMATION
    //        |-> Table 24-17. Format of Exit Reason
    template<>
    struct vmcsf_t<VMCSF_INFO_EXIT_REASON> {
        union {
            uint32_t storage;
            struct {
                vmx_exit_reason_e basic_exit_reason;
                uint16_t always_zero : 1;
                uint16_t not_defined0 : 10;
                uint16_t is_incident_to_enclave_mode : 1;
                uint16_t pending_mtf_vm_exit : 1;
                uint16_t vm_exit_from_vmx_root_operation : 1;
                uint16_t not_defined1 : 1;
                uint16_t vm_entry_failure : 1;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_INFO_EXIT_REASON);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 24 Virtual Machine CONTROL Structures
    //    |-> 24.9 VM-Exit INFORMATION Fields
    //      |-> 24.9.2 INFORMATION for VM Exits Due to Vectored Events
    //        |-> Table 24-18. Format of the VM-Exit Interruption-INFORMATION Field
    template<>
    struct vmcsf_t<VMCSF_INFO_VMEXIT_INTERRUPTION_INFO> {
        union {
            uint32_t storage;
            struct {
                uint32_t vector_index : 8;
                uint32_t interruption_type : 3;
                uint32_t error_code_valid : 1;
                uint32_t nmi_unblocking_due_to_iret : 1;
                uint32_t reserved : 18;
                uint32_t valid : 1;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_INFO_VMEXIT_INTERRUPTION_INFO);
    
    template<>
    struct vmcsf_t<VMCSF_INFO_VMEXIT_INSTRUCTION_LENGTH> {
        union {
            uint32_t storage;
            struct {
                uint32_t length;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_INFO_VMEXIT_INSTRUCTION_LENGTH);

    template<>
    struct vmcsf_t<VMCSF_GUEST_ES_LIMIT> {
        union {
            uint32_t storage;
            struct {
                uint32_t limit;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_ES_LIMIT);

    template<>
    struct vmcsf_t<VMCSF_GUEST_CS_LIMIT> {
        union {
            uint32_t storage;
            struct {
                uint32_t limit;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_CS_LIMIT);

    template<>
    struct vmcsf_t<VMCSF_GUEST_SS_LIMIT> {
        union {
            uint32_t storage;
            struct {
                uint32_t limit;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_SS_LIMIT);

    template<>
    struct vmcsf_t<VMCSF_GUEST_DS_LIMIT> {
        union {
            uint32_t storage;
            struct {
                uint32_t limit;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_DS_LIMIT);

    template<>
    struct vmcsf_t<VMCSF_GUEST_FS_LIMIT> {
        union {
            uint32_t storage;
            struct {
                uint32_t limit;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_FS_LIMIT);

    template<>
    struct vmcsf_t<VMCSF_GUEST_GS_LIMIT> {
        union {
            uint32_t storage;
            struct {
                uint32_t limit;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_GS_LIMIT);

    template<>
    struct vmcsf_t<VMCSF_GUEST_LDTR_LIMIT> {
        union {
            uint32_t storage;
            struct {
                uint32_t limit;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_LDTR_LIMIT);

    template<>
    struct vmcsf_t<VMCSF_GUEST_TR_LIMIT> {
        union {
            uint32_t storage;
            struct {
                uint32_t limit;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_TR_LIMIT);

    template<>
    struct vmcsf_t<VMCSF_GUEST_GDTR_LIMIT> {
        union {
            uint32_t storage;
            struct {
                uint32_t limit;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_GDTR_LIMIT);

    template<>
    struct vmcsf_t<VMCSF_GUEST_IDTR_LIMIT> {
        union {
            uint32_t storage;
            struct {
                uint32_t limit;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_IDTR_LIMIT);

    template<>
    struct vmcsf_t<VMCSF_GUEST_ES_ACCESS_RIGHTS> {
        union {
            uint32_t storage;
            struct {
                uint32_t accessed : 1;
                uint32_t writable : 1;
                uint32_t direction : 1;         // 0 = grows up, 1 = grows down
                uint32_t executable : 1;        // for data segment, always 0 here.
                uint32_t descriptor_type : 1;
                uint32_t dpl : 2;
                uint32_t present : 1;
                uint32_t reserved0 : 4;
                uint32_t used_by_system_software : 1;
                uint32_t reserved1 : 1;
                uint32_t default_operation_size : 1;
                uint32_t granularity : 1;
                uint32_t unusable : 1;
                uint32_t reserved2 : 15;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_ES_ACCESS_RIGHTS);

    template<>
    struct vmcsf_t<VMCSF_GUEST_CS_ACCESS_RIGHTS> {
        union {
            uint32_t storage;
            struct {
                uint32_t accessed : 1;
                uint32_t readable : 1;
                uint32_t conforming : 1;
                uint32_t executable : 1;        // for code segment, always 1 here.
                uint32_t descriptor_type : 1;
                uint32_t dpl : 2;
                uint32_t present : 1;
                uint32_t reserved0 : 4;
                uint32_t used_by_system_software : 1;
                uint32_t long_mode : 1;
                uint32_t default_operation_size : 1;
                uint32_t granularity : 1;
                uint32_t unusable : 1;
                uint32_t reserved1 : 15;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_CS_ACCESS_RIGHTS);

    template<>
    struct vmcsf_t<VMCSF_GUEST_SS_ACCESS_RIGHTS> {
        union {
            uint32_t storage;
            struct {
                uint32_t accessed : 1;
                uint32_t writable : 1;
                uint32_t direction : 1;         // 0 = grows up, 1 = grows down
                uint32_t executable : 1;        // for data segment, always 0 here.
                uint32_t descriptor_type : 1;
                uint32_t dpl : 2;
                uint32_t present : 1;
                uint32_t reserved0 : 4;
                uint32_t used_by_system_software : 1;
                uint32_t reserved1 : 1;
                uint32_t default_operation_size : 1;
                uint32_t granularity : 1;
                uint32_t unusable : 1;
                uint32_t reserved2 : 15;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_SS_ACCESS_RIGHTS);

    template<>
    struct vmcsf_t<VMCSF_GUEST_DS_ACCESS_RIGHTS> {
        union {
            uint32_t storage;
            struct {
                uint32_t accessed : 1;
                uint32_t writable : 1;
                uint32_t direction : 1;         // 0 = grows up, 1 = grows down
                uint32_t executable : 1;        // for data segment, always 0 here.
                uint32_t descriptor_type : 1;
                uint32_t dpl : 2;
                uint32_t present : 1;
                uint32_t reserved0 : 4;
                uint32_t used_by_system_software : 1;
                uint32_t reserved1 : 1;
                uint32_t default_operation_size : 1;
                uint32_t granularity : 1;
                uint32_t unusable : 1;
                uint32_t reserved2 : 15;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_DS_ACCESS_RIGHTS);

    template<>
    struct vmcsf_t<VMCSF_GUEST_FS_ACCESS_RIGHTS> {
        union {
            uint32_t storage;
            struct {
                uint32_t accessed : 1;
                uint32_t writable : 1;
                uint32_t direction : 1;         // 0 = grows up, 1 = grows down
                uint32_t executable : 1;        // for data segment, always 0 here.
                uint32_t descriptor_type : 1;
                uint32_t dpl : 2;
                uint32_t present : 1;
                uint32_t reserved0 : 4;
                uint32_t used_by_system_software : 1;
                uint32_t reserved1 : 1;
                uint32_t default_operation_size : 1;
                uint32_t granularity : 1;
                uint32_t unusable : 1;
                uint32_t reserved2 : 15;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_FS_ACCESS_RIGHTS);

    template<>
    struct vmcsf_t<VMCSF_GUEST_GS_ACCESS_RIGHTS> {
        union {
            uint32_t storage;
            struct {
                uint32_t accessed : 1;
                uint32_t writable : 1;
                uint32_t direction : 1;         // 0 = grows up, 1 = grows down
                uint32_t executable : 1;        // for data segment, always 0 here.
                uint32_t descriptor_type : 1;
                uint32_t dpl : 2;
                uint32_t present : 1;
                uint32_t reserved0 : 4;
                uint32_t used_by_system_software : 1;
                uint32_t reserved1 : 1;
                uint32_t default_operation_size : 1;
                uint32_t granularity : 1;
                uint32_t unusable : 1;
                uint32_t reserved2 : 15;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_GS_ACCESS_RIGHTS);

    template<>
    struct vmcsf_t<VMCSF_GUEST_LDTR_ACCESS_RIGHTS> {
        union {
            uint32_t storage;
            struct {
                uint32_t system_type : 4;
                uint32_t descriptor_type : 1;   // 0 = system, 1 = code or data; always 0 here.
                uint32_t dpl : 2;
                uint32_t present : 1;
                uint32_t reserved0 : 4;
                uint32_t used_by_system_software : 1;
                uint32_t reserved1 : 1;
                uint32_t default_operation_size : 1;
                uint32_t granularity : 1;
                uint32_t unusable : 1;
                uint32_t reserved2 : 15;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_LDTR_ACCESS_RIGHTS);

    template<>
    struct vmcsf_t<VMCSF_GUEST_TR_ACCESS_RIGHTS> {
        union {
            uint32_t storage;
            struct {
                uint32_t segment_type : 4;
                uint32_t descriptor_type : 1;
                uint32_t dpl : 2;
                uint32_t present : 1;
                uint32_t reserved0 : 4;
                uint32_t used_by_system_software : 1;
                uint32_t reserved1 : 1;
                uint32_t default_operation_size : 1;
                uint32_t granularity : 1;
                uint32_t unusable : 1;
                uint32_t reserved2 : 15;
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_GUEST_TR_ACCESS_RIGHTS);

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 27 VM Exits
    //    |-> 27.2 Recording VM-Exit Information and Updating VM-Entry Control Fields
    //      |-> 27.2.1 Basic VM-Exit Information
    //        |-> Table 27-1. Exit Qualification for Debug Exceptions
    template<>
    struct vmcsf_t<VMCSF_INFO_EXIT_QUALIFICATION> {
        union {
            uintptr_t storage;
            union {
                struct {
                    uintptr_t B0 : 1;
                    uintptr_t B1 : 1;
                    uintptr_t B2 : 1;
                    uintptr_t B3 : 1;
                    uintptr_t reserved0 : 9;
                    uintptr_t BD : 1;
                    uintptr_t BS : 1;
                    uintptr_t reserved1 : 1;
                    uintptr_t RTM : 1;
#if defined(_M_X64)
                    uintptr_t reserved2 : 47;
#else
                    uintptr_t reserved2 : 15;
#endif
                } debug_exceptions;
                struct {
                    uintptr_t linear_address;
                } invlpg;
                struct {
                    uintptr_t cr_number : 4;
                    uintptr_t access_type : 2;  // 0 = MOV to CR, 1 = MOV from CR, 2 = CLTS, 3 = LMSW
                    uintptr_t lmsw_operand_type : 1;    // 0 = register, 1 = memory
                    uintptr_t reserved0 : 1;
                    uintptr_t related_gpr : 4;  // 0 = RAX, 1 = RCX, 2 = RDX, 3 = RBX, 4 = RSP, 5 = RBP, 6 = RSI, 7 = RDI, 8-15 = R8-R15
                    uintptr_t reserved1 : 4;
                    uintptr_t lmsw_source_data : 16;
#if defined(_M_X64)
                    uintptr_t reserved2 : 32;
#endif
                } cr_access;
                // todo
            } semantics;
        };
    };

    SIREN_VMCSF_SIZE_GUARD(VMCSF_INFO_EXIT_QUALIFICATION);

#undef SIREN_VMCSF_SIZE_GUARD

    struct vmx_result_t {
        uint8_t value;

        [[nodiscard]]
        constexpr bool is_failure() const noexcept { return value != 0; }

        [[nodiscard]]
        constexpr bool is_success() const noexcept { return value == 0; }

        [[nodiscard]]
        static constexpr vmx_result_t success() noexcept { return { 0 }; }

        [[nodiscard]]
        static constexpr vmx_result_t failure_with_reason() noexcept { return { 1 }; }

        [[nodiscard]]
        static constexpr vmx_result_t failure_without_reason() noexcept { return { 2 }; }
    };

    static_assert(std::is_aggregate_v<vmx_result_t>);

    [[nodiscard]]
    inline vmx_result_t vmx_on(paddr_t vmxon_region_address) noexcept {
        return { __vmx_on(&vmxon_region_address) };
    }

    [[nodiscard]]
    inline vmx_result_t vmx_clear(paddr_t vmcs_region_address) noexcept {
        return { __vmx_vmclear(&vmcs_region_address) };
    }

    [[nodiscard]]
    inline vmx_result_t vmx_ptrld(paddr_t vmcs_region_address) noexcept {
        return { __vmx_vmptrld(&vmcs_region_address) };
    }

    [[nodiscard]]
    inline paddr_t vmx_ptrst() noexcept {
        paddr_t vmcs_region_address;
        __vmx_vmptrst(&vmcs_region_address);
        return vmcs_region_address;
    }

    template<uint32_t V>
    [[nodiscard]]
    vmx_result_t vmx_read(vmcsf_t<V>& field) noexcept {
        size_t raw_value;
        vmx_result_t vmx_result = { __vmx_vmread(V, &raw_value) };
        if (vmx_result.is_success()) {
            field.storage = static_cast<vmcsf_storage_t<V>>(raw_value);
        }
        return vmx_result;
    }

    template<uint32_t V>
    [[nodiscard]]
    vmx_result_t vmx_write(const vmcsf_t<V>& field) noexcept {
        return { __vmx_vmwrite(V, field.storage) };
    }

    [[nodiscard]]
    inline vmx_result_t vmx_launch() noexcept {
        return { __vmx_vmlaunch() };
    }

    [[nodiscard]]
    inline vmx_result_t vmx_resume() noexcept {
        return { __vmx_vmresume() };
    }

    inline void vmx_off() noexcept {
        __vmx_off();
    }

    [[nodiscard]]
    vmx_result_t vmx_invept() noexcept;

    [[nodiscard]]
    vmx_result_t vmx_invept(paddr_t eptp) noexcept;
}
