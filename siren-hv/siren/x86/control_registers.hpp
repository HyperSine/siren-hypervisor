#pragma once
#include <stdint.h>
#include <intrin.h>

namespace siren::x86 {
    // Defined in 
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 2 System Architecture Overview
    //    |-> 2.5 Control Registers
    //      |-> Figure 2-7. Control Registers
    struct cr0_legacy_t {
        union {
            uint32_t storage;
            struct {
                uint32_t protection_enable : 1;
                uint32_t monitor_coprocessor : 1;
                uint32_t x87_fpu_emulation : 1;
                uint32_t task_switched : 1;
                uint32_t extension_type : 1;
                uint32_t x87_fpu_numeric_error : 1;
                uint32_t reserved0 : 10;
                uint32_t write_protect : 1;
                uint32_t reserved1 : 1;
                uint32_t alignment_mask : 1;
                uint32_t reserved2 : 10;
                uint32_t not_write_through : 1;
                uint32_t cache_disable : 1;
                uint32_t paging : 1;
            } semantics;
        };
    };

    static_assert(sizeof(cr0_legacy_t) == 4);
    static_assert(sizeof(cr0_legacy_t::storage) == sizeof(cr0_legacy_t::semantics));

    // Defined in 
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 2 System Architecture Overview
    //    |-> 2.5 Control Registers
    //      |-> Figure 2-7. Control Registers
    struct cr0_long_t{
        union {
            uint64_t storage;
            struct {
                uint64_t protection_enable : 1;
                uint64_t monitor_coprocessor : 1;
                uint64_t x87_fpu_emulation : 1;
                uint64_t task_switched : 1;
                uint64_t extension_type : 1;
                uint64_t x87_fpu_numeric_error : 1;
                uint64_t reserved0 : 10;
                uint64_t write_protect : 1;
                uint64_t reserved1 : 1;
                uint64_t alignment_mask : 1;
                uint64_t reserved2 : 10;
                uint64_t not_write_through : 1;
                uint64_t cache_disable : 1;
                uint64_t paging : 1;
                uint64_t reserved3 : 32;
            } semantics;
        };
    };

    static_assert(sizeof(cr0_long_t) == 8);
    static_assert(sizeof(cr0_long_t::storage) == sizeof(cr0_long_t::semantics));

#if defined(_M_X64)
    using cr0_t = cr0_long_t;
#elif defined(_M_IX86)
    using cr0_t = cr0_legacy_t;
#endif

    // Defined in 
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 2 System Architecture Overview
    //    |-> 2.5 Control Registers
    //      |-> Figure 2-7. Control Registers
    struct cr2_legacy_t {
        union {
            uint32_t storage;
            struct {
                uint32_t page_fault_linear_address;
            } semantics;
        };
    };

    static_assert(sizeof(cr2_legacy_t) == 4);
    static_assert(sizeof(cr2_legacy_t::storage) == sizeof(cr2_legacy_t::semantics));

    // Defined in 
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 2 System Architecture Overview
    //    |-> 2.5 Control Registers
    //      |-> Figure 2-7. Control Registers
    struct cr2_long_t {
        union {
            uint64_t storage;
            struct {
                uint64_t page_fault_linear_address;
            } semantics;
        };
    };

    static_assert(sizeof(cr2_long_t) == 8);
    static_assert(sizeof(cr2_long_t::storage) == sizeof(cr2_long_t::semantics));

#if defined(_M_X64)
    using cr2_t = cr2_long_t;
#elif defined(_M_IX86)
    using cr2_t = cr2_legacy_t;
#endif

    // Defined in 
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 2 System Architecture Overview
    //    |-> 2.5 Control Registers
    //      |-> Figure 2-7. Control Registers
    struct cr3_legacy_t {
        union {
            uint32_t storage;
            struct {
                uint32_t reserved0 : 3;
                uint32_t page_level_write_through : 1;
                uint32_t page_level_cache_disable : 1;
                uint32_t reserved1 : 7;
                uint32_t page_directory_base : 20;
            } semantics;
        };
    };

    static_assert(sizeof(cr3_legacy_t) == 4);
    static_assert(sizeof(cr3_legacy_t::storage) == sizeof(cr3_legacy_t::semantics));

