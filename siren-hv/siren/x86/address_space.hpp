#pragma once
#include <stdint.h>

namespace siren::x86 {
    using paddr_t = uint64_t;

    using laddr32_t = uint32_t;     // 32-bits linear address
    using laddr64_t = uint64_t;     // 64-bits linear address

    using lgaddr_t = uintptr_t;     // logical address
    using lgaddr32_t = uint32_t;    // 32-bits logical address
    using lgaddr64_t = uint64_t;    // 64-bits logical address

    using vaddr_t = uintptr_t;      // virtual address, Aka. logical address
    using vaddr32_t = uint32_t;     // 32-bits virtual address
    using vaddr64_t = uint64_t;     // 64-bits virtual address
}
