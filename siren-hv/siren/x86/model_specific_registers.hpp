#pragma once
#include "../siren_global.hpp"

namespace siren::x86 {
    using msr_address_t = uint32_t;

    using msr_storage_t = uint64_t;

    // See Section 2.23, "MSRs in Pentium Processors."
    constexpr msr_address_t IA32_P5_MC_ADDR = 0x00000000;

    // See Section 2.23, "MSRs in Pentium Processors."
    constexpr msr_address_t IA32_P5_MC_TYPE = 0x00000001;

    // See Section 8.10.5, "Monitor/Mwait Address Range Determination."
    constexpr msr_address_t IA32_MONITOR_FILTER_SIZE = 0x00000006;

    // See Section 17.17, "Time-Stamp Counter."
    constexpr msr_address_t IA32_TIME_STAMP_COUNTER = 0x00000010;

    // Platform ID (R/O) The operating system can use this MSR to determine "slot" information for the processor and the proper microcode update to load.
    constexpr msr_address_t IA32_PLATFORM_ID = 0x00000017;

    // This register holds the APIC base address, permitting the relocation of the APIC memory map. See Section 10.4.4, "Local APIC Status and Location" and Section 10.4.5, "Relocating the Local APIC Registers".
    constexpr msr_address_t IA32_APIC_BASE = 0x0000001B;

    // Control Features in Intel 64 Processor (R/W) If any one enumeration
    constexpr msr_address_t IA32_FEATURE_CONTROL = 0x0000003A;

    // Per Logical Processor TSC Adjust (R/Write to clear)
    constexpr msr_address_t IA32_TSC_ADJUST = 0x0000003B;

    // Speculation Control (R/W) The MSR bits are defined as logical processor scope. On some core implementations, the bits may impact sibling logical processors on the same core. This MSR has a value of 0 after reset and is unaffected by INIT# or SIPI#.
    constexpr msr_address_t IA32_SPEC_CTRL = 0x00000048;

    // Prediction Command (WO) Gives software a way to issue commands that affect the state of predictors.
    constexpr msr_address_t IA32_PRED_CMD = 0x00000049;

    // Protected Processor Inventory Number Enable Control (R/W)
    constexpr msr_address_t IA32_PPIN_CTL = 0x0000004E;

    // Protected Processor Inventory Number (R/O)
    constexpr msr_address_t IA32_PPIN = 0x0000004F;

    // BIOS Update Trigger (W) Executing a WRMSR instruction to this MSR causes a microcode update to be loaded into the processor. See Section 9.11.6, "Microcode Update Loader." A processor may prevent writing to this MSR when loading guest states on VM entries or saving guest states on VM exits.
    constexpr msr_address_t IA32_BIOS_UPDT_TRIG = 0x00000079;

    // BIOS Update Signature (R/W) Returns the microcode update signature following the execution of CPUID.01H. A processor may prevent writing to this MSR when loading guest states on VM entries or saving guest states on VM exits.
    constexpr msr_address_t IA32_BIOS_SIGN_ID = 0x0000008B;

    // IA32_SGXLEPUBKEYHASH[63:0] (R/W) Bits 63:0 of the SHA256 digest of the SIGSTRUCT.MODULUS for SGX Launch Enclave. On reset, the default value is the digest of Intel's signing key.
    constexpr msr_address_t IA32_SGXLEPUBKEYHASH0 = 0x0000008C;

    // IA32_SGXLEPUBKEYHASH[127:64] (R/W) Bits 127:64 of the SHA256 digest of the SIGSTRUCT.MODULUS for SGX Launch Enclave. On reset, the default value is the digest of Intel's signing key.
    constexpr msr_address_t IA32_SGXLEPUBKEYHASH1 = 0x0000008D;

    // IA32_SGXLEPUBKEYHASH[191:128] (R/W) Bits 191:128 of the SHA256 digest of the SIGSTRUCT.MODULUS for SGX Launch Enclave. On reset, the default value is the digest of Intel's signing key.
    constexpr msr_address_t IA32_SGXLEPUBKEYHASH2 = 0x0000008E;

    // IA32_SGXLEPUBKEYHASH[255:192] (R/W) Bits 255:192 of the SHA256 digest of the SIGSTRUCT.MODULUS for SGX Launch Enclave. On reset, the default value is the digest of Intel's signing key.
    constexpr msr_address_t IA32_SGXLEPUBKEYHASH3 = 0x0000008F;

    // SMM Monitor Configuration (R/W)
    constexpr msr_address_t IA32_SMM_MONITOR_CTL = 0x0000009B;

    // Base address of the logical processor's SMRAM image (R/O, SMM only).
    constexpr msr_address_t IA32_SMBASE = 0x0000009E;

    // Power Filtering Control (R/W) This MSR has a value of 0 after reset and is unaffected by INIT# or SIPI#.
    constexpr msr_address_t IA32_MISC_PACKAGE_CTLS = 0x000000BC;

    // General Performance Counter 0 (R/W)
    constexpr msr_address_t IA32_PMC0 = 0x000000C1;

    // General Performance Counter 1 (R/W)
    constexpr msr_address_t IA32_PMC1 = 0x000000C2;

    // General Performance Counter 2 (R/W)
    constexpr msr_address_t IA32_PMC2 = 0x000000C3;

    // General Performance Counter 3 (R/W)
    constexpr msr_address_t IA32_PMC3 = 0x000000C4;

    // General Performance Counter 4 (R/W)
    constexpr msr_address_t IA32_PMC4 = 0x000000C5;

    // General Performance Counter 5 (R/W)
    constexpr msr_address_t IA32_PMC5 = 0x000000C6;

    // General Performance Counter 6 (R/W)
    constexpr msr_address_t IA32_PMC6 = 0x000000C7;

    // General Performance Counter 7 (R/W)
    constexpr msr_address_t IA32_PMC7 = 0x000000C8;

    // IA32 Core Capabilities Register
    constexpr msr_address_t IA32_CORE_CAPABILITIES = 0x000000CF;

    // UMWAIT Control (R/W)
    constexpr msr_address_t IA32_UMWAIT_CONTROL = 0x000000E1;

    // TSC Frequency Clock Counter (R/Write to clear)
    constexpr msr_address_t IA32_MPERF = 0x000000E7;

    // Actual Performance Clock Counter (R/Write to clear)
    constexpr msr_address_t IA32_APERF = 0x000000E8;

    // MTRR Capability (R/O) See Section 11.11.2.1, "IA32_MTRR_DEF_TYPE MSR."
    constexpr msr_address_t IA32_MTRRCAP = 0x000000FE;

    // Enumeration of Architectural Features (R/O)
    constexpr msr_address_t IA32_ARCH_CAPABILITIES = 0x0000010A;

    // Flush Command (WO) Gives software a way to invalidate structures with finer granularity than other architectural methods.
    constexpr msr_address_t IA32_FLUSH_CMD = 0x0000010B;

    // IA32_TSX_CTRL
    constexpr msr_address_t IA32_TSX_CTRL = 0x00000122;

    // SYSENTER_CS_MSR (R/W)
    constexpr msr_address_t IA32_SYSENTER_CS = 0x00000174;

    // SYSENTER_ESP_MSR (R/W)
    constexpr msr_address_t IA32_SYSENTER_ESP = 0x00000175;

    // SYSENTER_EIP_MSR (R/W)
    constexpr msr_address_t IA32_SYSENTER_EIP = 0x00000176;

    // Global Machine Check Capability (R/O)
    constexpr msr_address_t IA32_MCG_CAP = 0x00000179;

    // Global Machine Check Status (R/W0)
    constexpr msr_address_t IA32_MCG_STATUS = 0x0000017A;

    // Global Machine Check Control (R/W)
    constexpr msr_address_t IA32_MCG_CTL = 0x0000017B;

    // Performance Event Select Register 0 (R/W) If CPUID.0AH: EAX[15:8] >
    constexpr msr_address_t IA32_PERFEVTSEL0 = 0x00000186;

    // Performance Event Select Register 1 (R/W) If CPUID.0AH: EAX[15:8] >
    constexpr msr_address_t IA32_PERFEVTSEL1 = 0x00000187;

    // Performance Event Select Register 2 (R/W) If CPUID.0AH: EAX[15:8] >
    constexpr msr_address_t IA32_PERFEVTSEL2 = 0x00000188;

    // Performance Event Select Register 3 (R/W) If CPUID.0AH: EAX[15:8] >
    constexpr msr_address_t IA32_PERFEVTSEL3 = 0x00000189;

    // Performance Event Select Register 4 (R/W) If CPUID.0AH: EAX[15:8] >
    constexpr msr_address_t IA32_PERFEVTSEL4 = 0x0000018A;

    // Performance Event Select Register 5 (R/W) If CPUID.0AH: EAX[15:8] >
    constexpr msr_address_t IA32_PERFEVTSEL5 = 0x0000018B;

    // Performance Event Select Register 6 (R/W) If CPUID.0AH: EAX[15:8] >
    constexpr msr_address_t IA32_PERFEVTSEL6 = 0x0000018C;

    // Performance Event Select Register 7 (R/W) If CPUID.0AH: EAX[15:8] >
    constexpr msr_address_t IA32_PERFEVTSEL7 = 0x0000018D;

    // Overclocking Status (R/O) IA32_ARCH_CAPABILITIES[bit 23] enumerates support for this MSR.
    constexpr msr_address_t IA32_OVERCLOCKING_STATUS = 0x00000195;

    // Current Performance Status (R/O) See Section 14.1.1, "Software Interface For Initiating Performance State Transitions".
    constexpr msr_address_t IA32_PERF_STATUS = 0x00000198;

    // Performance Control MSR (R/W) Software makes a request for a new Performance state (P-State) by writing this MSR. See Section 14.1.1, "Software Interface For Initiating Performance State Transitions".
    constexpr msr_address_t IA32_PERF_CTL = 0x00000199;

    // Clock Modulation Control (R/W) See Section 14.8.3, "Software Controlled Clock Modulation."
    constexpr msr_address_t IA32_CLOCK_MODULATION = 0x0000019A;

    // Thermal Interrupt Control (R/W) Enables and disables the generation of an interrupt on temperature transitions detected with the processor's thermal sensors and thermal monitor. See Section 14.8.2, "Thermal Monitor."
    constexpr msr_address_t IA32_THERM_INTERRUPT = 0x0000019B;

    // Thermal Status Information (R/O) Contains status information about the processor's thermal sensor and automatic thermal monitoring facilities. See Section 14.8.2, "Thermal Monitor".
    constexpr msr_address_t IA32_THERM_STATUS = 0x0000019C;

    // Enable Misc. Processor Features (R/W) Allows a variety of processor functions to be enabled and disabled.
    constexpr msr_address_t IA32_MISC_ENABLE = 0x000001A0;

    // Performance Energy Bias Hint (R/W)
    constexpr msr_address_t IA32_ENERGY_PERF_BIAS = 0x000001B0;

    // Package Thermal Status Information (R/O) Contains status information about the package's thermal sensor. See Section 14.9, "Package Level Thermal Management."
    constexpr msr_address_t IA32_PACKAGE_THERM_STATUS = 0x000001B1;

    // Pkg Thermal Interrupt Control (R/W) Enables and disables the generation of an interrupt on temperature transitions detected with the package's thermal sensor. See Section 14.9, "Package Level Thermal Management."
    constexpr msr_address_t IA32_PACKAGE_THERM_INTERRUPT = 0x000001B2;

    // Trace/Profile Resource Control (R/W)
    constexpr msr_address_t IA32_DEBUGCTL = 0x000001D9;

    // Last Event Record Source IP Register (R/W)
    constexpr msr_address_t IA32_LER_FROM_IP = 0x000001DD;

    // Last Event Record Destination IP Register (R/W)
    constexpr msr_address_t IA32_LER_TO_IP = 0x000001DE;

    // Last Event Record Info Register (R/W)
    constexpr msr_address_t IA32_LER_INFO = 0x000001E0;

