#include <ntddk.h>
#include "paging.hpp"
#include "cpuid.hpp"
#include "multiprocessor.hpp"

namespace siren::x86 {

    [[nodiscard]]
    physical_address_t get_physical_address(virtual_address_t address) noexcept {
        return MmGetPhysicalAddress(reinterpret_cast<void*>(address)).QuadPart;
    }

    [[nodiscard]]
    physical_address_t get_max_physical_address() noexcept {
        static std::atomic_int once_flag;
        static physical_address_t max_physical_address;

        cpu_call_once(
            once_flag, 
            []() noexcept {
                // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
                //  |-> Chapter 4 Paging
                //    |-> 4.1 Paging Modes and Control Bits
                //      |-> 4.1.4 Enumeration of Paging Features by CPUID
                // 
                // CPUID.80000008H:EAX[7:0] reports the physical-address width supported by the processor. (For processors
                // that do not support CPUID function 80000008H, the width is generally 36 if CPUID.01H:EDX.PAE [bit 6] = 1
                // and 32 otherwise.) This width is referred to as MAXPHYADDR. MAXPHYADDR is at most 52.
                if (0x80000008u <= cpuid<0x80000000u>().semantics.max_leaf_for_extended_cpuid_information) {
                    max_physical_address = (physical_address_t{ 1 } << cpuid<0x80000008u>().semantics.physical_address_bits) - 1;
                } else if (cpuid<0x1>().semantics.edx.physical_address_extension == 1) {
                    max_physical_address = (physical_address_t{ 1 } << 36u) - 1;
                } else {
                    max_physical_address = (physical_address_t{ 1 } << 32u) - 1;
                }
            }
        );

        return max_physical_address;
    }

    [[nodiscard]]
    virtual_address_t get_virtual_address(physical_address_t address) noexcept {
        return reinterpret_cast<uintptr_t>(MmGetVirtualForPhysical(PHYSICAL_ADDRESS{ .QuadPart = static_cast<decltype(PHYSICAL_ADDRESS::QuadPart)>(address) }));
    }

}
