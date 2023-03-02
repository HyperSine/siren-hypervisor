#pragma once
#include <stddef.h>
#include <stdint.h>
#include <intrin.h>
#include <type_traits>

namespace siren::x86 {
    // Defined in
    // [*] Volume 3: System Programming Guide
    //  |-> Chapter 3 Protected-Mode Memory Management
    //    |-> 3.4 Logical and Linear Addresses
    //      |-> 3.4.2 Segment Selectors
    struct segment_selector_t {
        union {
            uint16_t storage;
            struct {
                uint16_t rpl : 2;
                uint16_t table_indicator : 1;
                uint16_t index : 13;
            } semantics;
        };
    };

    static_assert(sizeof(segment_selector_t) == 2);
    static_assert(sizeof(segment_selector_t::storage) == sizeof(segment_selector_t::semantics));
    static_assert(std::is_aggregate_v<segment_selector_t>);

    using segment_base32_t = uint32_t;
    using segment_base64_t = uint64_t;

    using segment_limit_t = uint32_t;

    struct segment_access_rights_t {
        union {
            uint32_t storage;
            struct {
                uint32_t reserved0 : 8;
                uint32_t segment_type : 4;
                uint32_t descriptor_type : 1;   // 0 = system, 1 = code or data.
                uint32_t dpl : 2;
                uint32_t present : 1;
                uint32_t reserved1 : 4;
                uint32_t used_by_system_software : 1;
                uint32_t long_mode : 1;
                uint32_t default_operation_size : 1;
                uint32_t granularity : 1;
                uint32_t reserved2 : 8;
            } semantics;
        };
    };

    static_assert(sizeof(segment_access_rights_t) == 4);
    static_assert(sizeof(segment_access_rights_t::storage) == sizeof(segment_access_rights_t::semantics));

    enum class segment_register_e { 
        CS, SS, DS, ES, FS, GS, LDTR, TR
    };

    // Defined in
    // [*] Volume 3: System Programming Guide
    //  |-> Chapter 3 Protected-Mode Memory Management
    //    |-> 3.4 Logical and Linear Addresses
    //      |-> 3.4.5 Segment Descriptors
    //        |-> Figure 3-8. Segment Descriptor
    struct segment_descriptor_long_t {
        union {
            uint64_t storage;
            union {
                struct {
                    uint64_t limit_low : 16;
                    uint64_t base_low : 24;
                    uint64_t accessed : 1;
                    uint64_t readable : 1;
                    uint64_t conforming : 1;
                    uint64_t executable : 1;        // for code segment, always 1 here.
                    uint64_t descriptor_type : 1;   // 0 = system, 1 = code or data; always 1 here.
                    uint64_t dpl : 2;
                    uint64_t present : 1;
                    uint64_t limit_high : 4;
                    uint64_t used_by_system_software : 1;
                    uint64_t long_mode : 1;
                    uint64_t default_operation_size : 1;    // 0 = 16-bit segment, 1 = 32-bit segment
                    uint64_t granularity : 1;
                    uint64_t base_high : 8;

                    [[nodiscard]]
                    constexpr auto base() const noexcept {
                        return static_cast<segment_base32_t>(base_low | base_high << 24u);
                    }

                    [[nodiscard]]
                    constexpr auto limit(bool unscramble = false) const noexcept {
                        auto limit_v = static_cast<segment_limit_t>(limit_low | limit_high << 16u);
                        if (granularity == 1 && unscramble) {
                            return segment_limit_t{ limit_v << 12u | 0xfffu };
                        } else {
                            return limit_v;
                        }
                    }
                } code;
                struct {
                    uint64_t limit_low : 16;
                    uint64_t base_low : 24;
                    uint64_t accessed : 1;
                    uint64_t writable : 1;
                    uint64_t direction : 1;         // 0 = grows up, 1 = grows down
                    uint64_t executable : 1;        // for data segment, always 0 here.
                    uint64_t descriptor_type : 1;   // 0 = system, 1 = code or data; always 1 here.
                    uint64_t dpl : 2;
                    uint64_t present : 1;
                    uint64_t limit_high : 4;
                    uint64_t used_by_system_software : 1;
                    uint64_t reserved : 1;
                    uint64_t default_operation_size : 1;    // 0 = 16-bit segment, 1 = 32-bit segment
                    uint64_t granularity : 1;
                    uint64_t base_high : 8;