    // SMRR Base Address (Writeable only in SMM) Base address of SMM memory range.
    constexpr msr_address_t IA32_SMRR_PHYSBASE = 0x000001F2;

    // SMRR Range Mask (Writeable only in SMM) Range Mask of SMM memory range.
    constexpr msr_address_t IA32_SMRR_PHYSMASK = 0x000001F3;

    // DCA Capability (R)
    constexpr msr_address_t IA32_PLATFORM_DCA_CAP = 0x000001F8;

    // If set, CPU supports Prefetch-Hint type.
    constexpr msr_address_t IA32_CPU_DCA_CAP = 0x000001F9;

    // DCA type 0 Status and Control register.
    constexpr msr_address_t IA32_DCA_0_CAP = 0x000001FA;

    // See Section 11.11.2.3, "Variable Range MTRRs."
    constexpr msr_address_t IA32_MTRR_PHYSBASE0 = 0x00000200;

    // MTRRphysMask0
    constexpr msr_address_t IA32_MTRR_PHYSMASK0 = 0x00000201;

    // MTRRphysBase1
    constexpr msr_address_t IA32_MTRR_PHYSBASE1 = 0x00000202;

    // MTRRphysMask1
    constexpr msr_address_t IA32_MTRR_PHYSMASK1 = 0x00000203;

    // MTRRphysBase2
    constexpr msr_address_t IA32_MTRR_PHYSBASE2 = 0x00000204;

    // MTRRphysMask2
    constexpr msr_address_t IA32_MTRR_PHYSMASK2 = 0x00000205;

    // MTRRphysBase3
    constexpr msr_address_t IA32_MTRR_PHYSBASE3 = 0x00000206;

    // MTRRphysMask3
    constexpr msr_address_t IA32_MTRR_PHYSMASK3 = 0x00000207;

    // MTRRphysBase4
    constexpr msr_address_t IA32_MTRR_PHYSBASE4 = 0x00000208;

    // MTRRphysMask4
    constexpr msr_address_t IA32_MTRR_PHYSMASK4 = 0x00000209;

    // MTRRphysBase5
    constexpr msr_address_t IA32_MTRR_PHYSBASE5 = 0x0000020A;

    // MTRRphysMask5
    constexpr msr_address_t IA32_MTRR_PHYSMASK5 = 0x0000020B;

    // MTRRphysBase6
    constexpr msr_address_t IA32_MTRR_PHYSBASE6 = 0x0000020C;

    // MTRRphysMask6
    constexpr msr_address_t IA32_MTRR_PHYSMASK6 = 0x0000020D;

    // MTRRphysBase7
    constexpr msr_address_t IA32_MTRR_PHYSBASE7 = 0x0000020E;

    // MTRRphysMask7
    constexpr msr_address_t IA32_MTRR_PHYSMASK7 = 0x0000020F;

    // MTRRphysBase8
    constexpr msr_address_t IA32_MTRR_PHYSBASE8 = 0x00000210;

    // MTRRphysMask8
    constexpr msr_address_t IA32_MTRR_PHYSMASK8 = 0x00000211;

    // MTRRphysBase9
    constexpr msr_address_t IA32_MTRR_PHYSBASE9 = 0x00000212;

    // MTRRphysMask9
    constexpr msr_address_t IA32_MTRR_PHYSMASK9 = 0x00000213;

    // MTRRfix64K_00000
    constexpr msr_address_t IA32_MTRR_FIX64K_00000 = 0x00000250;

    // MTRRfix16K_80000
    constexpr msr_address_t IA32_MTRR_FIX16K_80000 = 0x00000258;

    // MTRRfix16K_A0000
    constexpr msr_address_t IA32_MTRR_FIX16K_A0000 = 0x00000259;

    // See Section 11.11.2.2, "Fixed Range MTRRs."
    constexpr msr_address_t IA32_MTRR_FIX4K_C0000 = 0x00000268;

    // MTRRfix4K_C8000
    constexpr msr_address_t IA32_MTRR_FIX4K_C8000 = 0x00000269;

    // MTRRfix4K_D0000
    constexpr msr_address_t IA32_MTRR_FIX4K_D0000 = 0x0000026A;

    // MTRRfix4K_D8000
    constexpr msr_address_t IA32_MTRR_FIX4K_D8000 = 0x0000026B;

    // MTRRfix4K_E0000
    constexpr msr_address_t IA32_MTRR_FIX4K_E0000 = 0x0000026C;

    // MTRRfix4K_E8000
    constexpr msr_address_t IA32_MTRR_FIX4K_E8000 = 0x0000026D;

    // MTRRfix4K_F0000
    constexpr msr_address_t IA32_MTRR_FIX4K_F0000 = 0x0000026E;

    // MTRRfix4K_F8000
    constexpr msr_address_t IA32_MTRR_FIX4K_F8000 = 0x0000026F;

    // IA32_PAT (R/W)
    constexpr msr_address_t IA32_PAT = 0x00000277;

    // MSR to enable/disable CMCI capability for bank 0. (R/W) See Section 15.3.2.5, "IA32_MCi_CTL2 MSRs".
    constexpr msr_address_t IA32_MC0_CTL2 = 0x00000280;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC1_CTL2 = 0x00000281;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC2_CTL2 = 0x00000282;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC3_CTL2 = 0x00000283;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC4_CTL2 = 0x00000284;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC5_CTL2 = 0x00000285;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC6_CTL2 = 0x00000286;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC7_CTL2 = 0x00000287;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC8_CTL2 = 0x00000288;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC9_CTL2 = 0x00000289;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC10_CTL2 = 0x0000028A;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC11_CTL2 = 0x0000028B;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC12_CTL2 = 0x0000028C;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC13_CTL2 = 0x0000028D;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC14_CTL2 = 0x0000028E;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC15_CTL2 = 0x0000028F;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC16_CTL2 = 0x00000290;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC17_CTL2 = 0x00000291;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC18_CTL2 = 0x00000292;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC19_CTL2 = 0x00000293;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC20_CTL2 = 0x00000294;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC21_CTL2 = 0x00000295;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC22_CTL2 = 0x00000296;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC23_CTL2 = 0x00000297;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC24_CTL2 = 0x00000298;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC25_CTL2 = 0x00000299;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC26_CTL2 = 0x0000029A;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC27_CTL2 = 0x0000029B;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC28_CTL2 = 0x0000029C;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC29_CTL2 = 0x0000029D;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC30_CTL2 = 0x0000029E;

    // (R/W) Same fields as IA32_MC0_CTL2.
    constexpr msr_address_t IA32_MC31_CTL2 = 0x0000029F;

    // MTRRdefType (R/W)
    constexpr msr_address_t IA32_MTRR_DEF_TYPE = 0x000002FF;

    // Fixed-Function Performance Counter 0 (R/W): Counts Instr_Retired.Any.
    constexpr msr_address_t IA32_FIXED_CTR0 = 0x00000309;

    // Fixed-Function Performance Counter 1 (R/W): Counts CPU_CLK_Unhalted.Core.
    constexpr msr_address_t IA32_FIXED_CTR1 = 0x0000030A;

    // Fixed-Function Performance Counter 2 (R/W): Counts CPU_CLK_Unhalted.Ref.
    constexpr msr_address_t IA32_FIXED_CTR2 = 0x0000030B;

    // Read Only MSR that enumerates the existence of performance monitoring features. (R/O)
    constexpr msr_address_t IA32_PERF_CAPABILITIES = 0x00000345;

    // Fixed-Function Performance Counter Control (R/W) Counter increments while the results of ANDing respective enable bit in IA32_PERF_GLOBAL_CTRL with the corresponding OS or USR bits in this MSR is true.
    constexpr msr_address_t IA32_FIXED_CTR_CTRL = 0x0000038D;

    // Performance Counter Status (R/O)
    constexpr msr_address_t IA32_PERF_GLOBAL_STATUS = 0x0000038E;

    // Global Performance Counter Control (R/W) Counter increments while the result of ANDing the respective enable bit in this MSR with the corresponding OS or USR bits in the general-purpose or fixed counter control MSR is true.
    constexpr msr_address_t IA32_PERF_GLOBAL_CTRL = 0x0000038F;

    // Global Performance Counter Overflow Control (R/W)
    constexpr msr_address_t IA32_PERF_GLOBAL_OVF_CTRL = 0x00000390;

    // Global Performance Counter Overflow Reset Control (R/W)
    constexpr msr_address_t IA32_PERF_GLOBAL_STATUS_RESET = 0x00000390;

    // Global Performance Counter Overflow Set Control (R/W)
    constexpr msr_address_t IA32_PERF_GLOBAL_STATUS_SET = 0x00000391;

    // Indicator that core perfmon interface is in use. (R/O)
    constexpr msr_address_t IA32_PERF_GLOBAL_INUSE = 0x00000392;

    // PEBS Control (R/W)
    constexpr msr_address_t IA32_PEBS_ENABLE = 0x000003F1;

    // MC0_CTL
    constexpr msr_address_t IA32_MC0_CTL = 0x00000400;

    // MC0_STATUS
    constexpr msr_address_t IA32_MC0_STATUS = 0x00000401;

    // MC0_ADDR
    constexpr msr_address_t IA32_MC0_ADDR = 0x00000402;

    // MC0_MISC
    constexpr msr_address_t IA32_MC0_MISC = 0x00000403;

    // MC1_CTL
    constexpr msr_address_t IA32_MC1_CTL = 0x00000404;

    // MC1_STATUS
    constexpr msr_address_t IA32_MC1_STATUS = 0x00000405;

    // MC1_ADDR
    constexpr msr_address_t IA32_MC1_ADDR = 0x00000406;

    // MC1_MISC
    constexpr msr_address_t IA32_MC1_MISC = 0x00000407;

    // MC2_CTL
    constexpr msr_address_t IA32_MC2_CTL = 0x00000408;

    // MC2_STATUS
    constexpr msr_address_t IA32_MC2_STATUS = 0x00000409;

    // MC2_ADDR
    constexpr msr_address_t IA32_MC2_ADDR = 0x0000040A;

    // MC2_MISC
    constexpr msr_address_t IA32_MC2_MISC = 0x0000040B;

    // MC3_CTL
    constexpr msr_address_t IA32_MC3_CTL = 0x0000040C;

    // MC3_STATUS
    constexpr msr_address_t IA32_MC3_STATUS = 0x0000040D;

    // MC3_ADDR
    constexpr msr_address_t IA32_MC3_ADDR = 0x0000040E;

    // MC3_MISC
    constexpr msr_address_t IA32_MC3_MISC = 0x0000040F;

    // MC4_CTL
    constexpr msr_address_t IA32_MC4_CTL = 0x00000410;

    // MC4_STATUS
    constexpr msr_address_t IA32_MC4_STATUS = 0x00000411;

    // MC4_ADDR
    constexpr msr_address_t IA32_MC4_ADDR = 0x00000412;

    // MC4_MISC
    constexpr msr_address_t IA32_MC4_MISC = 0x00000413;

    // MC5_CTL
    constexpr msr_address_t IA32_MC5_CTL = 0x00000414;

    // MC5_STATUS
    constexpr msr_address_t IA32_MC5_STATUS = 0x00000415;

    // MC5_ADDR
    constexpr msr_address_t IA32_MC5_ADDR = 0x00000416;

    // MC5_MISC
    constexpr msr_address_t IA32_MC5_MISC = 0x00000417;

    // MC6_CTL
    constexpr msr_address_t IA32_MC6_CTL = 0x00000418;

    // MC6_STATUS
    constexpr msr_address_t IA32_MC6_STATUS = 0x00000419;

    // MC6_ADDR
    constexpr msr_address_t IA32_MC6_ADDR = 0x0000041A;

    // MC6_MISC
    constexpr msr_address_t IA32_MC6_MISC = 0x0000041B;

    // MC7_CTL
    constexpr msr_address_t IA32_MC7_CTL = 0x0000041C;