    // Defined in 
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 4 Paging
    //    |-> 4.4 PAE Paging
    //      |-> 4.4.1 PDPTE Registers
    //        |-> Table 4-7. Use of CR3 with PAE Paging
    struct cr3_pae_legacy_t {
        union {
            uint32_t storage;
            struct {
                uint32_t reserved : 5;
                uint32_t page_directory_base : 27;  // Beware! This is 32-bytes aligned, not 4kb aligned.
            } semantics;
        };
    };

    static_assert(sizeof(cr3_pae_legacy_t) == 4);
    static_assert(sizeof(cr3_pae_legacy_t::storage) == sizeof(cr3_pae_legacy_t::semantics));

    // Defined in 
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 2 System Architecture Overview
    //    |-> 2.5 Control Registers
    //      |-> Figure 2-7. Control Registers
    struct cr3_long_t {
        union {
            uint64_t storage;
            struct {
                uint64_t reserved0 : 3;
                uint64_t page_level_write_through : 1;
                uint64_t page_level_cache_disable : 1;
                uint64_t reserved1 : 7;
                uint64_t page_directory_base : 40;
                uint64_t reserved2 : 12;
            } semantics;
        };
    };

    static_assert(sizeof(cr3_long_t) == 8);
    static_assert(sizeof(cr3_long_t::storage) == sizeof(cr3_long_t::semantics));

    // Defined in 
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 4 Paging
    //    |-> 4.5 4-Level Paging and 5-Level Paging
    //      |-> 4.5.2 Use of CR3 with Ordinary 4-Level Paging and 5-Level Paging
    //        |-> Table 4-13. Use of CR3 with 4-Level Paging and 5-Level Paging and CR4.PCIDE = 1
    struct cr3_pcid_long_t {
        union {
            uint64_t storage;
            struct {
                uint64_t pcid : 12;
                uint64_t page_directory_base : 40;
                uint64_t reserved : 12;
            } semantics;
        };
    };

    static_assert(sizeof(cr3_pcid_long_t) == 8);
    static_assert(sizeof(cr3_pcid_long_t::storage) == sizeof(cr3_pcid_long_t::semantics));

#if defined(_M_X64)
    using cr3_t = cr3_long_t;
#elif defined(_M_IX86)
    using cr3_t = cr3_legacy_t;
#endif

    // Defined in 
    // [*] Volume 3: System Programming Guide
    //  |-> Chapter 2 System Architecture Overview
    //    |-> 2.5 Control Registers
    //      |-> Figure 2-7. Control Registers
    struct cr4_legacy_t {
        union {
            uint32_t storage;
            struct {
                uint32_t virtual8086_mode_extension : 1;
                uint32_t protected_mode_virtual_interrupts : 1;
                uint32_t time_stamp_disable : 1;
                uint32_t debugging_extension : 1;
                uint32_t page_size_extension : 1;
                uint32_t physical_address_extension : 1;
                uint32_t machine_check_enable : 1;
                uint32_t page_global_enable : 1;
                uint32_t performance_monitoring_counter_enable : 1;
                uint32_t os_support_for_fxsave_and_fxrstor : 1;
                uint32_t os_support_for_unmasked_xmm_exception : 1;
                uint32_t user_mode_instruction_prevention : 1;
                uint32_t reserved0 : 1;
                uint32_t vmx_enable : 1;
                uint32_t smx_enable : 1;
                uint32_t reserved1 : 1;
                uint32_t fsgsbase_instruction_enable : 1;
                uint32_t pcid_enable : 1;
                uint32_t os_support_for_xsave : 1;
                uint32_t key_locker_enable : 1;
                uint32_t supervisor_mode_execution_prevention : 1;
                uint32_t supervisor_mode_access_prevention : 1;
                uint32_t enable_protection_keys_for_user_mode_pages : 1;
                uint32_t control_flow_enforcement : 1;
                uint32_t enable_protection_keys_for_supervisor_mode_pages : 1;
                uint32_t reserved2 : 7;
            } semantics;
        };
    };

    static_assert(sizeof(cr4_legacy_t) == 4);
    static_assert(sizeof(cr4_legacy_t::storage) == sizeof(cr4_legacy_t::semantics));