                    [[nodiscard]]
                    constexpr auto base() const noexcept {
                        return static_cast<segment_base32_t>(base_low | base_high << 24u);
                    }

                    [[nodiscard]]
                    constexpr auto limit(bool unscramble = false) const noexcept {
                        auto limit_v = static_cast<segment_limit_t>(limit_low | limit_high << 16u);
                        if (granularity == 1 && unscramble) {
                            return segment_limit_t{ limit_v << 12u | 0xfffu };
                        } else {
                            return limit_v;
                        }
                    }
                } data;
                struct {
                    uint64_t flexible0 : 40;
                    uint64_t system_type : 4;
                    uint64_t descriptor_type : 1;   // 0 = system, 1 = code or data; always 0 here.
                    uint64_t dpl : 2;
                    uint64_t present : 1;
                    uint64_t flexible1 : 16;
                } system_half;
            } semantics;
        };
    };

    static_assert(sizeof(segment_descriptor_long_t) == 8);
    static_assert(sizeof(segment_descriptor_long_t::storage) == sizeof(segment_descriptor_long_t::semantics));

    struct segment_descriptor_system_long_t {
        union {
            struct {
                uint64_t data[2];
            } storage;
            union {
                // Defined in
                // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
                //  |-> Chapter 7 Task Management
                //    |-> 7.2 Task Management Data Structures
                //      |-> 7.2.1 TSS Descriptor in 64-bit mode
                //        |-> Figure 7-4. Format of TSS and LDT Descriptors in 64-bit Mode
                struct {
                    uint64_t limit_low : 16;
                    uint64_t base_low : 24;
                    uint64_t system_type : 4;       // for LDT segment descriptor, always 0x2 here.
                    uint64_t descriptor_type : 1;   // 0 = system, 1 = code or data; always 0 here.
                    uint64_t dpl : 2;
                    uint64_t present : 1;
                    uint64_t limit_high : 4;
                    uint64_t used_by_system_software : 1;
                    uint64_t reserved0 : 2;
                    uint64_t granularity : 1;
                    uint64_t base_middle : 8;
                    uint64_t base_high : 32;
                    uint64_t reserved1 : 32;

                    [[nodiscard]]
                    constexpr auto base() const noexcept {
                        return static_cast<segment_base64_t>(base_low | base_middle << 24u | base_high << 32u);
                    }

                    [[nodiscard]]
                    constexpr auto limit(bool unscramble = false) const noexcept {
                        auto limit_v = static_cast<segment_limit_t>(limit_low | limit_high << 16u);
                        if (granularity == 1 && unscramble) {
                            return segment_limit_t{ limit_v << 12u | 0xfffu };
                        } else {
                            return limit_v;
                        }
                    }
                } ldt;

                // Defined in
                // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
                //  |-> Chapter 7 Task Management
                //    |-> 7.2 Task Management Data Structures
                //      |-> 7.2.3 TSS Descriptor in 64-bit mode
                //        |-> Figure 7-4. Format of TSS and LDT Descriptors in 64-bit Mode
                struct {
                    uint64_t limit_low : 16;
                    uint64_t base_low : 24;
                    uint64_t system_type : 4;       // for TSS segment descriptor, 0x9 or 0xB here.
                    uint64_t descriptor_type : 1;   // 0 = system, 1 = code or data; always 0 here.
                    uint64_t dpl : 2;
                    uint64_t present : 1;
                    uint64_t limit_high : 4;
                    uint64_t used_by_system_software : 1;
                    uint64_t reserved0 : 2;
                    uint64_t granularity : 1;
                    uint64_t base_middle : 8;
                    uint64_t base_high : 32;
                    uint64_t reserved1 : 32;

                    [[nodiscard]]
                    constexpr auto base() const noexcept {
                        return static_cast<segment_base64_t>(base_low | base_middle << 24u | base_high << 32u);
                    }

                    [[nodiscard]]
                    constexpr auto limit(bool unscramble = false) const noexcept {
                        auto limit_v = static_cast<segment_limit_t>(limit_low | limit_high << 16u);
                        if (granularity == 1 && unscramble) {
                            return segment_limit_t{ limit_v << 12u | 0xfffu };
                        } else {
                            return limit_v;
                        }
                    }
                } tss;
            } semantics;
        };
    };