    // MC7_STATUS
    constexpr msr_address_t IA32_MC7_STATUS = 0x0000041D;

    // MC7_ADDR
    constexpr msr_address_t IA32_MC7_ADDR = 0x0000041E;

    // MC7_MISC
    constexpr msr_address_t IA32_MC7_MISC = 0x0000041F;

    // MC8_CTL
    constexpr msr_address_t IA32_MC8_CTL = 0x00000420;

    // MC8_STATUS
    constexpr msr_address_t IA32_MC8_STATUS = 0x00000421;

    // MC8_ADDR
    constexpr msr_address_t IA32_MC8_ADDR = 0x00000422;

    // MC8_MISC
    constexpr msr_address_t IA32_MC8_MISC = 0x00000423;

    // MC9_CTL
    constexpr msr_address_t IA32_MC9_CTL = 0x00000424;

    // MC9_STATUS
    constexpr msr_address_t IA32_MC9_STATUS = 0x00000425;

    // MC9_ADDR
    constexpr msr_address_t IA32_MC9_ADDR = 0x00000426;

    // MC9_MISC
    constexpr msr_address_t IA32_MC9_MISC = 0x00000427;

    // MC10_CTL
    constexpr msr_address_t IA32_MC10_CTL = 0x00000428;

    // MC10_STATUS
    constexpr msr_address_t IA32_MC10_STATUS = 0x00000429;

    // MC10_ADDR
    constexpr msr_address_t IA32_MC10_ADDR = 0x0000042A;

    // MC10_MISC
    constexpr msr_address_t IA32_MC10_MISC = 0x0000042B;

    // MC11_CTL
    constexpr msr_address_t IA32_MC11_CTL = 0x0000042C;

    // MC11_STATUS
    constexpr msr_address_t IA32_MC11_STATUS = 0x0000042D;

    // MC11_ADDR
    constexpr msr_address_t IA32_MC11_ADDR = 0x0000042E;

    // MC11_MISC
    constexpr msr_address_t IA32_MC11_MISC = 0x0000042F;

    // MC12_CTL
    constexpr msr_address_t IA32_MC12_CTL = 0x00000430;

    // MC12_STATUS
    constexpr msr_address_t IA32_MC12_STATUS = 0x00000431;

    // MC12_ADDR
    constexpr msr_address_t IA32_MC12_ADDR = 0x00000432;

    // MC12_MISC
    constexpr msr_address_t IA32_MC12_MISC = 0x00000433;

    // MC13_CTL
    constexpr msr_address_t IA32_MC13_CTL = 0x00000434;

    // MC13_STATUS
    constexpr msr_address_t IA32_MC13_STATUS = 0x00000435;

    // MC13_ADDR
    constexpr msr_address_t IA32_MC13_ADDR = 0x00000436;

    // MC13_MISC
    constexpr msr_address_t IA32_MC13_MISC = 0x00000437;

    // MC14_CTL
    constexpr msr_address_t IA32_MC14_CTL = 0x00000438;

    // MC14_STATUS
    constexpr msr_address_t IA32_MC14_STATUS = 0x00000439;

    // MC14_ADDR
    constexpr msr_address_t IA32_MC14_ADDR = 0x0000043A;

    // MC14_MISC
    constexpr msr_address_t IA32_MC14_MISC = 0x0000043B;

    // MC15_CTL
    constexpr msr_address_t IA32_MC15_CTL = 0x0000043C;

    // MC15_STATUS
    constexpr msr_address_t IA32_MC15_STATUS = 0x0000043D;

    // MC15_ADDR
    constexpr msr_address_t IA32_MC15_ADDR = 0x0000043E;

    // MC15_MISC
    constexpr msr_address_t IA32_MC15_MISC = 0x0000043F;

    // MC16_CTL
    constexpr msr_address_t IA32_MC16_CTL = 0x00000440;

    // MC16_STATUS
    constexpr msr_address_t IA32_MC16_STATUS = 0x00000441;

    // MC16_ADDR
    constexpr msr_address_t IA32_MC16_ADDR = 0x00000442;

    // MC16_MISC
    constexpr msr_address_t IA32_MC16_MISC = 0x00000443;

    // MC17_CTL
    constexpr msr_address_t IA32_MC17_CTL = 0x00000444;

    // MC17_STATUS
    constexpr msr_address_t IA32_MC17_STATUS = 0x00000445;

    // MC17_ADDR
    constexpr msr_address_t IA32_MC17_ADDR = 0x00000446;

    // MC17_MISC
    constexpr msr_address_t IA32_MC17_MISC = 0x00000447;

    // MC18_CTL
    constexpr msr_address_t IA32_MC18_CTL = 0x00000448;

    // MC18_STATUS
    constexpr msr_address_t IA32_MC18_STATUS = 0x00000449;

    // MC18_ADDR
    constexpr msr_address_t IA32_MC18_ADDR = 0x0000044A;

    // MC18_MISC
    constexpr msr_address_t IA32_MC18_MISC = 0x0000044B;

    // MC19_CTL
    constexpr msr_address_t IA32_MC19_CTL = 0x0000044C;

    // MC19_STATUS
    constexpr msr_address_t IA32_MC19_STATUS = 0x0000044D;

    // MC19_ADDR
    constexpr msr_address_t IA32_MC19_ADDR = 0x0000044E;

    // MC19_MISC
    constexpr msr_address_t IA32_MC19_MISC = 0x0000044F;

    // MC20_CTL
    constexpr msr_address_t IA32_MC20_CTL = 0x00000450;

    // MC20_STATUS
    constexpr msr_address_t IA32_MC20_STATUS = 0x00000451;

    // MC20_ADDR
    constexpr msr_address_t IA32_MC20_ADDR = 0x00000452;

    // MC20_MISC
    constexpr msr_address_t IA32_MC20_MISC = 0x00000453;

    // MC21_CTL
    constexpr msr_address_t IA32_MC21_CTL = 0x00000454;

    // MC21_STATUS
    constexpr msr_address_t IA32_MC21_STATUS = 0x00000455;

    // MC21_ADDR
    constexpr msr_address_t IA32_MC21_ADDR = 0x00000456;

    // MC21_MISC
    constexpr msr_address_t IA32_MC21_MISC = 0x00000457;

    // MC22_CTL
    constexpr msr_address_t IA32_MC22_CTL = 0x00000458;

    // MC22_STATUS
    constexpr msr_address_t IA32_MC22_STATUS = 0x00000459;

    // MC22_ADDR
    constexpr msr_address_t IA32_MC22_ADDR = 0x0000045A;

    // MC22_MISC
    constexpr msr_address_t IA32_MC22_MISC = 0x0000045B;

    // MC23_CTL
    constexpr msr_address_t IA32_MC23_CTL = 0x0000045C;

    // MC23_STATUS
    constexpr msr_address_t IA32_MC23_STATUS = 0x0000045D;

    // MC23_ADDR
    constexpr msr_address_t IA32_MC23_ADDR = 0x0000045E;

    // MC23_MISC
    constexpr msr_address_t IA32_MC23_MISC = 0x0000045F;

    // MC24_CTL
    constexpr msr_address_t IA32_MC24_CTL = 0x00000460;

    // MC24_STATUS
    constexpr msr_address_t IA32_MC24_STATUS = 0x00000461;

    // MC24_ADDR
    constexpr msr_address_t IA32_MC24_ADDR = 0x00000462;

    // MC24_MISC
    constexpr msr_address_t IA32_MC24_MISC = 0x00000463;

    // MC25_CTL
    constexpr msr_address_t IA32_MC25_CTL = 0x00000464;

    // MC25_STATUS
    constexpr msr_address_t IA32_MC25_STATUS = 0x00000465;

    // MC25_ADDR
    constexpr msr_address_t IA32_MC25_ADDR = 0x00000466;

    // MC25_MISC
    constexpr msr_address_t IA32_MC25_MISC = 0x00000467;

    // MC26_CTL
    constexpr msr_address_t IA32_MC26_CTL = 0x00000468;

    // MC26_STATUS
    constexpr msr_address_t IA32_MC26_STATUS = 0x00000469;

    // MC26_ADDR
    constexpr msr_address_t IA32_MC26_ADDR = 0x0000046A;

    // MC26_MISC
    constexpr msr_address_t IA32_MC26_MISC = 0x0000046B;

    // MC27_CTL
    constexpr msr_address_t IA32_MC27_CTL = 0x0000046C;

    // MC27_STATUS
    constexpr msr_address_t IA32_MC27_STATUS = 0x0000046D;

    // MC27_ADDR
    constexpr msr_address_t IA32_MC27_ADDR = 0x0000046E;

    // MC27_MISC
    constexpr msr_address_t IA32_MC27_MISC = 0x0000046F;

    // MC28_CTL
    constexpr msr_address_t IA32_MC28_CTL = 0x00000470;

    // MC28_STATUS
    constexpr msr_address_t IA32_MC28_STATUS = 0x00000471;

    // MC28_ADDR
    constexpr msr_address_t IA32_MC28_ADDR = 0x00000472;

    // MC28_MISC
    constexpr msr_address_t IA32_MC28_MISC = 0x00000473;

    // Reporting Register of Basic VMX Capabilities (R/O) See Appendix A.1, "Basic VMX Information."
    constexpr msr_address_t IA32_VMX_BASIC = 0x00000480;

    // Capability Reporting Register of Pin-Based VM-Execution Controls (R/O) See Appendix A.3.1, "Pin-Based VM- Execution Controls."
    constexpr msr_address_t IA32_VMX_PINBASED_CTLS = 0x00000481;

    // Capability Reporting Register of Primary Processor-Based VM-Execution Controls (R/O) See Appendix A.3.2, "Primary Processor- Based VM-Execution Controls."
    constexpr msr_address_t IA32_VMX_PROCBASED_CTLS = 0x00000482;

    // Capability Reporting Register of Primary VM-Exit Controls (R/O) See Appendix A.4.1, "Primary VM-Exit Controls."
    constexpr msr_address_t IA32_VMX_EXIT_CTLS = 0x00000483;

    // Capability Reporting Register of VM-Entry Controls (R/O) See Appendix A.5, "VM-Entry Controls."
    constexpr msr_address_t IA32_VMX_ENTRY_CTLS = 0x00000484;

    // Reporting Register of Miscellaneous VMX Capabilities (R/O) See Appendix A.6, "Miscellaneous Data."
    constexpr msr_address_t IA32_VMX_MISC = 0x00000485;

    // Capability Reporting Register of CR0 Bits Fixed to 0 (R/O) See Appendix A.7, "VMX-Fixed Bits in CR0."
    constexpr msr_address_t IA32_VMX_CR0_FIXED0 = 0x00000486;

    // Capability Reporting Register of CR0 Bits Fixed to 1 (R/O) See Appendix A.7, "VMX-Fixed Bits in CR0."
    constexpr msr_address_t IA32_VMX_CR0_FIXED1 = 0x00000487;

    // Capability Reporting Register of CR4 Bits Fixed to 0 (R/O) See Appendix A.8, "VMX-Fixed Bits in CR4."
    constexpr msr_address_t IA32_VMX_CR4_FIXED0 = 0x00000488;

    // Capability Reporting Register of CR4 Bits Fixed to 1 (R/O) See Appendix A.8, "VMX-Fixed Bits in CR4."
    constexpr msr_address_t IA32_VMX_CR4_FIXED1 = 0x00000489;

    // Capability Reporting Register of VMCS Field Enumeration (R/O) See Appendix A.9, "VMCS Enumeration."
    constexpr msr_address_t IA32_VMX_VMCS_ENUM = 0x0000048A;

    // Capability Reporting Register of Secondary Processor-Based VM-Execution Controls (R/O) See Appendix A.3.3, "Secondary Processor- Based VM-Execution Controls."
    constexpr msr_address_t IA32_VMX_PROCBASED_CTLS2 = 0x0000048B;

