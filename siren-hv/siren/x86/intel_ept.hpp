#pragma once
#include "../literals.hpp"
#include "paging.hpp"

namespace siren::x86 {
    using namespace ::siren::size_literals;

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 28 VMX Support for Address Translation
    //    |-> 28.3 The Extended Page Table Mechanism (EPT)
    //      |-> 28.3.2 EPT Translation Mechanism
    //        |-> Table 28-1. Format of an EPT PML4 Entry (PML4E) that References an EPT Page-Directory-Pointer Table
    struct ept_pml4_entry_t {
        union {
            uint64_t storage;
            struct {
                uint64_t read_access : 1;
                uint64_t write_access : 1;
                uint64_t execute_access : 1;
                uint64_t reserved0 : 5;
                uint64_t accessed_flag : 1;
                uint64_t ignored0 : 1;
                uint64_t user_mode_execute_access : 1;
                uint64_t ignored1 : 1;
                uint64_t pml3_physical_address : 40;
                uint64_t ignored2 : 12;

                [[nodiscard]]
                constexpr bool is_present() const noexcept {
                    return read_access == 1 || write_access == 1 || execute_access == 1;
                }
            } semantics;
        };
    };

    static_assert(sizeof(ept_pml4_entry_t::storage) == 8);
    static_assert(sizeof(ept_pml4_entry_t::storage) == sizeof(ept_pml4_entry_t::semantics));

    struct alignas(4_KiB_size_v) ept_pml4_t {
        ept_pml4_entry_t entry[4_kb_size_v / sizeof(ept_pml4_entry_t)];
    };

    static_assert(alignof(ept_pml4_t) == 4_kb_size_v);
    static_assert(sizeof(ept_pml4_t) == 4_kb_size_v);

    constexpr size_t ept_pml4_entry_count_v = std::extent_v<decltype(ept_pml4_t::entry)>;

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 28 VMX Support for Address Translation
    //    |-> 28.3 The Extended Page Table Mechanism (EPT)
    //      |-> 28.3.2 EPT Translation Mechanism
    //        |-> Table 28-2. Format of an EPT Page-Directory-Pointer-Table Entry (PDPTE) that Maps a 1-GByte Page
    //            Table 28-3. Format of an EPT Page-Directory-Pointer-Table Entry (PDPTE) that References an EPT Page Directory
    struct ept_pdpt_entry_t {
        union {
            uint64_t storage;
            union {
                struct {
                    uint64_t read_access : 1;
                    uint64_t write_access : 1;
                    uint64_t execute_access : 1;
                    uint64_t reserved0 : 4;
                    uint64_t always_zero : 1;
                    uint64_t accessed_flag : 1;
                    uint64_t reserved1 : 1;
                    uint64_t user_mode_execute_access : 1;
                    uint64_t ignored0 : 1;
                    uint64_t pml2_physical_address : 40;
                    uint64_t ignored1 : 12;

                    [[nodiscard]]
                    constexpr bool is_present() const noexcept {
                        return (read_access == 1 || write_access == 1 || execute_access == 1) && always_zero == 0;
                    }
                } for_pml2;
                struct {
                    uint64_t read_access : 1;
                    uint64_t write_access : 1;
                    uint64_t execute_access : 1;
                    uint64_t memory_type : 3;
                    uint64_t ignore_pat_memory_type : 1;
                    uint64_t always_one : 1;
                    uint64_t accessed_flag : 1;
                    uint64_t dirty_flag : 1;
                    uint64_t user_mode_execute_access : 1;
                    uint64_t ignored0 : 1;
                    uint64_t page_physical_address : 40;
                    uint64_t ignored1 : 5;
                    uint64_t verify_guest_paging : 1;
                    uint64_t paging_write_access : 1;
                    uint64_t ignored2 : 1;
                    uint64_t allow_supervisor_shadow_stack_access : 1;
                    uint64_t ignored3 : 2;
                    uint64_t suppress_ve_exception : 1;

                    [[nodiscard]]
                    constexpr bool is_present() const noexcept {
                        return (read_access == 1 || write_access == 1 || execute_access == 1) && always_one == 1;
                    }
                } for_1gb_page;
            } semantics;
        };
    };

    static_assert(sizeof(ept_pdpt_entry_t::storage) == 8);
    static_assert(sizeof(ept_pdpt_entry_t::storage) == sizeof(ept_pdpt_entry_t::semantics));

    struct alignas(4_kb_size_v) ept_pdpt_t {
        ept_pdpt_entry_t entry[4_kb_size_v / sizeof(ept_pdpt_entry_t)];
    };

