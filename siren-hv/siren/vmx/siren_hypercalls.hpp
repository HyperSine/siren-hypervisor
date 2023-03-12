#pragma once
#include <stdint.h>
#include "../nt_status.hpp"

namespace siren::vmx {
    //
    // eax <= 'vhrs'
    // rbx <= function id
    // rcx, rdx, rsi, rdi, r8, r9, ...
    //
    namespace siren_hypercalls {
        // function id: 0
        [[nodiscard]]
        uint32_t echo() noexcept;

        // function id: 1
        [[nodiscard]]
        void turn_off_vm() noexcept;

        // function id: 2
        [[nodiscard]]
        nt_status ept_commit_1gb_page(uint64_t guest_page_base, uint64_t host_page_base, uint32_t flags) noexcept;

        // function id: 3
        [[nodiscard]]
        nt_status ept_commit_2mb_page(uint64_t guest_page_base, uint64_t host_page_base, uint32_t flags) noexcept;

        // function id: 4
        [[nodiscard]]
        nt_status ept_commit_4kb_page(uint64_t guest_page_base, uint64_t host_page_base, uint32_t flags) noexcept;

        // function id: 5
        [[nodiscard]]
        nt_status ept_uncommit_1gb_page(uint64_t guest_page_base) noexcept;

        // function id: 6
        [[nodiscard]]
        nt_status ept_uncommit_2mb_page(uint64_t guest_page_base) noexcept;

        // function id: 7
        [[nodiscard]]
        nt_status ept_uncommit_4kb_page(uint64_t guest_page_base) noexcept;

        // function id: 8
        [[nodiscard]]
        void ept_flush() noexcept;
    };
}