    static_assert(sizeof(segment_descriptor_system_long_t) == 16);
    static_assert(sizeof(segment_descriptor_system_long_t::storage) == sizeof(segment_descriptor_system_long_t::semantics));

    struct gate_descriptor_long_t {
        union {
            struct {
                uint64_t data[2];
            } storage;
            union {
                // Defined in
                // [*] Volume 3: System Programming Guide
                //  |-> Chapter 5 Protection
                //    |-> 5.8 Privilege Level Checking When Trasferring Program Control Between Code Segments
                //      |-> 5.8.3 Call Gates
                //        |-> 5.8.3.1 IA-32e Mode Call Gates
                struct {
                    uint64_t offset_low : 16;
                    uint64_t selector : 16;
                    uint64_t reserved0 : 8;
                    uint64_t system_segment_type : 4;   // must be 0xC here.
                    uint64_t descriptor_type : 1;       // 0 = system, 1 = code or data; always 0 here.
                    uint64_t dpl : 2;
                    uint64_t present : 1;
                    uint64_t offset_middle : 16;
                    uint64_t offset_high : 32;
                    uint64_t reserved1 : 32;

                    [[nodiscard]]
                    constexpr auto offset() const noexcept {
                        return static_cast<uint64_t>(offset_low | offset_middle << 16u | offset_high << 32u);
                    }
                } call_gate;

                // Defined in
                // [*] Volume 3: System Programming Guide
                //  |-> Chapter 6 Interrupt and Exception Handling
                //    |-> 6.14 Exception and Interrupt Handling in 64-bit Mode
                //      |-> 6.14.1 64-Bit Mode IDT
                //        |-> Figure 6-8. 64-Bit IDT Gate Descriptors
                struct {
                    uint64_t offset_low : 16;
                    uint64_t selector : 16;
                    uint64_t interrupt_stack_table : 3;
                    uint64_t reserved0 : 5;
                    uint64_t system_segment_type : 4;   // must be 0xE here.
                    uint64_t descriptor_type : 1;       // 0 = system, 1 = code or data; always 0 here.
                    uint64_t dpl : 2;
                    uint64_t present : 1;
                    uint64_t offset_middle : 16;
                    uint64_t offset_high : 32;
                    uint64_t reserved1 : 32;

                    [[nodiscard]]
                    constexpr auto offset() const noexcept {
                        return static_cast<uint64_t>(offset_low | offset_middle << 16u | offset_high << 32u);
                    }
                } interrupt_gate;

                // Defined in
                // [*] Volume 3: System Programming Guide
                //  |-> Chapter 6 Interrupt and Exception Handling
                //    |-> 6.14 Exception and Interrupt Handling in 64-bit Mode
                //      |-> 6.14.1 64-Bit Mode IDT
                //        |-> Figure 6-8. 64-Bit IDT Gate Descriptors
                struct {
                    uint64_t offset_low : 16;
                    uint64_t selector : 16;
                    uint64_t interrupt_stack_table : 3;
                    uint64_t reserved0 : 5;
                    uint64_t system_segment_type : 4;   // must be 0xF here.
                    uint64_t descriptor_type : 1;       // 0 = system, 1 = code or data; always 0 here.
                    uint64_t dpl : 2;
                    uint64_t present : 1;
                    uint64_t offset_middle : 16;
                    uint64_t offset_high : 32;
                    uint64_t reserved1 : 32;

                    [[nodiscard]]
                    constexpr auto offset() const noexcept {
                        return static_cast<uint64_t>(offset_low | offset_middle << 16u | offset_high << 32u);
                    }
                } trap_gate;
            } semantics;
        };
    };

    static_assert(sizeof(gate_descriptor_long_t) == 16);
    static_assert(sizeof(gate_descriptor_long_t::storage) == sizeof(gate_descriptor_long_t::semantics));