    // Capability Reporting Register of EPT and VPID (R/O) See Appendix A.10, "VPID and EPT Capabilities."
    constexpr msr_address_t IA32_VMX_EPT_VPID_CAP = 0x0000048C;

    // Capability Reporting Register of Pin-Based VM-Execution Flex Controls (R/O) See Appendix A.3.1, "Pin-Based VM- Execution Controls."
    constexpr msr_address_t IA32_VMX_TRUE_PINBASED_CTLS = 0x0000048D;

    // Capability Reporting Register of Primary Processor-Based VM-Execution Flex Controls (R/O) See Appendix A.3.2, "Primary Processor- Based VM-Execution Controls."
    constexpr msr_address_t IA32_VMX_TRUE_PROCBASED_CTLS = 0x0000048E;

    // Capability Reporting Register of VM-Exit Flex Controls (R/O) See Appendix A.4, "VM-Exit Controls."
    constexpr msr_address_t IA32_VMX_TRUE_EXIT_CTLS = 0x0000048F;

    // Capability Reporting Register of VM-Entry Flex Controls (R/O) See Appendix A.5, "VM-Entry Controls."
    constexpr msr_address_t IA32_VMX_TRUE_ENTRY_CTLS = 0x00000490;

    // Capability Reporting Register of VM- Function Controls (R/O)
    constexpr msr_address_t IA32_VMX_VMFUNC = 0x00000491;

    // Capability Reporting Register of Tertiary Processor-Based VM-Execution Controls (R/O) See Appendix A.3.4, "Tertiary Processor- Based VM-Execution Controls."
    constexpr msr_address_t IA32_VMX_PROCBASED_CTLS3 = 0x00000492;

    // Capability Reporting Register of Secondary VM-Exit Controls (R/O) See Appendix A.4.2, "Secondary VM-Exit Controls."
    constexpr msr_address_t IA32_VMX_EXIT_CTLS2 = 0x00000493;

    // Full Width Writable IA32_PMC0 Alias (R/W)
    constexpr msr_address_t IA32_A_PMC0 = 0x000004C1;

    // Full Width Writable IA32_PMC1 Alias (R/W)
    constexpr msr_address_t IA32_A_PMC1 = 0x000004C2;

    // Full Width Writable IA32_PMC2 Alias (R/W)
    constexpr msr_address_t IA32_A_PMC2 = 0x000004C3;

    // Full Width Writable IA32_PMC3 Alias (R/W)
    constexpr msr_address_t IA32_A_PMC3 = 0x000004C4;

    // Full Width Writable IA32_PMC4 Alias (R/W)
    constexpr msr_address_t IA32_A_PMC4 = 0x000004C5;

    // Full Width Writable IA32_PMC5 Alias (R/W)
    constexpr msr_address_t IA32_A_PMC5 = 0x000004C6;

    // Full Width Writable IA32_PMC6 Alias (R/W)
    constexpr msr_address_t IA32_A_PMC6 = 0x000004C7;

    // Full Width Writable IA32_PMC7 Alias (R/W)
    constexpr msr_address_t IA32_A_PMC7 = 0x000004C8;

    // Allows software to signal some MCEs to only a single logical processor in the system. (R/W) See Section 15.3.1.4, "IA32_MCG_EXT_CTL MSR".
    constexpr msr_address_t IA32_MCG_EXT_CTL = 0x000004D0;

    // Status and SVN Threshold of SGX Support for ACM (R/O).
    constexpr msr_address_t IA32_SGX_SVN_STATUS = 0x00000500;

    // Trace Output Base Register (R/W)
    constexpr msr_address_t IA32_RTIT_OUTPUT_BASE = 0x00000560;

    // Trace Output Mask Pointers Register (R/W) If ((CPUID.(EAX=07H,
    constexpr msr_address_t IA32_RTIT_OUTPUT_MASK_PTRS = 0x00000561;

    // Trace Control Register (R/W)
    constexpr msr_address_t IA32_RTIT_CTL = 0x00000570;

    // Tracing Status Register (R/W)
    constexpr msr_address_t IA32_RTIT_STATUS = 0x00000571;

    // Trace Filter CR3 Match Register (R/W)
    constexpr msr_address_t IA32_RTIT_CR3_MATCH = 0x00000572;

    // Region 0 Start Address (R/W)
    constexpr msr_address_t IA32_RTIT_ADDR0_A = 0x00000580;

    // Region 0 End Address (R/W)
    constexpr msr_address_t IA32_RTIT_ADDR0_B = 0x00000581;

    // Region 1 Start Address (R/W)
    constexpr msr_address_t IA32_RTIT_ADDR1_A = 0x00000582;

    // Region 1 End Address (R/W)
    constexpr msr_address_t IA32_RTIT_ADDR1_B = 0x00000583;

    // Region 2 Start Address (R/W)
    constexpr msr_address_t IA32_RTIT_ADDR2_A = 0x00000584;

    // Region 2 End Address (R/W)
    constexpr msr_address_t IA32_RTIT_ADDR2_B = 0x00000585;

    // Region 3 Start Address (R/W)
    constexpr msr_address_t IA32_RTIT_ADDR3_A = 0x00000586;

    // Region 3 End Address (R/W)
    constexpr msr_address_t IA32_RTIT_ADDR3_B = 0x00000587;

    // DS Save Area (R/W) Points to the linear address of the first byte of the DS buffer management area, which is used to manage the BTS and PEBS buffers. See Section 19.6.3.4, "Debug Store (DS) Mechanism."
    constexpr msr_address_t IA32_DS_AREA = 0x00000600;

    // Configure User Mode CET (R/W)
    constexpr msr_address_t IA32_U_CET = 0x000006A0;

    // Configure Supervisor Mode CET (R/W)
    constexpr msr_address_t IA32_S_CET = 0x000006A2;

    // Linear address to be loaded into SSP on transition to privilege level 0. (R/W) If the processor does not support Intel 64 architecture, these fields have only 32 bits; bits 63:32 of the MSRs are reserved. On processors that support Intel 64 architecture this value cannot represent a non-canonical address. In protected mode, only 31:0 are loaded. The linear address written must be aligned to 8 bytes and bits 2:0 must be 0 (hardware requires bits 1:0 to be 0).
    constexpr msr_address_t IA32_PL0_SSP = 0x000006A4;

    // Linear address to be loaded into SSP on transition to privilege level 1. (R/W) If the processor does not support Intel 64 architecture, these fields have only 32 bits; bits 63:32 of the MSRs are reserved. On processors that support Intel 64 architecture this value cannot represent a non-canonical address. In protected mode, only 31:0 are loaded. The linear address written must be aligned to 8 bytes and bits 2:0 must be 0 (hardware requires bits 1:0 to be 0).
    constexpr msr_address_t IA32_PL1_SSP = 0x000006A5;

    // Linear address to be loaded into SSP on transition to privilege level 2. (R/W) If the processor does not support Intel 64 architecture, these fields have only 32 bits; bits 63:32 of the MSRs are reserved. On processors that support Intel 64 architecture this value cannot represent a non-canonical address. In protected mode, only 31:0 are loaded. The linear address written must be aligned to 8 bytes and bits 2:0 must be 0 (hardware requires bits 1:0 to be 0).
    constexpr msr_address_t IA32_PL2_SSP = 0x000006A6;

    // Linear address to be loaded into SSP on transition to privilege level 3. (R/W) If the processor does not support Intel 64 architecture, these fields have only 32 bits; bits 63:32 of the MSRs are reserved. On processors that support Intel 64 architecture this value cannot represent a non-canonical address. In protected mode, only 31:0 are loaded. The linear address written must be aligned to 8 bytes and bits 2:0 must be 0 (hardware requires bits 1:0 to be 0).
    constexpr msr_address_t IA32_PL3_SSP = 0x000006A7;

    // Linear address of a table of seven shadow stack pointers that are selected in IA-32e mode using the IST index (when not 0) from the interrupt gate descriptor. (R/W) This MSR is not present on processors that do not support Intel 64 architecture. This field cannot represent a non-canonical address.
    constexpr msr_address_t IA32_INTERRUPT_SSP_TABLE_ADDR = 0x000006A8;

    // TSC Target of Local APIC's TSC Deadline Mode (R/W)
    constexpr msr_address_t IA32_TSC_DEADLINE = 0x000006E0;

    // Specifies the PK permissions associated with each protection domain for supervisor pages (R/W)
    constexpr msr_address_t IA32_PKRS = 0x000006E1;

    // Enable/disable HWP (R/W)
    constexpr msr_address_t IA32_PM_ENABLE = 0x00000770;

    // HWP Performance Range Enumeration (R/O)
    constexpr msr_address_t IA32_HWP_CAPABILITIES = 0x00000771;

    // Power Management Control Hints for All Logical Processors in a Package (R/W)
    constexpr msr_address_t IA32_HWP_REQUEST_PKG = 0x00000772;

    // Control HWP Native Interrupts (R/W)
    constexpr msr_address_t IA32_HWP_INTERRUPT = 0x00000773;

    // Power Management Control Hints to a Logical Processor (R/W)
    constexpr msr_address_t IA32_HWP_REQUEST = 0x00000774;

    // IA32_PECI_HWP_REQUEST_INFO
    constexpr msr_address_t IA32_PECI_HWP_REQUEST_INFO = 0x00000775;

    // IA32_HWP_CTL
    constexpr msr_address_t IA32_HWP_CTL = 0x00000776;

    // Log bits indicating changes to Guaranteed & excursions to Minimum (R/W)
    constexpr msr_address_t IA32_HWP_STATUS = 0x00000777;

    // x2APIC ID Register (R/O) See x2APIC Specification.
    constexpr msr_address_t IA32_X2APIC_APICID = 0x00000802;

    // x2APIC Version Register (R/O)
    constexpr msr_address_t IA32_X2APIC_VERSION = 0x00000803;

    // x2APIC Task Priority Register (R/W)
    constexpr msr_address_t IA32_X2APIC_TPR = 0x00000808;

    // x2APIC Processor Priority Register (R/O)
    constexpr msr_address_t IA32_X2APIC_PPR = 0x0000080A;

    // x2APIC EOI Register (W/O)
    constexpr msr_address_t IA32_X2APIC_EOI = 0x0000080B;

    // x2APIC Logical Destination Register (R/O)
    constexpr msr_address_t IA32_X2APIC_LDR = 0x0000080D;

    // x2APIC Spurious Interrupt Vector Register (R/W)
    constexpr msr_address_t IA32_X2APIC_SIVR = 0x0000080F;

    // x2APIC In-Service Register Bits 31:0 (R/O)
    constexpr msr_address_t IA32_X2APIC_ISR0 = 0x00000810;

    // x2APIC In-Service Register Bits 63:32 (R/O) If CPUID.01H:ECX.[21] = 1
    constexpr msr_address_t IA32_X2APIC_ISR1 = 0x00000811;

    // x2APIC In-Service Register Bits 95:64 (R/O) If CPUID.01H:ECX.[21] = 1
    constexpr msr_address_t IA32_X2APIC_ISR2 = 0x00000812;

    // x2APIC In-Service Register Bits 127:96 (R/O)
    constexpr msr_address_t IA32_X2APIC_ISR3 = 0x00000813;

    // x2APIC In-Service Register Bits 159:128 (R/O)
    constexpr msr_address_t IA32_X2APIC_ISR4 = 0x00000814;

    // x2APIC In-Service Register Bits 191:160 (R/O)
    constexpr msr_address_t IA32_X2APIC_ISR5 = 0x00000815;

    // x2APIC In-Service Register Bits 223:192 (R/O)
    constexpr msr_address_t IA32_X2APIC_ISR6 = 0x00000816;

    // x2APIC In-Service Register Bits 255:224 (R/O)
    constexpr msr_address_t IA32_X2APIC_ISR7 = 0x00000817;

    // x2APIC Trigger Mode Register Bits 31:0 (R/O)
    constexpr msr_address_t IA32_X2APIC_TMR0 = 0x00000818;

