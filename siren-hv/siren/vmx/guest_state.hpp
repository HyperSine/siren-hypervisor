#pragma once
#include "../../siren_global.hpp"
#include "../../x86/flags_register.hpp"
#include "../../x86/simd_registers.hpp"

namespace siren::hypervisors::vmx {

    struct guest_registers_t {
        //union { struct { uint8_t al; uint8_t ah; }; uint16_t ax; uint32_t eax; uint64_t rax; };
        //union { struct { uint8_t cl; uint8_t ch; }; uint16_t cx; uint32_t ecx; uint64_t rcx; };
        //union { struct { uint8_t dl; uint8_t dh; }; uint16_t dx; uint32_t edx; uint64_t rdx; };
        //union { struct { uint8_t bl; uint8_t bh; }; uint16_t bx; uint32_t ebx; uint64_t rbx; };
        //union { struct { uint8_t spl; uint8_t sph; }; uint16_t sp; uint32_t esp; uint64_t rsp; };
        //union { struct { uint8_t bpl; uint8_t bph; }; uint16_t bp; uint32_t ebp; uint64_t rbp; };
        //union { struct { uint8_t sil; uint8_t sih; }; uint16_t si; uint32_t esi; uint64_t rsi; };
        //union { struct { uint8_t dil; uint8_t dih; }; uint16_t di; uint32_t edi; uint64_t rdi; };
        //union { uint8_t r8b; uint16_t r8w; uint32_t r8d; uint64_t r8; };
        //union { uint8_t r9b; uint16_t r9w; uint32_t r9d; uint64_t r9; };
        //union { uint8_t r10b; uint16_t r10w; uint32_t r10d; uint64_t r10; };
        //union { uint8_t r11b; uint16_t r11w; uint32_t r11d; uint64_t r11; };
        //union { uint8_t r12b; uint16_t r12w; uint32_t r12d; uint64_t r12; };
        //union { uint8_t r13b; uint16_t r13w; uint32_t r13d; uint64_t r13; };
        //union { uint8_t r14b; uint16_t r14w; uint32_t r14d; uint64_t r14; };
        //union { uint8_t r15b; uint16_t r15w; uint32_t r15d; uint64_t r15; };
        //union { uint16_t ip; uint32_t eip; uint64_t rip; };
        //union { x86::eflags_t eflags; x86::rflags_t rflags; };
        uint64_t rax;
        uint64_t rcx;
        uint64_t rdx;
        uint64_t rbx;
        uint64_t rsp;
        uint64_t rbp;
        uint64_t rsi;
        uint64_t rdi;
        uint64_t r8;
        uint64_t r9;
        uint64_t r10;
        uint64_t r11;
        uint64_t r12;
        uint64_t r13;
        uint64_t r14;
        uint64_t r15;
        uint64_t rip;
        x86::rflags_t rflags;
        x86::xmm_t xmm0;
        x86::xmm_t xmm1;
        x86::xmm_t xmm2;
        x86::xmm_t xmm3;
        x86::xmm_t xmm4;
        x86::xmm_t xmm5;
        x86::xmm_t xmm6;
        x86::xmm_t xmm7;
        x86::xmm_t xmm8;
        x86::xmm_t xmm9;
        x86::xmm_t xmm10;
        x86::xmm_t xmm11;
        x86::xmm_t xmm12;
        x86::xmm_t xmm13;
        x86::xmm_t xmm14;
        x86::xmm_t xmm15;
    };

    static_assert(alignof(guest_registers_t) == alignof(x86::xmm_t));

}
