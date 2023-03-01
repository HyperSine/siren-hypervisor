#pragma once
#include "../siren_global.hpp"

namespace siren::x86 {
    struct eflags_t {
        union {
            uint32_t storage;
            struct {
                uint32_t carry_flag : 1;
                uint32_t always_one : 1;
                uint32_t parity_flag : 1;
                uint32_t reserved0 : 1;
                uint32_t auxiliary_carry_flag : 1;
                uint32_t reserved1 : 1;
                uint32_t zero_flag : 1;
                uint32_t sign_flag : 1;
                uint32_t trap_flag : 1;
                uint32_t interrupt_enable_flag : 1;
                uint32_t direction_flag : 1;
                uint32_t overflow_flag : 1;
                uint32_t io_privilege_level : 2;
                uint32_t nested_task : 1;
                uint32_t reserved2 : 1;
                uint32_t resume_flag : 1;
                uint32_t virtual8086_mode : 1;
                uint32_t alignment_check_or_access_control : 1;
                uint32_t virtual_interrupt_flag : 1;
                uint32_t virtual_interrupt_pending : 1;
                uint32_t id_flag : 1;
                uint32_t reserved3 : 10;
            } semantics;
        };
    };

    static_assert(sizeof(eflags_t) == 4);
    static_assert(sizeof(eflags_t::storage) == sizeof(eflags_t::semantics));

    struct rflags_t {
        union {
            uint64_t storage;
            struct {
                uint32_t carry_flag : 1;
                uint32_t always_one : 1;
                uint32_t parity_flag : 1;
                uint32_t reserved0 : 1;
                uint32_t auxiliary_carry_flag : 1;
                uint32_t reserved1 : 1;
                uint32_t zero_flag : 1;
                uint32_t sign_flag : 1;
                uint32_t trap_flag : 1;
                uint32_t interrupt_enable_flag : 1;
                uint32_t direction_flag : 1;
                uint32_t overflow_flag : 1;
                uint32_t io_privilege_level : 2;
                uint32_t nested_task : 1;
                uint32_t reserved2 : 1;
                uint32_t resume_flag : 1;
                uint32_t virtual8086_mode : 1;
                uint32_t alignment_check_or_access_control : 1;
                uint32_t virtual_interrupt_flag : 1;
                uint32_t virtual_interrupt_pending : 1;
                uint32_t id_flag : 1;
                uint32_t reserved3 : 10;
                uint32_t reserved4 : 32;
            } semantics;
        };
    };

    static_assert(sizeof(rflags_t) == 8);
    static_assert(sizeof(rflags_t::storage) == sizeof(rflags_t::semantics));
}