    // x2APIC Trigger Mode Register Bits 63:32 (R/O)
    constexpr msr_address_t IA32_X2APIC_TMR1 = 0x00000819;

    // x2APIC Trigger Mode Register Bits 95:64 (R/O)
    constexpr msr_address_t IA32_X2APIC_TMR2 = 0x0000081A;

    // x2APIC Trigger Mode Register Bits 127:96 (R/O)
    constexpr msr_address_t IA32_X2APIC_TMR3 = 0x0000081B;

    // x2APIC Trigger Mode Register Bits 159:128 (R/O)
    constexpr msr_address_t IA32_X2APIC_TMR4 = 0x0000081C;

    // x2APIC Trigger Mode Register Bits 191:160 (R/O)
    constexpr msr_address_t IA32_X2APIC_TMR5 = 0x0000081D;

    // x2APIC Trigger Mode Register Bits 223:192 (R/O)
    constexpr msr_address_t IA32_X2APIC_TMR6 = 0x0000081E;

    // x2APIC Trigger Mode Register Bits 255:224 (R/O)
    constexpr msr_address_t IA32_X2APIC_TMR7 = 0x0000081F;

    // x2APIC Interrupt Request Register Bits 31:0 (R/O)
    constexpr msr_address_t IA32_X2APIC_IRR0 = 0x00000820;

    // x2APIC Interrupt Request Register Bits 63:32 (R/O)
    constexpr msr_address_t IA32_X2APIC_IRR1 = 0x00000821;

    // x2APIC Interrupt Request Register Bits 95:64 (R/O)
    constexpr msr_address_t IA32_X2APIC_IRR2 = 0x00000822;

    // x2APIC Interrupt Request Register Bits 127:96 (R/O)
    constexpr msr_address_t IA32_X2APIC_IRR3 = 0x00000823;

    // x2APIC Interrupt Request Register Bits 159:128 (R/O)
    constexpr msr_address_t IA32_X2APIC_IRR4 = 0x00000824;

    // x2APIC Interrupt Request Register Bits 191:160 (R/O)
    constexpr msr_address_t IA32_X2APIC_IRR5 = 0x00000825;

    // x2APIC Interrupt Request Register Bits 223:192 (R/O)
    constexpr msr_address_t IA32_X2APIC_IRR6 = 0x00000826;

    // x2APIC Interrupt Request Register Bits 255:224 (R/O)
    constexpr msr_address_t IA32_X2APIC_IRR7 = 0x00000827;

    // x2APIC Error Status Register (R/W)
    constexpr msr_address_t IA32_X2APIC_ESR = 0x00000828;

    // x2APIC LVT Corrected Machine Check Interrupt Register (R/W)
    constexpr msr_address_t IA32_X2APIC_LVT_CMCI = 0x0000082F;

    // x2APIC Interrupt Command Register (R/W)
    constexpr msr_address_t IA32_X2APIC_ICR = 0x00000830;

    // x2APIC LVT Timer Interrupt Register (R/W) If CPUID.01H:ECX.[21] = 1
    constexpr msr_address_t IA32_X2APIC_LVT_TIMER = 0x00000832;

    // x2APIC LVT Thermal Sensor Interrupt Register (R/W)
    constexpr msr_address_t IA32_X2APIC_LVT_THERMAL = 0x00000833;

    // x2APIC LVT Performance Monitor Interrupt Register (R/W)
    constexpr msr_address_t IA32_X2APIC_LVT_PMI = 0x00000834;

    // x2APIC LVT LINT0 Register (R/W)
    constexpr msr_address_t IA32_X2APIC_LVT_LINT0 = 0x00000835;

    // x2APIC LVT LINT1 Register (R/W)
    constexpr msr_address_t IA32_X2APIC_LVT_LINT1 = 0x00000836;

    // x2APIC LVT Error Register (R/W)
    constexpr msr_address_t IA32_X2APIC_LVT_ERROR = 0x00000837;

    // x2APIC Initial Count Register (R/W)
    constexpr msr_address_t IA32_X2APIC_INIT_COUNT = 0x00000838;

    // x2APIC Current Count Register (R/O)
    constexpr msr_address_t IA32_X2APIC_CUR_COUNT = 0x00000839;

    // x2APIC Divide Configuration Register (R/W) If CPUID.01H:ECX.[21] = 1
    constexpr msr_address_t IA32_X2APIC_DIV_CONF = 0x0000083E;

    // x2APIC Self IPI Register (W/O)
    constexpr msr_address_t IA32_X2APIC_SELF_IPI = 0x0000083F;

    // Memory Encryption Capability MSR
    constexpr msr_address_t IA32_TME_CAPABILITY = 0x00000981;

    // Memory Encryption Activation MSR This MSR is used to lock the MSRs listed below. Any write to the following MSRs will be ignored after they are locked. The lock is reset when CPU is reset. * IA32_TME_ACTIVATE * IA32_TME_EXCLUDE_MASK * IA32_TME_EXCLUDE_BASE Note that IA32_TME_EXCLUDE_MASK and IA32_TME_EXCLUDE_BASE must be configured before IA32_TME_ACTIVATE.
    constexpr msr_address_t IA32_TME_ACTIVATE = 0x00000982;

    // Memory Encryption Exclude Mask
    constexpr msr_address_t IA32_TME_EXCLUDE_MASK = 0x00000983;

    // Memory Encryption Exclude Base
    constexpr msr_address_t IA32_TME_EXCLUDE_BASE = 0x00000984;

    // Status of Most Recent Platform to Local or Local to Platform Copies (R/O)
    constexpr msr_address_t IA32_COPY_STATUS = 0x00000990;

    // Information about IWKeyBackup Register (R/O)
    constexpr msr_address_t IA32_IWKEYBACKUP_STATUS = 0x00000991;

    // Silicon Debug Feature Control (R/W)
    constexpr msr_address_t IA32_DEBUG_INTERFACE = 0x00000C80;

    // L3 QOS Configuration (R/W)
    constexpr msr_address_t IA32_L3_QOS_CFG = 0x00000C81;

    // L2 QOS Configuration (R/W)
    constexpr msr_address_t IA32_L2_QOS_CFG = 0x00000C82;

    // Monitoring Event Select Register (R/W)
    constexpr msr_address_t IA32_QM_EVTSEL = 0x00000C8D;

    // Monitoring Counter Register (R/O)
    constexpr msr_address_t IA32_QM_CTR = 0x00000C8E;

    // Resource Association Register (R/W)
    constexpr msr_address_t IA32_PQR_ASSOC = 0x00000C8F;

    // L3 CAT Mask for COS0 (R/W)
    constexpr msr_address_t IA32_L3_MASK_0 = 0x00000C90;

    // L2 CAT Mask for COS0 (R/W)
    constexpr msr_address_t IA32_L2_MASK_0 = 0x00000D10;

    // Supervisor State of MPX Configuration (R/W)
    constexpr msr_address_t IA32_BNDCFGS = 0x00000D90;

    // Copy Local State to Platform State (W)
    constexpr msr_address_t IA32_COPY_LOCAL_TO_PLATFORM = 0x00000D91;

    // Copy Platform State to Local State (W)
    constexpr msr_address_t IA32_COPY_PLATFORM_TO_LOCAL = 0x00000D92;

    // Extended Supervisor State Mask (R/W)
    constexpr msr_address_t IA32_XSS = 0x00000DA0;

    // Package Level Enable/disable HDC (R/W)
    constexpr msr_address_t IA32_PKG_HDC_CTL = 0x00000DB0;

    // Enable/disable HWP (R/W)
    constexpr msr_address_t IA32_PM_CTL1 = 0x00000DB1;

    // Per-Logical_Processor HDC Idle Residency (R/0)
    constexpr msr_address_t IA32_THREAD_STALL = 0x00000DB2;

    // Last Branch Record Entry 1200 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1200_INFO = 0x00001200;

    // Last Branch Record Entry 1201 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1201_INFO = 0x00001201;

    // Last Branch Record Entry 1202 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1202_INFO = 0x00001202;

    // Last Branch Record Entry 1203 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1203_INFO = 0x00001203;

    // Last Branch Record Entry 1204 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1204_INFO = 0x00001204;

    // Last Branch Record Entry 1205 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1205_INFO = 0x00001205;

    // Last Branch Record Entry 1206 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1206_INFO = 0x00001206;

    // Last Branch Record Entry 1207 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1207_INFO = 0x00001207;

    // Last Branch Record Entry 1208 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1208_INFO = 0x00001208;

    // Last Branch Record Entry 1209 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1209_INFO = 0x00001209;

    // Last Branch Record Entry 120A Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_120A_INFO = 0x0000120A;

    // Last Branch Record Entry 120B Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_120B_INFO = 0x0000120B;

    // Last Branch Record Entry 120C Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_120C_INFO = 0x0000120C;

    // Last Branch Record Entry 120D Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_120D_INFO = 0x0000120D;

    // Last Branch Record Entry 120E Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_120E_INFO = 0x0000120E;

    // Last Branch Record Entry 120F Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_120F_INFO = 0x0000120F;

    // Last Branch Record Entry 1210 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1210_INFO = 0x00001210;

    // Last Branch Record Entry 1211 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1211_INFO = 0x00001211;

    // Last Branch Record Entry 1212 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1212_INFO = 0x00001212;

    // Last Branch Record Entry 1213 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1213_INFO = 0x00001213;

    // Last Branch Record Entry 1214 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1214_INFO = 0x00001214;

    // Last Branch Record Entry 1215 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1215_INFO = 0x00001215;

    // Last Branch Record Entry 1216 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1216_INFO = 0x00001216;

    // Last Branch Record Entry 1217 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1217_INFO = 0x00001217;

    // Last Branch Record Entry 1218 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1218_INFO = 0x00001218;

    // Last Branch Record Entry 1219 Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1219_INFO = 0x00001219;

    // Last Branch Record Entry 121A Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_121A_INFO = 0x0000121A;

    // Last Branch Record Entry 121B Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_121B_INFO = 0x0000121B;

    // Last Branch Record Entry 121C Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_121C_INFO = 0x0000121C;

    // Last Branch Record Entry 121D Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_121D_INFO = 0x0000121D;

    // Last Branch Record Entry 121E Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_121E_INFO = 0x0000121E;

    // Last Branch Record Entry 121F Info Register (R/W) An attempt to read or write IA32_LBR_x_INFO such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_121F_INFO = 0x0000121F;

    // Last Branch Record Enabling and Configuration Register (R/W)
    constexpr msr_address_t IA32_LBR_CTL = 0x000014CE;

    // Last Branch Record Maximum Stack Depth Register (R/W)
    constexpr msr_address_t IA32_LBR_DEPTH = 0x000014CF;

    // Last Branch Record entry 1500 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1500_FROM_IP = 0x00001500;

    // Last Branch Record entry 1501 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1501_FROM_IP = 0x00001501;

    // Last Branch Record entry 1502 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1502_FROM_IP = 0x00001502;

    // Last Branch Record entry 1503 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1503_FROM_IP = 0x00001503;

    // Last Branch Record entry 1504 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1504_FROM_IP = 0x00001504;

    // Last Branch Record entry 1505 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1505_FROM_IP = 0x00001505;

    // Last Branch Record entry 1506 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1506_FROM_IP = 0x00001506;

    // Last Branch Record entry 1507 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1507_FROM_IP = 0x00001507;

    // Last Branch Record entry 1508 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1508_FROM_IP = 0x00001508;

    // Last Branch Record entry 1509 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1509_FROM_IP = 0x00001509;

    // Last Branch Record entry 150A source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_150A_FROM_IP = 0x0000150A;

    // Last Branch Record entry 150B source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_150B_FROM_IP = 0x0000150B;

    // Last Branch Record entry 150C source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_150C_FROM_IP = 0x0000150C;

    // Last Branch Record entry 150D source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_150D_FROM_IP = 0x0000150D;

    // Last Branch Record entry 150E source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_150E_FROM_IP = 0x0000150E;

