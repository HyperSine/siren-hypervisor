#include <ntddk.h>
#include "tlfs.hypercalls.hpp"

extern "C" {
    NTKERNELAPI UINT64 NTAPI HvlInvokeHypercall(UINT64 InputValue, PHYSICAL_ADDRESS InputBlockPa, PHYSICAL_ADDRESS OutputBlockPa);
    NTKERNELAPI UINT64 NTAPI HvlInvokeFastExtendedHypercall(UINT64 InputValue, PVOID InputBlock, ULONG InputBlockSize, PVOID OutputBlock, ULONG OutputBlockSize);
}

namespace siren::microsoft_hv {

    namespace hypercalls {

        [[nodiscard]]
        result_value_t flush_virtual_address_space(address_space_id_t address_space, flush_flags_t flags, uint64_t processor_mask) noexcept {
            UINT64 input_block[3];
            input_block[0] = address_space;     // AddressSpace +0x0    8   Specifies an address space ID (EPT PML4 table pointer).
            input_block[1] = flags.storage;     // Flags        +0x8    8   RsvdZ
            input_block[2] = processor_mask;

            auto input_value = input_value_t{
                .semantics = {
                    .call_code = call_code_e::HvFlushVirtualAddressSpace,
                    .fast = 1,
                    .is_nested = 0
                }
            };

            return result_value_t{ .storage = HvlInvokeFastExtendedHypercall(input_value.storage, &input_block, sizeof(input_block), nullptr, 0) };
        }

        [[nodiscard]]
        result_value_t flush_guest_physical_address_space(spa_t address_space) noexcept {
            UINT64 input_block[2];
            input_block[0] = address_space;     // AddressSpace +0x0    8   Specifies an address space ID (EPT PML4 table pointer).
            input_block[1] = 0;                 // Flags        +0x8    8   RsvdZ

            auto input_value = input_value_t{
                .semantics = {
                    .call_code = call_code_e::HvCallFlushGuestPhysicalAddressSpace,
                    .fast = 1,
                    .is_nested = 0
                }
            };

            return result_value_t{ .storage = HvlInvokeFastExtendedHypercall(input_value.storage, &input_block, sizeof(input_block), nullptr, 0) };
        }

    }

}
