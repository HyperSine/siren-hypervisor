#pragma once
#include "../x86/model_specific_registers.hpp"

namespace siren::microsoft_hv {
    // MSR used to identify the guest OS. 
    constexpr uint32_t HV_X64_MSR_GUEST_OS_ID = 0x40000000;

    // MSR used to setup pages used to communicate with the hypervisor. 
    constexpr uint32_t HV_X64_MSR_HYPERCALL = 0x40000001;

    // MSR used to provide vcpu index 
    constexpr uint32_t HV_X64_MSR_VP_INDEX = 0x40000002;

    // MSR used to reset the guest OS. 
    constexpr uint32_t HV_X64_MSR_RESET = 0x40000003;

    // MSR used to provide vcpu runtime in 100ns units 
    constexpr uint32_t HV_X64_MSR_VP_RUNTIME = 0x40000010;

    // MSR used to read the per-partition time reference counter 
    constexpr uint32_t HV_X64_MSR_TIME_REF_COUNT = 0x40000020;

    // A partition's reference time stamp counter (TSC) page 
    constexpr uint32_t HV_X64_MSR_REFERENCE_TSC = 0x40000021;

    // MSR used to retrieve the TSC frequency 
    constexpr uint32_t HV_X64_MSR_TSC_FREQUENCY = 0x40000022;

    // MSR used to retrieve the local APIC timer frequency 
    constexpr uint32_t HV_X64_MSR_APIC_FREQUENCY = 0x40000023;

    // Configures non-privileged instruction execution prevention
    constexpr uint32_t HV_X64_MSR_NPIEP_CONFIG = 0x40000040;

    // Define the virtual APIC registers 
    constexpr uint32_t HV_X64_MSR_EOI = 0x40000070;
    constexpr uint32_t HV_X64_MSR_ICR = 0x40000071;
    constexpr uint32_t HV_X64_MSR_TPR = 0x40000072;

    constexpr uint32_t HV_X64_MSR_VP_ASSIST_PAGE = 0x40000073;

    // Define synthetic interrupt controller model specific registers. 
    constexpr uint32_t HV_X64_MSR_SCONTROL = 0x40000080;
    constexpr uint32_t HV_X64_MSR_SVERSION = 0x40000081;
    constexpr uint32_t HV_X64_MSR_SIEFP = 0x40000082;
    constexpr uint32_t HV_X64_MSR_SIMP = 0x40000083;
    constexpr uint32_t HV_X64_MSR_EOM = 0x40000084;
    constexpr uint32_t HV_X64_MSR_SINT0 = 0x40000090;
    constexpr uint32_t HV_X64_MSR_SINT1 = 0x40000091;
    constexpr uint32_t HV_X64_MSR_SINT2 = 0x40000092;
    constexpr uint32_t HV_X64_MSR_SINT3 = 0x40000093;
    constexpr uint32_t HV_X64_MSR_SINT4 = 0x40000094;
    constexpr uint32_t HV_X64_MSR_SINT5 = 0x40000095;
    constexpr uint32_t HV_X64_MSR_SINT6 = 0x40000096;
    constexpr uint32_t HV_X64_MSR_SINT7 = 0x40000097;
    constexpr uint32_t HV_X64_MSR_SINT8 = 0x40000098;
    constexpr uint32_t HV_X64_MSR_SINT9 = 0x40000099;
    constexpr uint32_t HV_X64_MSR_SINT10 = 0x4000009A;
    constexpr uint32_t HV_X64_MSR_SINT11 = 0x4000009B;
    constexpr uint32_t HV_X64_MSR_SINT12 = 0x4000009C;
    constexpr uint32_t HV_X64_MSR_SINT13 = 0x4000009D;
    constexpr uint32_t HV_X64_MSR_SINT14 = 0x4000009E;
    constexpr uint32_t HV_X64_MSR_SINT15 = 0x4000009F;

    /*
     * Synthetic Timer MSRs. Four timers per vcpu.
     */
    constexpr uint32_t HV_X64_MSR_STIMER0_CONFIG = 0x400000B0;
    constexpr uint32_t HV_X64_MSR_STIMER0_COUNT = 0x400000B1;
    constexpr uint32_t HV_X64_MSR_STIMER1_CONFIG = 0x400000B2;
    constexpr uint32_t HV_X64_MSR_STIMER1_COUNT = 0x400000B3;
    constexpr uint32_t HV_X64_MSR_STIMER2_CONFIG = 0x400000B4;
    constexpr uint32_t HV_X64_MSR_STIMER2_COUNT = 0x400000B5;
    constexpr uint32_t HV_X64_MSR_STIMER3_CONFIG = 0x400000B6;
    constexpr uint32_t HV_X64_MSR_STIMER3_COUNT = 0x400000B7;