    static_assert(alignof(ept_pdpt_t) == 4_kb_size_v);
    static_assert(sizeof(ept_pdpt_t) == 4_kb_size_v);

    constexpr size_t ept_pdpt_entry_count_v = std::extent_v<decltype(ept_pdpt_t::entry)>;

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 28 VMX Support for Address Translation
    //    |-> 28.3 The Extended Page Table Mechanism (EPT)
    //      |-> 28.3.2 EPT Translation Mechanism
    //        |-> Table 28-4. Format of an EPT Page-Directory Entry (PDE) that Maps a 2-MByte Page
    //            Table 28-5. Format of an EPT Page-Directory Entry (PDE) that References an EPT Page Table
    struct ept_pdt_entry_t {
        union {
            uint64_t storage;
            union {
                struct {
                    uint64_t read_access : 1;
                    uint64_t write_access : 1;
                    uint64_t execute_access : 1;
                    uint64_t reserved0 : 4;
                    uint64_t always_zero : 1;
                    uint64_t accessed_flag : 1;
                    uint64_t reserved1 : 1;
                    uint64_t user_mode_execute_access : 1;
                    uint64_t ignored0 : 1;
                    uint64_t pml1_physical_address : 40;
                    uint64_t ignored1 : 12;

                    [[nodiscard]]
                    constexpr bool is_present() const noexcept {
                        return (read_access == 1 || write_access == 1 || execute_access == 1) && always_zero == 0;
                    }
                } for_pml1;
                struct {
                    uint64_t read_access : 1;
                    uint64_t write_access : 1;
                    uint64_t execute_access : 1;
                    uint64_t memory_type : 3;
                    uint64_t ignore_pat_memory_type : 1;
                    uint64_t always_one : 1;
                    uint64_t accessed_flag : 1;
                    uint64_t dirty_flag : 1;
                    uint64_t user_mode_execute_access : 1;
                    uint64_t ignored0 : 1;
                    uint64_t page_physical_address : 40;
                    uint64_t ignored1 : 5;
                    uint64_t verify_guest_paging : 1;
                    uint64_t paging_write_access : 1;
                    uint64_t ignored2 : 1;
                    uint64_t allow_supervisor_shadow_stack_access : 1;
                    uint64_t ignored3 : 2;
                    uint64_t suppress_ve_exception : 1;

                    [[nodiscard]]
                    constexpr bool is_present() const noexcept {
                        return (read_access == 1 || write_access == 1 || execute_access == 1) && always_one == 1;
                    }
                } for_2mb_page;
            } semantics;
        };
    };

    static_assert(sizeof(ept_pdt_entry_t::storage) == 8);
    static_assert(sizeof(ept_pdt_entry_t::storage) == sizeof(ept_pdt_entry_t::semantics));

    struct alignas(4_kb_size_v) ept_pdt_t {
        ept_pdt_entry_t entry[4_kb_size_v / sizeof(ept_pdt_entry_t)];
    };

    static_assert(alignof(ept_pdt_t) == 4_kb_size_v);
    static_assert(sizeof(ept_pdt_t) == 4_kb_size_v);

    constexpr size_t ept_pdt_entry_count_v = std::extent_v<decltype(ept_pdt_t::entry)>;

    // Defined in
    // [*] Volume 3 (3A, 3B, 3C & 3D): System Programming Guide
    //  |-> Chapter 28 VMX Support for Address Translation
    //    |-> 28.3 The Extended Page Table Mechanism (EPT)
    //      |-> 28.3.3 EPT-Induced VM Exits
    //        |-> 28.3.3.1 EPT Misconfigurations
    //          |-> Table 28-6. Format of an EPT Page-Table Entry that Maps a 4-KByte Page
    struct ept_pt_entry_t {
        union {
            uint64_t storage;
            struct {
                uint64_t read_access : 1;
                uint64_t write_access : 1;
                uint64_t execute_access : 1;
                uint64_t memory_type : 3;
                uint64_t ignore_pat_memory_type : 1;
                uint64_t ignored0 : 1;
                uint64_t accessed_flag : 1;
                uint64_t dirty_flag : 1;
                uint64_t user_mode_execute_access : 1;
                uint64_t ignored1 : 1;
                uint64_t page_physical_address : 40;
                uint64_t ignored2 : 5;
                uint64_t verify_guest_paging : 1;
                uint64_t paging_write_access : 1;
                uint64_t ignored3 : 1;
                uint64_t allow_supervisor_shadow_stack_access : 1;
                uint64_t sub_page_write_permissions : 1;
                uint64_t ignored4 : 1;
                uint64_t suppress_ve_exception : 1;