    // Last Branch Record entry 150F source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_150F_FROM_IP = 0x0000150F;

    // Last Branch Record entry 1510 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1510_FROM_IP = 0x00001510;

    // Last Branch Record entry 1511 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1511_FROM_IP = 0x00001511;

    // Last Branch Record entry 1512 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1512_FROM_IP = 0x00001512;

    // Last Branch Record entry 1513 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1513_FROM_IP = 0x00001513;

    // Last Branch Record entry 1514 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1514_FROM_IP = 0x00001514;

    // Last Branch Record entry 1515 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1515_FROM_IP = 0x00001515;

    // Last Branch Record entry 1516 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1516_FROM_IP = 0x00001516;

    // Last Branch Record entry 1517 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1517_FROM_IP = 0x00001517;

    // Last Branch Record entry 1518 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1518_FROM_IP = 0x00001518;

    // Last Branch Record entry 1519 source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1519_FROM_IP = 0x00001519;

    // Last Branch Record entry 151A source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_151A_FROM_IP = 0x0000151A;

    // Last Branch Record entry 151B source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_151B_FROM_IP = 0x0000151B;

    // Last Branch Record entry 151C source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_151C_FROM_IP = 0x0000151C;

    // Last Branch Record entry 151D source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_151D_FROM_IP = 0x0000151D;

    // Last Branch Record entry 151E source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_151E_FROM_IP = 0x0000151E;

    // Last Branch Record entry 151F source IP register (R/W). An attempt to read or write IA32_LBR_x_FROM_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_151F_FROM_IP = 0x0000151F;

    // Last Branch Record Entry 1600 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1600_TO_IP = 0x00001600;

    // Last Branch Record Entry 1601 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1601_TO_IP = 0x00001601;

    // Last Branch Record Entry 1602 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1602_TO_IP = 0x00001602;

    // Last Branch Record Entry 1603 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1603_TO_IP = 0x00001603;

    // Last Branch Record Entry 1604 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1604_TO_IP = 0x00001604;

    // Last Branch Record Entry 1605 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1605_TO_IP = 0x00001605;

    // Last Branch Record Entry 1606 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1606_TO_IP = 0x00001606;

    // Last Branch Record Entry 1607 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1607_TO_IP = 0x00001607;

    // Last Branch Record Entry 1608 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1608_TO_IP = 0x00001608;

    // Last Branch Record Entry 1609 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1609_TO_IP = 0x00001609;

    // Last Branch Record Entry 160A Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_160A_TO_IP = 0x0000160A;

    // Last Branch Record Entry 160B Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_160B_TO_IP = 0x0000160B;

    // Last Branch Record Entry 160C Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_160C_TO_IP = 0x0000160C;

    // Last Branch Record Entry 160D Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_160D_TO_IP = 0x0000160D;

    // Last Branch Record Entry 160E Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_160E_TO_IP = 0x0000160E;

    // Last Branch Record Entry 160F Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_160F_TO_IP = 0x0000160F;

    // Last Branch Record Entry 1610 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1610_TO_IP = 0x00001610;

    // Last Branch Record Entry 1611 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1611_TO_IP = 0x00001611;

    // Last Branch Record Entry 1612 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1612_TO_IP = 0x00001612;

    // Last Branch Record Entry 1613 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1613_TO_IP = 0x00001613;

    // Last Branch Record Entry 1614 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1614_TO_IP = 0x00001614;

    // Last Branch Record Entry 1615 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1615_TO_IP = 0x00001615;

    // Last Branch Record Entry 1616 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1616_TO_IP = 0x00001616;

    // Last Branch Record Entry 1617 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1617_TO_IP = 0x00001617;

    // Last Branch Record Entry 1618 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1618_TO_IP = 0x00001618;

    // Last Branch Record Entry 1619 Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_1619_TO_IP = 0x00001619;

    // Last Branch Record Entry 161A Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_161A_TO_IP = 0x0000161A;

    // Last Branch Record Entry 161B Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_161B_TO_IP = 0x0000161B;

    // Last Branch Record Entry 161C Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_161C_TO_IP = 0x0000161C;

    // Last Branch Record Entry 161D Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_161D_TO_IP = 0x0000161D;

    // Last Branch Record Entry 161E Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_161E_TO_IP = 0x0000161E;

    // Last Branch Record Entry 161F Destination IP Register (R/W) An attempt to read or write IA32_LBR_x_TO_IP such that x >= IA32_LBR_DEPTH.DEPTH will #GP.
    constexpr msr_address_t IA32_LBR_161F_TO_IP = 0x0000161F;

    // Hardware Feedback Interface Pointer
    constexpr msr_address_t IA32_HW_FEEDBACK_PTR = 0x000017D0;

    // Hardware Feedback Interface Configuration If CPUID.06H:EAX.[19] = 1
    constexpr msr_address_t IA32_HW_FEEDBACK_CONFIG = 0x000017D1;

    // Thread Feedback Characteristics (R/O)
    constexpr msr_address_t IA32_THREAD_FEEDBACK_CHAR = 0x000017D2;

    // Hardware Feedback Thread Configuration (R/W)
    constexpr msr_address_t IA32_HW_FEEDBACK_THREAD_CONFI = 0x000017D4;

    // History Reset Enable (R/W)
    constexpr msr_address_t IA32_HRESET_ENABLE = 0x000017DA;

    // Extended Feature Enables
    constexpr msr_address_t IA32_EFER = 0xC0000080;

    // System Call Target Address (R/W)
    constexpr msr_address_t IA32_STAR = 0xC0000081;

    // IA-32e Mode System Call Target Address (R/W) Target RIP for the called procedure when SYSCALL is executed in 64-bit mode.
    constexpr msr_address_t IA32_LSTAR = 0xC0000082;

    // IA-32e Mode System Call Target Address (R/W) Not used, as the SYSCALL instruction is not recognized in compatibility mode.
    constexpr msr_address_t IA32_CSTAR = 0xC0000083;

    // System Call Flag Mask (R/W)
    constexpr msr_address_t IA32_FMASK = 0xC0000084;

    // Map of BASE Address of FS (R/W)
    constexpr msr_address_t IA32_FS_BASE = 0xC0000100;

    // Map of BASE Address of GS (R/W)
    constexpr msr_address_t IA32_GS_BASE = 0xC0000101;

    // Swap Target of BASE Address of GS (R/W)
    constexpr msr_address_t IA32_KERNEL_GS_BASE = 0xC0000102;

    // Auxiliary TSC (R/W)
    constexpr msr_address_t IA32_TSC_AUX = 0xC0000103;

    template<msr_address_t>
    struct msr_t {
        uint64_t storage;
    };