    // Hyper-V guest idle MSR 
    constexpr uint32_t HV_X64_MSR_GUEST_IDLE = 0x400000F0;

    // Hyper-V guest crash notification MSR's 
    constexpr uint32_t HV_X64_MSR_CRASH_P0 = 0x40000100;
    constexpr uint32_t HV_X64_MSR_CRASH_P1 = 0x40000101;
    constexpr uint32_t HV_X64_MSR_CRASH_P2 = 0x40000102;
    constexpr uint32_t HV_X64_MSR_CRASH_P3 = 0x40000103;
    constexpr uint32_t HV_X64_MSR_CRASH_P4 = 0x40000104;
    constexpr uint32_t HV_X64_MSR_CRASH_CTL = 0x40000105;

    // TSC emulation after migration 
    constexpr uint32_t HV_X64_MSR_REENLIGHTENMENT_CONTROL = 0x40000106;
    constexpr uint32_t HV_X64_MSR_TSC_EMULATION_CONTROL = 0x40000107;
    constexpr uint32_t HV_X64_MSR_TSC_EMULATION_STATUS = 0x40000108;

    // Configuration register for the time-unhalted timer.
    constexpr uint32_t HV_X64_MSR_STIME_UNHALTED_TIMER_CONFIG = 0x40000114;

    // Period for the time-unhalted timer.
    constexpr uint32_t HV_X64_MSR_STIME_UNHALTED_TIMER_COUNT = 0x40000115;

    constexpr uint32_t HV_X64_MSR_NESTED_VP_INDEX = 0x40001002;
    constexpr uint32_t HV_X64_MSR_NESTED_SCONTROL = 0x40001080;
    constexpr uint32_t HV_X64_MSR_NESTED_SVERSION = 0x40001081;
    constexpr uint32_t HV_X64_MSR_NESTED_SIEFP = 0x40001082;
    constexpr uint32_t HV_X64_MSR_NESTED_SIMP = 0x40001083;
    constexpr uint32_t HV_X64_MSR_NESTED_EOM = 0x40001084;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT0 = 0x40001090;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT1 = 0x40001091;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT2 = 0x40001092;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT3 = 0x40001093;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT4 = 0x40001094;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT5 = 0x40001095;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT6 = 0x40001096;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT7 = 0x40001097;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT8 = 0x40001098;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT9 = 0x40001099;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT10 = 0x4000109A;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT11 = 0x4000109B;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT12 = 0x4000109C;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT13 = 0x4000109D;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT14 = 0x4000109E;
    constexpr uint32_t HV_X64_MSR_NESTED_SINT15 = 0x4000109F;
}

namespace siren::x86 {
    template<>
    struct msr_t<microsoft_hv::HV_X64_MSR_GUEST_OS_ID> {
        union {
            uint64_t storage;
            struct {
                uint64_t build_number : 16;
                uint64_t service_version : 8;
                uint64_t minor_version : 8;
                uint64_t major_version : 8;
                uint64_t os_id : 8;
                uint64_t vendor_id : 15;
                uint64_t os_type : 1;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<microsoft_hv::HV_X64_MSR_GUEST_OS_ID>) == sizeof(uint64_t));
    static_assert(sizeof(msr_t<microsoft_hv::HV_X64_MSR_GUEST_OS_ID>::storage) == sizeof(msr_t<microsoft_hv::HV_X64_MSR_GUEST_OS_ID>::semantics));

    template<>
    struct msr_t<microsoft_hv::HV_X64_MSR_HYPERCALL> {
        union {
            uint64_t storage;
            struct {
                uint64_t enable : 1;
                uint64_t locked : 1;
                uint64_t reserved : 10;
                uint64_t hypercall_pfn : 52;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<microsoft_hv::HV_X64_MSR_HYPERCALL>) == sizeof(uint64_t));
    static_assert(sizeof(msr_t<microsoft_hv::HV_X64_MSR_HYPERCALL>::storage) == sizeof(msr_t<microsoft_hv::HV_X64_MSR_HYPERCALL>::semantics));

    template<>
    struct msr_t<microsoft_hv::HV_X64_MSR_VP_ASSIST_PAGE> {
        union {
            uint64_t storage;
            struct {
                uint64_t enable : 1;
                uint64_t reserved : 11;
                uint64_t physical_address : 52;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<microsoft_hv::HV_X64_MSR_VP_ASSIST_PAGE>) == sizeof(uint64_t));
    static_assert(sizeof(msr_t<microsoft_hv::HV_X64_MSR_VP_ASSIST_PAGE>::storage) == sizeof(msr_t<microsoft_hv::HV_X64_MSR_VP_ASSIST_PAGE>::semantics));
}