    // Defined in
    // [*] Volume 3: System Programming Guide
    //  |-> Chapter 3 Protected-Mode Memory Management
    //    |-> 3.4 Logical and Linear Addresses
    //      |-> 3.4.5 Segment Descriptors
    //        |-> Figure 3-8. Segment Descriptor
    struct segment_descriptor_legacy_t {
        union {
            uint64_t storage;
            union {
                struct {
                    uint64_t limit_low : 16;
                    uint64_t base_low : 24;
                    uint64_t accessed : 1;
                    uint64_t readable : 1;
                    uint64_t conforming : 1;
                    uint64_t executable : 1;        // for code segment, always 1 here.
                    uint64_t descriptor_type : 1;   // 0 = system, 1 = code or data; always 1 here.
                    uint64_t dpl : 2;
                    uint64_t present : 1;
                    uint64_t limit_high : 4;
                    uint64_t used_by_system_software : 1;
                    uint64_t reserved : 1;
                    uint64_t default_operation_size : 1;    // 0 = 16-bit segment, 1 = 32-bit segment
                    uint64_t granularity : 1;
                    uint64_t base_high : 8;

                    [[nodiscard]]
                    constexpr auto base() const noexcept {
                        return static_cast<segment_base32_t>(base_low | base_high << 24u);
                    }

                    [[nodiscard]]
                    constexpr auto limit(bool unscramble = false) const noexcept {
                        auto limit_v = static_cast<segment_limit_t>(limit_low | limit_high << 16u);
                        if (granularity == 1 && unscramble) {
                            return segment_limit_t{ limit_v << 12u | 0xfffu };
                        } else {
                            return limit_v;
                        }
                    }
                } code;
                struct {
                    uint64_t limit_low : 16;
                    uint64_t base_low : 24;
                    uint64_t accessed : 1;
                    uint64_t writable : 1;
                    uint64_t direction : 1;         // 0 = grows up, 1 = grows down
                    uint64_t executable : 1;        // for data segment, always 0 here.
                    uint64_t descriptor_type : 1;   // 0 = system, 1 = code or data; always 1 here.
                    uint64_t dpl : 2;
                    uint64_t present : 1;
                    uint64_t limit_high : 4;
                    uint64_t used_by_system_software : 1;
                    uint64_t reserved : 1;
                    uint64_t default_operation_size : 1;    // 0 = 16-bit segment, 1 = 32-bit segment
                    uint64_t granularity : 1;
                    uint64_t base_high : 8;

                    [[nodiscard]]
                    constexpr auto base() const noexcept {
                        return static_cast<segment_base32_t>(base_low | base_high << 24u);
                    }

                    [[nodiscard]]
                    constexpr auto limit(bool unscramble = false) const noexcept {
                        auto limit_v = static_cast<segment_limit_t>(limit_low | limit_high << 16u);
                        if (granularity == 1 && unscramble) {
                            return segment_limit_t{ limit_v << 12u | 0xfffu };
                        } else {
                            return limit_v;
                        }
                    }
                } data;
                union {
                    // not explained explicitly, but should be the same with TSS
                    struct {
                        uint64_t limit_low : 16;
                        uint64_t base_low : 24;
                        uint64_t system_type : 4;       // always 0x2 here.
                        uint64_t descriptor_type : 1;   // 0 = system, 1 = code or data; always 0 here.
                        uint64_t dpl : 2;
                        uint64_t present : 1;
                        uint64_t limit_high : 4;
                        uint64_t used_by_system_software : 1;
                        uint64_t reserved0 : 2;
                        uint64_t granularity : 1;
                        uint64_t base_high : 8;

                        [[nodiscard]]
                        constexpr auto base() const noexcept {
                            return static_cast<segment_base32_t>(base_low | base_high << 24u);
                        }

                        [[nodiscard]]
                        constexpr auto limit(bool unscramble = false) const noexcept {
                            auto limit_v = static_cast<segment_limit_t>(limit_low | limit_high << 16u);
                            if (granularity == 1 && unscramble) {
                                return segment_limit_t{ limit_v << 12u | 0xfffu };
                            } else {
                                return limit_v;
                            }
                        }
                    } ldt;

                    // Defined in
                    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
                    //  |-> Chapter 7 Task Management
                    //    |-> 7.2 Task Management Data Structures
                    //      |-> 7.2.2 TSS Descriptor
                    //        |-> Figure 7-3. TSS Descriptor
                    struct {
                        uint64_t limit_low : 16;
                        uint64_t base_low : 24;
                        uint64_t system_type : 4;       // 0x1 = 16-bits available, 0x3 = 16-bits busy, 0x9 = 32-bits available, 0xB = 32-bits busy.
                        uint64_t descriptor_type : 1;   // 0 = system, 1 = code or data; always 0 here.
                        uint64_t dpl : 2;
                        uint64_t present : 1;
                        uint64_t limit_high : 4;
                        uint64_t used_by_system_software : 1;
                        uint64_t reserved0 : 2;
                        uint64_t granularity : 1;
                        uint64_t base_high : 8;

