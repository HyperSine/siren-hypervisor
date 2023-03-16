#include "paging.hpp"
#include "cpuid.hpp"
#include "../synchronization.hpp"

namespace siren::x86 {
    paddr_t get_max_physical_address() noexcept {
        static constinit once_flag once_flag;
        static constinit uint64_t max_physical_address;

        once_flag.call_once(
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
                    max_physical_address = (uint64_t{ 1 } << cpuid<0x80000008u>().semantics.physical_address_bits) - 1u;
                } else if (cpuid<0x1>().semantics.edx.physical_address_extension == 1) {
                    max_physical_address = (uint64_t{ 1 } << 36u) - 1u;
                } else {
                    max_physical_address = (uint64_t{ 1 } << 32u) - 1u;
                }
            }
        );

        return max_physical_address;
    }
}
