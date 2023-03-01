#pragma once
#include "../siren_global.hpp"

namespace siren::x86 {

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 6 Interrupt and Exception Handling
    //    |-> 6.2 Exception and Interrupt Vectors
    //      |-> Table 6-1. Protected-Mode Exceptions and Interrupts
    enum class interrupt_e : uint8_t{
        FAULT_DEVIDE_ERROR = 0,
        FAULT_TRAP_DEBUG_EXCEPTION = 1,
        INTR_NMI_INTERRUPT = 2,
        TRAP_BREAKPOINT = 3,
        TRAP_OVERFLOW = 4,
        FAULT_BOUND_RANGE_EXCEEDED = 5,
        FAULT_UNDEFINED_OPCODE = 6,
        FAULT_NO_MATH_COPROCESSOR = 7,
        ABORT_DOUBLE_FAULT = 8,
        FAULT_COPROCESSOR_SEGMENT_OVERRUN = 9, // reserved, processors after the Intel386 processor do not generate this exception.
        FAULT_INVALID_TSS = 10,
        FAULT_SEGMENT_NOT_PRESENT = 11,
        FAULT_STACK_SEGMENT_FAULT = 12,
        FAULT_GENERAL_PROTECTION = 13,
        FAULT_PAGE_FAULT = 14,
        INTEL_RESERVED_15 = 15,
        FAULT_X87_FP_ERROR = 16,
        FAULT_ALIGNMENT_CHECK = 17,
        ABORT_MACHINE_CHECK = 18,
        FAULT_SIMD_FP_EXCEPTION = 19,
        FAULT_VIRTUALIZATION_EXCEPTION = 20,
        FAULT_CONTROL_PROTECTION_EXCEPTION = 21,
        INTEL_RESERVED_22 = 22,
        INTEL_RESERVED_23 = 23,
        INTEL_RESERVED_24 = 24,
        INTEL_RESERVED_25 = 25,
        INTEL_RESERVED_26 = 26,
        INTEL_RESERVED_27 = 27,
        INTEL_RESERVED_28 = 28,
        INTEL_RESERVED_29 = 29,
        INTEL_RESERVED_30 = 30,
        INTEL_RESERVED_31 = 31,
        // the laters are user defined interrupts
    };

}