                        [[nodiscard]]
                        constexpr auto base() const noexcept {
                            return static_cast<segment_base32_t>(base_low | (base_high << 24u));
                        }

                        [[nodiscard]]
                        constexpr auto limit(bool unscramble = false) const noexcept {
                            auto limit_v = static_cast<segment_limit_t>(limit_low | limit_high << 16u);
                            if (granularity == 1 && unscramble) {
                                return segment_limit_t{ limit_v << 12u | 0xfffu };
                            } else {
                                return limit_v;
                            }
                        }
                    } tss;
                } system;
            } semantics;
        };
    };

    static_assert(sizeof(segment_descriptor_legacy_t) == 8);
    static_assert(sizeof(segment_descriptor_legacy_t::storage) == sizeof(segment_descriptor_legacy_t::semantics));

    struct gate_descriptor_legacy_t {
        union {
            uint64_t storage;
            union {
                // Defined in
                // [*] Volume 3: System Programming Guide
                //  |-> Chapter 5 Protection
                //    |-> 5.8 Privilege Level Checking When Trasferring Program Control Between Code Segments
                //      |-> 5.8.3 Call Gates
                //        |-> Figure 5-8. Call-Gate Descriptor
                struct {
                    uint64_t offset_low : 16;
                    uint64_t selector : 16;
                    uint64_t parameter_count : 5;
                    uint64_t reserved0 : 3;
                    uint64_t system_segment_type : 4;   // 0x4 = 16-bits call gate, 0xC = 32-bits call gate.
                    uint64_t descriptor_type : 1;       // 0 = system, 1 = code or data; always 0 here.
                    uint64_t dpl : 2;
                    uint64_t present : 1;
                    uint64_t offset_high : 16;

                    [[nodiscard]]
                    constexpr auto offset() const noexcept {
                        return static_cast<uint32_t>(offset_low | offset_high << 16u);
                    }
                } call_gate;

                // Defined in
                // [*] Volume 3: System Programming Guide
                //  |-> Chapter 6 Interrupt and Exception Handling
                //    |-> 6.11 IDT Descriptors
                //      |-> Figure 6-2. IDT Gate Descriptors
                struct {
                    uint64_t offset_low : 16;
                    uint64_t selector : 16;
                    uint64_t reserved0 : 8;
                    uint64_t system_segment_type : 4;   // 0x6 = 16-bits gate, 0xE = 32-bits gate.
                    uint64_t descriptor_type : 1;       // 0 = system, 1 = code or data; always 0 here.
                    uint64_t dpl : 2;
                    uint64_t present : 1;
                    uint64_t offset_high : 16;

                    [[nodiscard]]
                    constexpr auto offset() const noexcept {
                        return static_cast<uint32_t>(offset_low | offset_high << 16u);
                    }
                } interrupt_gate;

                // Defined in
                // [*] Volume 3: System Programming Guide
                //  |-> Chapter 6 Interrupt and Exception Handling
                //    |-> 6.11 IDT Descriptors
                //      |-> Figure 6-2. IDT Gate Descriptors
                struct {
                    uint64_t offset_low : 16;
                    uint64_t selector : 16;
                    uint64_t reserved0 : 8;
                    uint64_t system_segment_type : 4;   // 0x7 = 16-bits gate, 0xF = 32-bits gate.
                    uint64_t descriptor_type : 1;       // 0 = system, 1 = code or data; always 0 here.
                    uint64_t dpl : 2;
                    uint64_t present : 1;
                    uint64_t offset_high : 16;

                    [[nodiscard]]
                    constexpr auto offset() const noexcept {
                        return static_cast<uint32_t>(offset_low | offset_high << 16u);
                    }
                } trap_gate;

                // Defined in
                // [*] Volume 3: System Programming Guide
                //  |-> Chapter 6 Interrupt and Exception Handling
                //    |-> 6.11 IDT Descriptors
                //      |-> Figure 6-2. IDT Gate Descriptors
                struct {
                    uint64_t reserved0 : 16;
                    uint64_t selector : 16;
                    uint64_t reserved1 : 8;
                    uint64_t system_segment_type : 4;   // always 0x5 here.
                    uint64_t descriptor_type : 1;       // 0 = system, 1 = code or data; always 0 here.
                    uint64_t dpl : 2;
                    uint64_t present : 1;
                    uint64_t reserved2 : 16;
                } task_gate;
            } semantics;
        };
    };