    // Defined in 
    // [*] Volume 3: System Programming Guide
    //  |-> Chapter 2 System Architecture Overview
    //    |-> 2.5 Control Registers
    //      |-> Figure 2-7. Control Registers
    struct cr4_long_t {
        union {
            uint64_t storage;
            struct {
                uint64_t virtual8086_mode_extension : 1;
                uint64_t protected_mode_virtual_interrupts : 1;
                uint64_t time_stamp_disable : 1;
                uint64_t debugging_extension : 1;
                uint64_t page_size_extension : 1;
                uint64_t physical_address_extension : 1;
                uint64_t machine_check_enable : 1;
                uint64_t page_global_enable : 1;
                uint64_t performance_monitoring_counter_enable : 1;
                uint64_t os_support_for_fxsave_and_fxrstor : 1;
                uint64_t os_support_for_unmasked_xmm_exception : 1;
                uint64_t user_mode_instruction_prevention : 1;
                uint64_t linear_address_has_57bits : 1;
                uint64_t vmx_enable : 1;
                uint64_t smx_enable : 1;
                uint64_t reserved0 : 1;
                uint64_t fsgsbase_instruction_enable : 1;
                uint64_t pcid_enable : 1;
                uint64_t os_support_for_xsave : 1;
                uint64_t key_locker_enable : 1;
                uint64_t supervisor_mode_execution_prevention : 1;
                uint64_t supervisor_mode_access_prevention : 1;
                uint64_t enable_protection_keys_for_user_mode_pages : 1;
                uint64_t control_flow_enforcement : 1;
                uint64_t enable_protection_keys_for_supervisor_mode_pages : 1;
                uint64_t reserved2 : 39;
            } semantics;
        };
    };

    static_assert(sizeof(cr4_long_t) == 8);
    static_assert(sizeof(cr4_long_t::storage) == sizeof(cr4_long_t::semantics));

#if defined(_M_X64)
    using cr4_t = cr4_long_t;
#elif defined(_M_IX86)
    using cr4_t = cr4_legacy_t;
#endif

    // Defined in 
    // [*] Volume 3: System Programming Guide
    //  |-> Chapter 2: System Architecture Overview
    //    |-> 2.5 Control Registers
    struct cr8_long_t {
        union {
            uint64_t storage;
            struct {
                uint64_t task_priority_level : 4;
                uint64_t reserved0 : 60;
            } semantics;
        };
    };

    static_assert(sizeof(cr8_long_t) == 8);
    static_assert(sizeof(cr8_long_t::storage) == sizeof(cr8_long_t::semantics));

#if defined(_M_X64)
    using cr8_t = cr8_long_t;
#endif

    template<typename Ty = cr0_t>
    [[nodiscard]]
    Ty read_cr0() noexcept {
        return Ty{ .storage = __readcr0() };
    }

    template<typename Ty = cr0_t>
    void write_cr0(Ty cr0_value) noexcept {
        return __writecr0(cr0_value.storage);
    }

    template<typename Ty = cr2_t>
    [[nodiscard]]
    Ty read_cr2() noexcept {
        return Ty{ .storage = __readcr2() };
    }

    template<typename Ty = cr2_t>
    void write_cr2(Ty cr2_value) noexcept {
        return __writecr2(cr2_value.storage);
    }

    template<typename Ty = cr3_t>
    [[nodiscard]]
    Ty read_cr3() noexcept {
        return Ty{ .storage = __readcr3() };
    }

    template<typename Ty = cr3_t>
    void write_cr3(Ty cr3_value) noexcept {
        return __writecr3(cr3_value.storage);
    }

    template<typename Ty = cr4_t>
    [[nodiscard]]
    Ty read_cr4() noexcept {
        return Ty{ .storage = __readcr4() };
    }

    template<typename Ty = cr4_t>
    void write_cr4(Ty cr4_value) noexcept {
        return __writecr4(cr4_value.storage);
    }

#if defined(_M_X64)
    template<typename Ty = cr8_t>
    [[nodiscard]]
    Ty read_cr8() noexcept {
        return Ty{ .storage = __readcr8() };
    }

    template<typename Ty = cr8_t>
    void write_cr8(Ty cr8_value) noexcept {
        return __writecr8(cr8_value.storage);
    }
#endif
}