                [[nodiscard]]
                constexpr bool is_present() const noexcept {
                    return read_access == 1 || write_access == 1 || execute_access == 1;
                }
            } semantics;
        };
    };

    static_assert(sizeof(ept_pt_entry_t::storage) == 8);
    static_assert(sizeof(ept_pt_entry_t::storage) == sizeof(ept_pt_entry_t::semantics));

    struct alignas(4_kb_size_v) ept_pt_t {
        ept_pt_entry_t entry[4_kb_size_v / sizeof(ept_pt_entry_t)];
    };

    static_assert(alignof(ept_pt_t) == 4_kb_size_v);
    static_assert(sizeof(ept_pt_t) == 4_kb_size_v);

    constexpr size_t ept_pt_entry_count_v = std::extent_v<decltype(ept_pt_t::entry)>;

    struct guest_physical_address_t {
        physical_address_t storage;

        constexpr operator physical_address_t() const noexcept {
            return storage;
        }

        template<unsigned _Level>
            requires(1 <= _Level && _Level <= 5)    // only Page-Map-Level 1 ~ 5 is allowed
        [[nodiscard]]
        constexpr auto index_of_pml() const noexcept {
            return static_cast<uint32_t>((storage >> (12u + (_Level - 1u) * 9u)) & 0x1ffu);
        }

        [[nodiscard]]
        constexpr auto offset_of_1gb_page() const noexcept {
            return static_cast<uint32_t>(storage & 1_gb_mask_v);
        }

        [[nodiscard]]
        constexpr auto offset_of_2mb_page() const noexcept {
            return static_cast<uint32_t>(storage & 2_mb_mask_v);
        }

        [[nodiscard]]
        constexpr auto offset_of_4kb_page() const noexcept {
            return static_cast<uint32_t>(storage & 4_kb_mask_v);
        }

        [[nodiscard]]
        static constexpr auto for_512gb_region(uint32_t pml4_index) noexcept {
            return guest_physical_address_t{
                physical_address_t{ pml4_index & 0x1ffu } << 512_gb_shift_v
            };
        }

        [[nodiscard]]
        static constexpr auto for_1gb_page(uint32_t pml4_index, uint32_t pml3_index, uint32_t page_offset) noexcept {
            return guest_physical_address_t{
                physical_address_t{ page_offset & 1_gb_mask_v } |
                physical_address_t{ pml3_index & 0x1ffu } << 1_gb_shift_v |
                physical_address_t{ pml4_index & 0x1ffu } << 512_gb_shift_v
            };
        }

        [[nodiscard]]
        static constexpr auto for_1gb_region(uint32_t pml4_index, uint32_t pml3_index) noexcept {
            return guest_physical_address_t::for_1gb_page(pml4_index, pml3_index, 0);
        }

        [[nodiscard]]
        static constexpr auto for_2mb_page(uint32_t pml4_index, uint32_t pml3_index, uint32_t pml2_index, uint32_t page_offset) noexcept {
            return guest_physical_address_t{
                physical_address_t{ page_offset & 2_mb_mask_v } |
                physical_address_t{ pml2_index & 0x1ffu } << 2_mb_shift_v |
                physical_address_t{ pml3_index & 0x1ffu } << 1_gb_shift_v |
                physical_address_t{ pml4_index & 0x1ffu } << 512_gb_shift_v
            };
        }

        [[nodiscard]]
        static constexpr auto for_2mb_region(uint32_t pml4_index, uint32_t pml3_index, uint32_t pml2_index) noexcept {
            return guest_physical_address_t::for_2mb_page(pml4_index, pml3_index, pml2_index, 0);
        }

        [[nodiscard]]
        static constexpr auto for_4kb_page(uint32_t pml4_index, uint32_t pml3_index, uint32_t pml2_index, uint32_t pml1_index, uint32_t page_offset) noexcept {
            return guest_physical_address_t{
                physical_address_t{ page_offset & 4_kb_mask_v } |
                physical_address_t{ pml1_index & 0x1ffu } << 4_kb_shift_v |
                physical_address_t{ pml2_index & 0x1ffu } << 2_mb_shift_v |
                physical_address_t{ pml3_index & 0x1ffu } << 1_gb_shift_v |
                physical_address_t{ pml4_index & 0x1ffu } << 512_gb_shift_v
            };
        }
    };

    using host_physical_address_t = physical_address_t;
}