    static_assert(sizeof(gate_descriptor_legacy_t) == 8);
    static_assert(sizeof(gate_descriptor_legacy_t::storage) == sizeof(gate_descriptor_legacy_t::semantics));

#if defined(_M_X64)
    using segment_descriptor_t = segment_descriptor_long_t;
#elif defined(_M_IX86)
    using segment_descriptor_t = segment_descriptor_legacy_t;
#endif

    // Defined in
    // [*] Volume 3: System Programming Guide
    //  |-> Chapter 3 Protected-Mode Memory Management
    //    |-> 3.5 System Descriptor Types
    //      |-> 3.5.1 Segment Descriptor Tables
    //        |-> Figure 3-11. Pseudo-Descriptor Formats
    struct pseudo_descriptor_legacy_t {
        union {
            struct {
                uint8_t padding[2];
                uint8_t data[2 + 4];
            } storage;
            struct {
                uint8_t padding[2];
                uint16_t limit;
                uint32_t base;
            } semantics;
        };
    };

    static_assert(alignof(pseudo_descriptor_legacy_t) == 4);
    static_assert(sizeof(pseudo_descriptor_legacy_t::storage) == 8);
    static_assert(sizeof(pseudo_descriptor_legacy_t::storage) == sizeof(pseudo_descriptor_legacy_t::semantics));
    static_assert(offsetof(pseudo_descriptor_legacy_t, semantics.limit) == 2);
    static_assert(offsetof(pseudo_descriptor_legacy_t, semantics.base) == 4);

    // Defined in
    // [*] Volume 3: System Programming Guide
    //  |-> Chapter 3 Protected-Mode Memory Management
    //    |-> 3.5 System Descriptor Types
    //      |-> 3.5.1 Segment Descriptor Tables
    //        |-> Figure 3-11. Pseudo-Descriptor Formats
    struct pseudo_descriptor_long_t {
        union {
            struct {
                uint8_t padding[6];
                uint8_t data[2 + 8];
            } storage;
            struct {
                uint8_t padding[6];
                uint16_t limit;
                uint64_t base;
            } semantics;
        };
    };

    static_assert(alignof(pseudo_descriptor_long_t) == 8);
    static_assert(sizeof(pseudo_descriptor_long_t::storage) == 16);
    static_assert(sizeof(pseudo_descriptor_long_t::storage) == sizeof(pseudo_descriptor_long_t::semantics));
    static_assert(offsetof(pseudo_descriptor_long_t, semantics.limit) == 6);
    static_assert(offsetof(pseudo_descriptor_long_t, semantics.base) == 8);

#if defined(_M_X64)
    using gdtr_t = pseudo_descriptor_long_t;
    using idtr_t = pseudo_descriptor_long_t;
#elif defined(_M_IX86)
    using gdtr_t = pseudo_descriptor_legacy_t;
    using idtr_t = pseudo_descriptor_legacy_t;
#endif

    template<segment_register_e SegmentReg>
    [[nodiscard]]
    segment_selector_t read_segment_selector() noexcept;

    template<segment_register_e SegmentReg>
    [[nodiscard]]
    void write_segment_selector(segment_selector_t new_selector) noexcept;

    // return 0xfffff000 if failed
    [[nodiscard]]
    segment_limit_t read_segment_unscrambled_limit(segment_selector_t selector) noexcept;

    [[nodiscard]]
    segment_access_rights_t read_segment_access_rights(segment_selector_t selector) noexcept;

    [[nodiscard]]
    inline gdtr_t read_gdtr() noexcept {
        gdtr_t gdtr = {};
        _sgdt(&gdtr.storage.data);
        return gdtr;
    }

    [[nodiscard]]
    inline idtr_t read_idtr() noexcept {
        idtr_t idtr = {};
        __sidt(&idtr.storage.data);
        return idtr;
    }

    inline void write_gdtr(gdtr_t new_gdtr) noexcept {
        _lgdt(&new_gdtr.storage.data);
    }

    inline void write_idtr(gdtr_t new_idtr) noexcept {
        __lidt(&new_idtr.storage.data);
    }
}