    template<>
    struct msr_t<IA32_FEATURE_CONTROL> {
        union {
            uint64_t storage;
            struct {
                uint64_t lock : 1;
                uint64_t enable_vmx_inside_smx : 1;
                uint64_t enable_vmx_outside_smx : 1;
                uint64_t reserved0 : 5;
                uint64_t senter_function_bitmap : 7;
                uint64_t senter_global_enable : 1;
                uint64_t reserved1 : 1;
                uint64_t sgx_launch_control_enable : 1;
                uint64_t sgx_global_enable : 1;
                uint64_t reserved2 : 1;
                uint64_t lmce_on : 1;
                uint64_t reserved3 : 43;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_FEATURE_CONTROL>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_FEATURE_CONTROL>::storage) == sizeof(msr_t<IA32_FEATURE_CONTROL>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 11 Memory Cache Control
    //    |-> 11.11 Memory Type Range Registers (MTRRs)
    //      |-> 11.11.1 MTRR Feature Identification
    //        |-> Figure 11-5. IA32_MTRRCAP Register
    // and
    // [*] Volume 4: Model-Specific Registers
    //  |-> Chapter 2 Model-Specific Registers (MSRs)
    //    |-> 2.1 Architectural MSRs
    template<>
    struct msr_t<IA32_MTRRCAP> {
        union {
            uint64_t storage;
            struct {
                uint64_t variable_range_mtrrs_count : 8;
                uint64_t fixed_range_mtrrs_support : 1;
                uint64_t reserved0 : 1;
                uint64_t write_combining_support : 1;
                uint64_t smrr_support : 1;
                uint64_t prmrr_support : 1;
                uint64_t reserved1 : 51;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRRCAP>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRRCAP>::storage) == sizeof(msr_t<IA32_MTRRCAP>::semantics));

    template<>
    struct msr_t<IA32_SYSENTER_CS> {
        union {
            uint64_t storage;
            struct {
                uint64_t cs : 16;
                uint64_t ignored : 48;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_SYSENTER_CS>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_SYSENTER_CS>::storage) == sizeof(msr_t<IA32_SYSENTER_CS>::semantics));

    template<>
    struct msr_t<IA32_SYSENTER_EIP> {
        union {
            uint64_t storage;
            union {
                uint32_t eip;
                uint64_t rip;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_SYSENTER_EIP>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_SYSENTER_EIP>::storage) == sizeof(msr_t<IA32_SYSENTER_EIP>::semantics));

    template<>
    struct msr_t<IA32_SYSENTER_ESP> {
        union {
            uint64_t storage;
            union {
                uint32_t esp;
                uint64_t rsp;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_SYSENTER_ESP>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_SYSENTER_ESP>::storage) == sizeof(msr_t<IA32_SYSENTER_ESP>::semantics));

    template<>
    struct msr_t<IA32_MISC_ENABLE> {
        union {
            uint64_t storage;
            struct {
                uint64_t fast_strings_enable : 1;
                uint64_t reserved0 : 2;
                uint64_t automatic_thermal_control_circuit_enable : 1;
                uint64_t reserved1 : 3;
                uint64_t performance_monitoring_available : 1;
                uint64_t reserved2 : 3;
                uint64_t branch_trace_storage_unavailable : 1;
                uint64_t processor_event_based_sampling_unavailable : 1;
                uint64_t reserved3 : 3;
                uint64_t enhanced_intel_speedstep_technology_enable : 1;
                uint64_t reserved4 : 1;
                uint64_t enable_monitor_fsm : 1;
                uint64_t reserved5 : 3;
                uint64_t limit_cpuid_maxval : 1;
                uint64_t xtpr_message_disable : 1;
                uint64_t reserved6 : 10;
                uint64_t xd_bit_disable : 1;
                uint64_t reserved7 : 29;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MISC_ENABLE>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MISC_ENABLE>::storage) == sizeof(msr_t<IA32_MISC_ENABLE>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 11 Memory Cache Control
    //    |-> 11.11 Memory Type Range Registers (MTRRs)
    //      |-> 11.11.2 Setting Memory Ranges with MTRRs
    //        |-> 11.11.2.4 System-Management Range Register Interface
    //          |-> Figure 11-8. IA32_SMRR_PHYSBASE and IA32_SMRR_PHYSMASK SMRR Pair
    template<>
    struct msr_t<IA32_SMRR_PHYSBASE> {
        union {
            uint64_t storage;
            struct {
                uint64_t memory_type : 8;
                uint64_t reserved0 : 4;
                uint64_t physical_base : 20;
                uint64_t reserved1 : 32;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_SMRR_PHYSBASE>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_SMRR_PHYSBASE>::storage) == sizeof(msr_t<IA32_SMRR_PHYSBASE>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 11 Memory Cache Control
    //    |-> 11.11 Memory Type Range Registers (MTRRs)
    //      |-> 11.11.2 Setting Memory Ranges with MTRRs
    //        |-> 11.11.2.4 System-Management Range Register Interface
    //          |-> Figure 11-8. IA32_SMRR_PHYSBASE and IA32_SMRR_PHYSMASK SMRR Pair
    template<>
    struct msr_t<IA32_SMRR_PHYSMASK> {
        union {
            uint64_t storage;
            struct {
                uint64_t reserved0 : 11;
                uint64_t valid : 1;
                uint64_t physical_mask : 20;
                uint64_t reserved1 : 32;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_SMRR_PHYSMASK>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_SMRR_PHYSMASK>::storage) == sizeof(msr_t<IA32_SMRR_PHYSMASK>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSBASE0> {
        union {
            uint64_t storage;
            struct {
                uint64_t memory_type : 8;
                uint64_t reserved0 : 4;
                uint64_t physical_base : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE0>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE0>::storage) == sizeof(msr_t<IA32_MTRR_PHYSBASE0>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSMASK0> {
        union {
            uint64_t storage;
            struct {
                uint64_t reserved0 : 11;
                uint64_t valid : 1;
                uint64_t physical_mask : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK0>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK0>::storage) == sizeof(msr_t<IA32_MTRR_PHYSMASK0>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSBASE1> {
        union {
            uint64_t storage;
            struct {
                uint64_t memory_type : 8;
                uint64_t reserved0 : 4;
                uint64_t physical_base : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE1>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE1>::storage) == sizeof(msr_t<IA32_MTRR_PHYSBASE1>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSMASK1> {
        union {
            uint64_t storage;
            struct {
                uint64_t reserved0 : 11;
                uint64_t valid : 1;
                uint64_t physical_mask : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK1>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK1>::storage) == sizeof(msr_t<IA32_MTRR_PHYSMASK1>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSBASE2> {
        union {
            uint64_t storage;
            struct {
                uint64_t memory_type : 8;
                uint64_t reserved0 : 4;
                uint64_t physical_base : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE2>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE2>::storage) == sizeof(msr_t<IA32_MTRR_PHYSBASE2>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSMASK2> {
        union {
            uint64_t storage;
            struct {
                uint64_t reserved0 : 11;
                uint64_t valid : 1;
                uint64_t physical_mask : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK2>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK2>::storage) == sizeof(msr_t<IA32_MTRR_PHYSMASK2>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSBASE3> {
        union {
            uint64_t storage;
            struct {
                uint64_t memory_type : 8;
                uint64_t reserved0 : 4;
                uint64_t physical_base : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE3>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE3>::storage) == sizeof(msr_t<IA32_MTRR_PHYSBASE3>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSMASK3> {
        union {
            uint64_t storage;
            struct {
                uint64_t reserved0 : 11;
                uint64_t valid : 1;
                uint64_t physical_mask : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK3>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK3>::storage) == sizeof(msr_t<IA32_MTRR_PHYSMASK3>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSBASE4> {
        union {
            uint64_t storage;
            struct {
                uint64_t memory_type : 8;
                uint64_t reserved0 : 4;
                uint64_t physical_base : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE4>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE4>::storage) == sizeof(msr_t<IA32_MTRR_PHYSBASE4>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSMASK4> {
        union {
            uint64_t storage;
            struct {
                uint64_t reserved0 : 11;
                uint64_t valid : 1;
                uint64_t physical_mask : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK4>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK4>::storage) == sizeof(msr_t<IA32_MTRR_PHYSMASK4>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSBASE5> {
        union {
            uint64_t storage;
            struct {
                uint64_t memory_type : 8;
                uint64_t reserved0 : 4;
                uint64_t physical_base : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE5>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE5>::storage) == sizeof(msr_t<IA32_MTRR_PHYSBASE5>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSMASK5> {
        union {
            uint64_t storage;
            struct {
                uint64_t reserved0 : 11;
                uint64_t valid : 1;
                uint64_t physical_mask : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK5>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK5>::storage) == sizeof(msr_t<IA32_MTRR_PHYSMASK5>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSBASE6> {
        union {
            uint64_t storage;
            struct {
                uint64_t memory_type : 8;
                uint64_t reserved0 : 4;
                uint64_t physical_base : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE6>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE6>::storage) == sizeof(msr_t<IA32_MTRR_PHYSBASE6>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSMASK6> {
        union {
            uint64_t storage;
            struct {
                uint64_t reserved0 : 11;
                uint64_t valid : 1;
                uint64_t physical_mask : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK6>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK6>::storage) == sizeof(msr_t<IA32_MTRR_PHYSMASK6>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSBASE7> {
        union {
            uint64_t storage;
            struct {
                uint64_t memory_type : 8;
                uint64_t reserved0 : 4;
                uint64_t physical_base : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE7>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE7>::storage) == sizeof(msr_t<IA32_MTRR_PHYSBASE7>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSMASK7> {
        union {
            uint64_t storage;
            struct {
                uint64_t reserved0 : 11;
                uint64_t valid : 1;
                uint64_t physical_mask : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK7>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK7>::storage) == sizeof(msr_t<IA32_MTRR_PHYSMASK7>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSBASE8> {
        union {
            uint64_t storage;
            struct {
                uint64_t memory_type : 8;
                uint64_t reserved0 : 4;
                uint64_t physical_base : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE8>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE8>::storage) == sizeof(msr_t<IA32_MTRR_PHYSBASE8>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSMASK8> {
        union {
            uint64_t storage;
            struct {
                uint64_t reserved0 : 11;
                uint64_t valid : 1;
                uint64_t physical_mask : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK8>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK8>::storage) == sizeof(msr_t<IA32_MTRR_PHYSMASK8>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSBASE9> {
        union {
            uint64_t storage;
            struct {
                uint64_t memory_type : 8;
                uint64_t reserved0 : 4;
                uint64_t physical_base : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE9>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSBASE9>::storage) == sizeof(msr_t<IA32_MTRR_PHYSBASE9>::semantics));

    template<>
    struct msr_t<IA32_MTRR_PHYSMASK9> {
        union {
            uint64_t storage;
            struct {
                uint64_t reserved0 : 11;
                uint64_t valid : 1;
                uint64_t physical_mask : 40;
                uint64_t reserved1 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK9>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_PHYSMASK9>::storage) == sizeof(msr_t<IA32_MTRR_PHYSMASK9>::semantics));

    template<>
    struct msr_t<IA32_MTRR_FIX64K_00000> {
        union {
            uint64_t storage;
            struct {
                uint8_t memory_type[8];
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_FIX64K_00000>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_FIX64K_00000>::storage) == sizeof(msr_t<IA32_MTRR_FIX64K_00000>::semantics));

    template<>
    struct msr_t<IA32_MTRR_FIX16K_80000> {
        union {
            uint64_t storage;
            struct {
                uint8_t memory_type[8];
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_FIX16K_80000>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_FIX16K_80000>::storage) == sizeof(msr_t<IA32_MTRR_FIX16K_80000>::semantics));

    template<>
    struct msr_t<IA32_MTRR_FIX16K_A0000> {
        union {
            uint64_t storage;
            struct {
                uint8_t memory_type[8];
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_FIX16K_A0000>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_FIX16K_A0000>::storage) == sizeof(msr_t<IA32_MTRR_FIX16K_A0000>::semantics));

    template<>
    struct msr_t<IA32_MTRR_FIX4K_C0000> {
        union {
            uint64_t storage;
            struct {
                uint8_t memory_type[8];
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_C0000>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_C0000>::storage) == sizeof(msr_t<IA32_MTRR_FIX4K_C0000>::semantics));

    template<>
    struct msr_t<IA32_MTRR_FIX4K_C8000> {
        union {
            uint64_t storage;
            struct {
                uint8_t memory_type[8];
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_C8000>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_C8000>::storage) == sizeof(msr_t<IA32_MTRR_FIX4K_C8000>::semantics));

    template<>
    struct msr_t<IA32_MTRR_FIX4K_D0000> {
        union {
            uint64_t storage;
            struct {
                uint8_t memory_type[8];
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_D0000>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_D0000>::storage) == sizeof(msr_t<IA32_MTRR_FIX4K_D0000>::semantics));

    template<>
    struct msr_t<IA32_MTRR_FIX4K_D8000> {
        union {
            uint64_t storage;
            struct {
                uint8_t memory_type[8];
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_D8000>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_D8000>::storage) == sizeof(msr_t<IA32_MTRR_FIX4K_D8000>::semantics));

    template<>
    struct msr_t<IA32_MTRR_FIX4K_E0000> {
        union {
            uint64_t storage;
            struct {
                uint8_t memory_type[8];
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_E0000>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_E0000>::storage) == sizeof(msr_t<IA32_MTRR_FIX4K_E0000>::semantics));

    template<>
    struct msr_t<IA32_MTRR_FIX4K_E8000> {
        union {
            uint64_t storage;
            struct {
                uint8_t memory_type[8];
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_E8000>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_E8000>::storage) == sizeof(msr_t<IA32_MTRR_FIX4K_E8000>::semantics));

    template<>
    struct msr_t<IA32_MTRR_FIX4K_F0000> {
        union {
            uint64_t storage;
            struct {
                uint8_t memory_type[8];
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_F0000>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_F0000>::storage) == sizeof(msr_t<IA32_MTRR_FIX4K_F0000>::semantics));

    template<>
    struct msr_t<IA32_MTRR_FIX4K_F8000> {
        union {
            uint64_t storage;
            struct {
                uint8_t memory_type[8];
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_F8000>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_FIX4K_F8000>::storage) == sizeof(msr_t<IA32_MTRR_FIX4K_F8000>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 11 Memory Cache Control
    //    |-> 11.11 Memory Type Range Registers (MTRRs)
    //      |-> 11.11.2 Setting Memory Ranges with MTRRs
    //        |-> 11.11.2.1 IA32_MTRR_DEF_TYPE MSR
    //          |-> Figure 11-6. IA32_MTRR_DEF_TYPE MSR
    template<>
    struct msr_t<IA32_MTRR_DEF_TYPE> {
        union {
            uint64_t storage;
            struct {
                uint64_t default_memory_type : 8;
                uint64_t reserved0 : 2;
                uint64_t fixed_range_mtrrs_enable : 1;
                uint64_t mtrrs_enable : 1;
                uint64_t reserved1 : 52;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_MTRR_DEF_TYPE>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_MTRR_DEF_TYPE>::storage) == sizeof(msr_t<IA32_MTRR_DEF_TYPE>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Appendix A VMX Capability Reporting Facility
    //    |-> A.1 Basic VMX Information
    template<>
    struct msr_t<IA32_VMX_BASIC> {
        union {
            uint64_t storage;
            struct {
                uint64_t vmcs_revision : 31;
                uint64_t reserved0 : 1;
                uint64_t vmcs_size : 13;
                uint64_t reserved1 : 3;
                uint64_t use_32bits_physical_address : 1;    // This bit is always 0 for processors that support Intel 64 architecture.
                uint64_t smi_smm_dual_monitor_treatment : 1;
                uint64_t vmcs_memory_type : 4;
                uint64_t ins_outs_instruction_reporting : 1;
                uint64_t default1_controls_can_be_zero : 1;
                uint64_t use_vmentry_to_deliver_hardware_exceptions : 1;
                uint64_t reserved2 : 7;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_BASIC>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_BASIC>::storage) == sizeof(msr_t<IA32_VMX_BASIC>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Appendix A VMX Capability Reporting Facility
    //    |-> A.3 VM-Execution Controls
    //      |-> A.3.1 Pin-Based VM-Execution Controls
    template<>
    struct msr_t<IA32_VMX_PINBASED_CTLS> {
        union {
            uint64_t storage;
            struct {
                uint32_t mask0;
                uint32_t mask1;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_PINBASED_CTLS>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_PINBASED_CTLS>::storage) == sizeof(msr_t<IA32_VMX_PINBASED_CTLS>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Appendix A VMX Capability Reporting Facility
    //    |-> A.3 VM-Execution Controls
    //      |-> A.3.2 Primary Processor-Based VM-Execution Controls
    template<>
    struct msr_t<IA32_VMX_PROCBASED_CTLS> {
        union {
            uint64_t storage;
            struct {
                uint32_t mask0;
                uint32_t mask1;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_PROCBASED_CTLS>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_PROCBASED_CTLS>::storage) == sizeof(msr_t<IA32_VMX_PROCBASED_CTLS>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Appendix A VMX Capability Reporting Facility
    //    |-> A.4 VM-Exit Conrols
    //      |-> A.4.1 Primary VM-Exit Controls
    template<>
    struct msr_t<IA32_VMX_EXIT_CTLS> {
        union {
            uint64_t storage;
            struct {
                uint32_t mask0;
                uint32_t mask1;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_EXIT_CTLS>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_EXIT_CTLS>::storage) == sizeof(msr_t<IA32_VMX_EXIT_CTLS>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Appendix A VMX Capability Reporting Facility
    //    |-> A.5 VM-Entry Controls
    template<>
    struct msr_t<IA32_VMX_ENTRY_CTLS> {
        union {
            uint64_t storage;
            struct {
                uint32_t mask0;
                uint32_t mask1;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_ENTRY_CTLS>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_ENTRY_CTLS>::storage) == sizeof(msr_t<IA32_VMX_ENTRY_CTLS>::semantics));

    template<>
    struct msr_t<IA32_VMX_MISC> {
        union {
            uint64_t storage;
            struct {
                uint64_t todo0 : 5;
                uint64_t todo1 : 1;
                uint64_t support_for_activity_state_1 : 1;
                uint64_t support_for_activity_state_2 : 1;
                uint64_t support_for_activity_state_3 : 1;
                uint64_t reserved0 : 5;
                uint64_t todo2 : 1;
                uint64_t todo3 : 1;
                uint64_t todo4 : 9;
                uint64_t todo5 : 3;
                uint64_t todo6 : 1;
                uint64_t todo7 : 1;
                uint64_t todo8 : 1;
                uint64_t reserved1 : 1;
                uint64_t mseg_revision : 32;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_MISC>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_MISC>::storage) == sizeof(msr_t<IA32_VMX_MISC>::semantics));

    template<>
    struct msr_t<IA32_VMX_CR0_FIXED0> {
        union {
            uint64_t storage;
            struct {
                uint64_t mask_value;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_CR0_FIXED0>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_CR0_FIXED0>::storage) == sizeof(msr_t<IA32_VMX_CR0_FIXED0>::semantics));

    template<>
    struct msr_t<IA32_VMX_CR0_FIXED1> {
        union {
            uint64_t storage;
            struct {
                uint64_t mask_value;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_CR0_FIXED1>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_CR0_FIXED1>::storage) == sizeof(msr_t<IA32_VMX_CR0_FIXED1>::semantics));

    template<>
    struct msr_t<IA32_VMX_CR4_FIXED0> {
        union {
            uint64_t storage;
            struct {
                uint64_t mask_value;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_CR4_FIXED0>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_CR4_FIXED0>::storage) == sizeof(msr_t<IA32_VMX_CR4_FIXED0>::semantics));

    template<>
    struct msr_t<IA32_VMX_CR4_FIXED1> {
        union {
            uint64_t storage;
            struct {
                uint64_t mask_value;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_CR4_FIXED1>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_CR4_FIXED1>::storage) == sizeof(msr_t<IA32_VMX_CR4_FIXED1>::semantics));

    template<>
    struct msr_t<IA32_VMX_VMCS_ENUM> {
        union {
            uint64_t storage;
            struct {
                uint64_t reserved0 : 1;
                uint64_t highest_index_for_vmcs_encoding : 9;
                uint64_t reserved1 : 54;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_VMCS_ENUM>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_VMCS_ENUM>::storage) == sizeof(msr_t<IA32_VMX_VMCS_ENUM>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Appendix A VMX Capability Reporting Facility
    //    |-> A.3 VM-Execution Controls
    //      |-> A.3.3 Secondary Processor-Based VM-Execution Controls
    template<>
    struct msr_t<IA32_VMX_PROCBASED_CTLS2> {
        union {
            uint64_t storage;
            struct {
                uint32_t mask0;
                uint32_t mask1;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_PROCBASED_CTLS2>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_PROCBASED_CTLS2>::storage) == sizeof(msr_t<IA32_VMX_PROCBASED_CTLS2>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Appendix A VMX Capability Reporting Facility
    //    |-> A.10 VPID and EPT Capabilities
    template<>
    struct msr_t<IA32_VMX_EPT_VPID_CAP> {
        union {
            uint64_t storage;
            struct {
                uint64_t support_execute_only : 1;
                uint64_t reserved0 : 5;
                uint64_t support_pml4 : 1;
                uint64_t support_pml5 : 1;  // from Intel's '5-Level Paging and 5-Level EPT White Paper'
                uint64_t allow_ept_cache_uncacheable : 1;
                uint64_t reserved2 : 5;
                uint64_t allow_ept_cache_write_back : 1;
                uint64_t reserved3 : 1;
                uint64_t support_2mb_page : 1;
                uint64_t support_1gb_page : 1;
                uint64_t reserved4 : 2;
                uint64_t support_invept : 1;
                uint64_t support_accessed_and_ditry_flags : 1;
                uint64_t advanced_reports_for_ept_violations : 1;
                uint64_t support_supervisor_shadow_stack_control : 1;
                uint64_t reserved5 : 1;
                uint64_t support_invept_single_context : 1;
                uint64_t support_invept_all_context : 1;
                uint64_t reserved6 : 5;
                uint64_t support_invvpid : 1;
                uint64_t reserved7 : 7;
                uint64_t support_invvpid_individual_address : 1;
                uint64_t support_invvpid_single_context : 1;
                uint64_t support_invvpid_all_context : 1;
                uint64_t support_invvpid_single_context_retaining_globals : 1;
                uint64_t reserved8 : 4;
                uint64_t maximum_hlat_prefix_size : 6;
                uint64_t reserved9 : 10;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_EPT_VPID_CAP>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_EPT_VPID_CAP>::storage) == sizeof(msr_t<IA32_VMX_EPT_VPID_CAP>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Appendix A VMX Capability Reporting Facility
    //    |-> A.3 VM-Execution Controls
    //      |-> A.3.1 Pin-Based VM-Execution Controls
    template<>
    struct msr_t<IA32_VMX_TRUE_PINBASED_CTLS> {
        union {
            uint64_t storage;
            struct {
                uint32_t mask0;
                uint32_t mask1;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_TRUE_PINBASED_CTLS>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_TRUE_PINBASED_CTLS>::storage) == sizeof(msr_t<IA32_VMX_TRUE_PINBASED_CTLS>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Appendix A VMX Capability Reporting Facility
    //    |-> A.3 VM-Execution Controls
    //      |-> A.3.2 Primary Processor-Based VM-Execution Controls
    template<>
    struct msr_t<IA32_VMX_TRUE_PROCBASED_CTLS> {
        union {
            uint64_t storage;
            struct {
                uint32_t mask0;
                uint32_t mask1;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_TRUE_PROCBASED_CTLS>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_TRUE_PROCBASED_CTLS>::storage) == sizeof(msr_t<IA32_VMX_TRUE_PROCBASED_CTLS>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Appendix A VMX Capability Reporting Facility
    //    |-> A.4 VM-Exit Conrols
    //      |-> A.4.1 Primary VM-Exit Controls
    template<>
    struct msr_t<IA32_VMX_TRUE_EXIT_CTLS> {
        union {
            uint64_t storage;
            struct {
                uint32_t mask0;
                uint32_t mask1;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_TRUE_EXIT_CTLS>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_TRUE_EXIT_CTLS>::storage) == sizeof(msr_t<IA32_VMX_TRUE_EXIT_CTLS>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Appendix A VMX Capability Reporting Facility
    //    |-> A.5 VM-Entry Controls
    template<>
    struct msr_t<IA32_VMX_TRUE_ENTRY_CTLS> {
        union {
            uint64_t storage;
            struct {
                uint32_t mask0;
                uint32_t mask1;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_TRUE_ENTRY_CTLS>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_TRUE_ENTRY_CTLS>::storage) == sizeof(msr_t<IA32_VMX_TRUE_ENTRY_CTLS>::semantics));

    template<>
    struct msr_t<IA32_VMX_VMFUNC> {
        union {
            uint64_t storage;
            struct {
                uint64_t eptp_switching : 1;
                uint64_t reserved0 : 63;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_VMFUNC>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_VMFUNC>::storage) == sizeof(msr_t<IA32_VMX_VMFUNC>::semantics));

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Appendix A VMX Capability Reporting Facility
    //    |-> A.3 VM-Execution Controls
    //      |-> A.3.4 Tertiary Processor-Based VM-Execution Controls
    template<>
    struct msr_t<IA32_VMX_PROCBASED_CTLS3> {
        union {
            uint64_t storage;
            struct {
                uint64_t mask;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_PROCBASED_CTLS3>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_PROCBASED_CTLS3>::storage) == sizeof(msr_t<IA32_VMX_PROCBASED_CTLS3>::semantics));

    template<>
    struct msr_t<IA32_VMX_EXIT_CTLS2> {
        union {
            uint64_t storage;
            struct {
                uint64_t mask;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_VMX_EXIT_CTLS2>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_VMX_EXIT_CTLS2>::storage) == sizeof(msr_t<IA32_VMX_EXIT_CTLS2>::semantics));

    template<>
    struct msr_t<IA32_EFER> {
        union {
            uint64_t storage;
            struct {
                uint64_t syscall_enable : 1;
                uint64_t reserved0 : 7;
                uint64_t ia32e_mode_enable : 1;
                uint64_t reserved1 : 1;
                uint64_t ia32e_mode_active : 1;
                uint64_t nx_enable : 1;
                uint64_t reserved2 : 52;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_EFER>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_EFER>::storage) == sizeof(msr_t<IA32_EFER>::semantics));

    template<>
    struct msr_t<IA32_STAR> {
        union {
            uint64_t storage;
            struct {
                uint64_t reserved : 32;
                uint64_t syscall_cs : 16;
                uint64_t sysret_cs : 16;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_STAR>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_STAR>::storage) == sizeof(msr_t<IA32_STAR>::semantics));

    template<>
    struct msr_t<IA32_LSTAR> {
        union {
            uint64_t storage;
            struct {
                uint64_t rip;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_LSTAR>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_LSTAR>::storage) == sizeof(msr_t<IA32_LSTAR>::semantics));

    template<>
    struct msr_t<IA32_FMASK> {
        union {
            uint64_t storage;
            struct {
                uint64_t syscall_eflags_mask : 32;
                uint64_t reserved : 32;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_FMASK>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_FMASK>::storage) == sizeof(msr_t<IA32_FMASK>::semantics));

    template<>
    struct msr_t<IA32_FS_BASE> {
        union {
            uint64_t storage;
            struct {
                uint64_t base;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_FS_BASE>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_FS_BASE>::storage) == sizeof(msr_t<IA32_FS_BASE>::semantics));

    template<>
    struct msr_t<IA32_GS_BASE> {
        union {
            uint64_t storage;
            struct {
                uint64_t base;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_GS_BASE>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_GS_BASE>::storage) == sizeof(msr_t<IA32_GS_BASE>::semantics));

    template<>
    struct msr_t<IA32_KERNEL_GS_BASE> {
        union {
            uint64_t storage;
            struct {
                uint64_t base;
            } semantics;
        };
    };

    static_assert(sizeof(msr_t<IA32_KERNEL_GS_BASE>) == sizeof(msr_storage_t));
    static_assert(sizeof(msr_t<IA32_KERNEL_GS_BASE>::storage) == sizeof(msr_t<IA32_KERNEL_GS_BASE>::semantics));

    [[nodiscard]]
    inline msr_storage_t read_msr(msr_address_t address) noexcept {
        return msr_storage_t{ __readmsr(address) };
    }

    inline void write_msr(msr_address_t msr_address, msr_storage_t msr_value) noexcept {
        return __writemsr(msr_address, msr_value);
    }

    template<msr_address_t MsrAddress>
    [[nodiscard]]
    msr_t<MsrAddress> read_msr() noexcept {
        return msr_t<MsrAddress>{ .storage = __readmsr(MsrAddress) };
    }

    template<msr_address_t MsrAddress>
    void write_msr(msr_t<MsrAddress> msr_value) noexcept {
        return __writemsr(MsrAddress, msr_value.storage);
    }
}
