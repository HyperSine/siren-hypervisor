#pragma once
#include "../irql_annotations.hpp"
#include "../literals.hpp"
#include "../memory.hpp"

#include "../x86/paging.hpp"
#include "../x86/intel_ept.hpp"

namespace siren::vmx {
    using namespace ::siren::size_literals;

    class dynamic_ept {
    public:
        struct setting_flags {
            uint32_t read_access : 1;
            uint32_t write_access : 1;
            uint32_t execute_access : 1;
            uint32_t memory_type : 3;
            uint32_t ignore_pat_memory_type : 1;
            uint32_t reserved0 : 1;
            uint32_t accessed_flag : 1;
            uint32_t dirty_flag : 1;
            uint32_t user_mode_execute_access : 1;
            uint32_t reserved1 : 14;
            uint32_t verify_guest_paging : 1;
            uint32_t paging_write_access : 1;
            uint32_t reserved2 : 1;
            uint32_t allow_supervisor_shadow_stack_access : 1;
            uint32_t sub_page_write_permissions : 1;
            uint32_t reserved3 : 1;
            uint32_t suppress_ve_exception : 1;

            [[nodiscard]]
            bool is_present() const noexcept;

            void apply_to(decltype(x86::ept_pdt_entry_t::semantics.for_pml1)& entry) const noexcept;
            void apply_to(decltype(x86::ept_pdpt_entry_t::semantics.for_pml2)& entry) const noexcept;
            void apply_to(decltype(x86::ept_pml4_entry_t::semantics)& entry) const noexcept;

            void apply_to(decltype(x86::ept_pt_entry_t::semantics)& entry) const noexcept;
            void apply_to(decltype(x86::ept_pdt_entry_t::semantics.for_2MiB_page)& entry) const noexcept;
            void apply_to(decltype(x86::ept_pdpt_entry_t::semantics.for_1GiB_page)& entry) const noexcept;

            [[nodiscard]]
            static setting_flags load_from(const decltype(x86::ept_pdt_entry_t::semantics.for_pml1)& entry) noexcept;
            [[nodiscard]]
            static setting_flags load_from(const decltype(x86::ept_pdpt_entry_t::semantics.for_pml2)& entry) noexcept;
            [[nodiscard]]
            static setting_flags load_from(const decltype(x86::ept_pml4_entry_t::semantics)& entry) noexcept;

            [[nodiscard]]
            static setting_flags load_from(const decltype(x86::ept_pt_entry_t::semantics)& entry) noexcept;
            [[nodiscard]]
            static setting_flags load_from(const decltype(x86::ept_pdt_entry_t::semantics.for_2MiB_page)& entry) noexcept;
            [[nodiscard]]
            static setting_flags load_from(const decltype(x86::ept_pdpt_entry_t::semantics.for_1GiB_page)& entry) noexcept;
        };

        static_assert(sizeof(setting_flags) == sizeof(uint32_t));
        static_assert(alignof(setting_flags) == alignof(uint32_t));
        static_assert(std::is_aggregate_v<setting_flags>);

        struct page_description {
            x86::paddr_t read_access : 1;
            x86::paddr_t write_access : 1;
            x86::paddr_t execute_access : 1;
            x86::paddr_t memory_type : 3;
            x86::paddr_t ignore_pat_memory_type : 1;
            x86::paddr_t reserved0 : 1;
            x86::paddr_t accessed_flag : 1;
            x86::paddr_t dirty_flag : 1;
            x86::paddr_t user_mode_execute_access : 1;
            x86::paddr_t reserved1 : 1;
            x86::paddr_t page_physical_pfn : 40;
            x86::paddr_t page_type : 5;     // 0: 4-KiB page, 1: 2-MiB page, 2: 1-GiB page
            x86::paddr_t verify_guest_paging : 1;
            x86::paddr_t paging_write_access : 1;
            x86::paddr_t reserved2 : 1;
            x86::paddr_t allow_supervisor_shadow_stack_access : 1;
            x86::paddr_t sub_page_write_permissions : 1;
            x86::paddr_t reserved3 : 1;
            x86::paddr_t suppress_ve_exception : 1;

            [[nodiscard]]
            static page_description load_from(const decltype(x86::ept_pt_entry_t::semantics)& entry) noexcept;
            [[nodiscard]]
            static page_description load_from(const decltype(x86::ept_pdt_entry_t::semantics.for_2MiB_page)& entry) noexcept;
            [[nodiscard]]
            static page_description load_from(const decltype(x86::ept_pdpt_entry_t::semantics.for_1GiB_page)& entry) noexcept;
        };

        static_assert(sizeof(page_description) == sizeof(x86::paddr_t));
        static_assert(alignof(page_description) == alignof(x86::paddr_t));
        static_assert(std::is_aggregate_v<page_description>);

        struct node_data {
            union {
                x86::ept_pt_t pml1;
                x86::ept_pdt_t pml2;
                x86::ept_pdpt_t pml3;
                x86::ept_pml4_t pml4;
            };
        };

        static_assert(sizeof(node_data) == 4_KiB_uz);
        static_assert(alignof(node_data) == 4_KiB_uz);

        struct node {
            node* parent;
            node* forward;
            node* backward;
            node* children;

            node_data* table;
            x86::paddr_t table_level : 3;
            x86::paddr_t table_index : 9;
            x86::paddr_t table_pfn : 52;

            node* link_before(node* other) noexcept;

            node* link_after(node* other) noexcept;

            node* unlink() noexcept;

            [[nodiscard]]
            node* get_child(uint32_t index) noexcept;

            [[nodiscard]]
            node* get_child_lowerbound(uint32_t bound) noexcept;

            [[nodiscard]]
            node* get_child_upperbound(uint32_t bound) noexcept;

            [[nodiscard]]
            size_t count_children() const noexcept;

            [[nodiscard]]
            const node* get_child(uint32_t index) const noexcept;

            [[nodiscard]]
            const node* get_child_lowerbound(uint32_t bound) const noexcept;

            [[nodiscard]]
            const node* get_child_upperbound(uint32_t bound) const noexcept;

            // make sure current node is detached before calling
            node* attach(node* parent, uint32_t index) noexcept;

            // make sure current node is attached before calling
            node* detach() noexcept;

            [[nodiscard]]
            bool is_pml_present(uint32_t index) const noexcept;

            template<int Level>
                requires (1 <= Level && Level <= 4)
            [[nodiscard]]
            bool is_pml_present(uint32_t index) const noexcept {
                if constexpr (Level == 2) {
                    return table->pml2.entries[index].semantics.for_pml1.is_present();
                } else if constexpr (Level == 3) {
                    return table->pml3.entries[index].semantics.for_pml2.is_present();
                } else if constexpr (Level == 4) {
                    return table->pml4.entries[index].semantics.is_present();
                } else {
                    return false;
                }
            }

            [[nodiscard]]
            bool is_page_present(uint32_t index) const noexcept;

            template<int Level>
                requires (1 <= Level && Level <= 4)
            [[nodiscard]]
            bool is_page_present(uint32_t index) const noexcept {
                if constexpr (Level == 1) {
                    return table->pml1.entries[index].semantics.is_present();
                } else if constexpr (Level == 2) {
                    return table->pml2.entries[index].semantics.for_2MiB_page.is_present();
                } else if constexpr (Level == 3) {
                    return table->pml3.entries[index].semantics.for_1GiB_page.is_present();
                } else {
                    return false;
                }
            }

            template<int Level>
            void set_page_entry(uint32_t index, x86::paddr_t page_base) {
                if constexpr (Level == 1) {
                    table->pml1.entries[index].semantics.page_physical_address = x86::address_to_pfn(page_base, x86::on_4KiB_page_t{});
                } else if constexpr (Level == 2) {
                    table->pml2.entries[index].semantics.for_2MiB_page.page_physical_address = x86::address_to_pfn(page_base, x86::on_4KiB_page_t{});
                } else if constexpr (Level == 3) {
                    table->pml3.entries[index].semantics.for_1GiB_page.page_physical_address = x86::address_to_pfn(page_base, x86::on_4KiB_page_t{});
                }
            }

            template<int Level>
            void set_page_entry(uint32_t index, setting_flags flags) {
                if constexpr (Level == 1) {
                    flags.apply_to(table->pml1.entries[index].semantics);
                } else if constexpr (Level == 2) {
                    flags.apply_to(table->pml2.entries[index].semantics.for_2MiB_page);
                } else if constexpr (Level == 3) {
                    flags.apply_to(table->pml3.entries[index].semantics.for_1GiB_page);
                }
            }

            void split_page_entry(uint32_t index, node* new_node) noexcept;
        };

    private:
        node* m_cache_nodes;
        node* m_top_level_node;

        void cache_push(node* nd) noexcept;

        [[nodiscard]]
        node* cache_pop() noexcept;

        _IRQL_requires_max_(DISPATCH_LEVEL)
        [[nodiscard]]
        expected<void, nt_status> cache_reserve_at_least(size_t require_size) noexcept;

        _IRQL_requires_max_(DISPATCH_LEVEL)
        void cache_shrink(size_t keep_size) noexcept;

        [[nodiscard]]
        size_t cache_size() const noexcept;

        _IRQL_requires_max_(DISPATCH_LEVEL)
        [[nodiscard]]
        expected<node*, nt_status> node_new() noexcept;

        [[nodiscard]]
        expected<node*, nt_status> node_new_from_cache() noexcept;

        [[nodiscard]]
        node* node_get(int level, x86::guest_paddr_t gpa) const noexcept;

        _When_(high_irql == true, _IRQL_requires_max_(HIGH_LEVEL))
        _When_(high_irql == false, _IRQL_requires_max_(DISPATCH_LEVEL))
        [[nodiscard]]
        expected<node*, nt_status> node_ensure(int level, x86::guest_paddr_t gpa, bool high_irql) noexcept;

        void node_free_to_cache(node* nd) noexcept;

        _IRQL_requires_max_(DISPATCH_LEVEL)
        void node_free(node* nd) noexcept;

        _IRQL_requires_max_(DISPATCH_LEVEL)
        void terminate() noexcept;

    public:
        dynamic_ept() noexcept;

        // copy constructor is not allowed
        dynamic_ept(const dynamic_ept& other) = delete;

        // move constructor
        dynamic_ept(dynamic_ept&& other) noexcept;

        // copy assignment is not allowed
        dynamic_ept& operator=(const dynamic_ept&) = delete;

        // move assigment operator
        _IRQL_requires_max_(DISPATCH_LEVEL)
        dynamic_ept& operator=(dynamic_ept&& other) noexcept;

        _IRQL_requires_max_(DISPATCH_LEVEL)
        ~dynamic_ept() noexcept;

        _IRQL_requires_max_(DISPATCH_LEVEL)
        [[nodiscard]]
        expected<void, nt_status> initialize() noexcept;

        [[nodiscard]]
        x86::paddr_t get_top_level_address() const noexcept;

        _IRQL_requires_max_(DISPATCH_LEVEL)
        [[nodiscard]]
        expected<void, nt_status> prepare_page(size_t page_size, x86::guest_paddr_t gpa_base) noexcept;

        [[nodiscard]]
        expected<void, nt_status> modify_page(size_t page_size, x86::guest_paddr_t gpa_base, x86::host_paddr_t hpa_base) noexcept;

        [[nodiscard]]
        expected<void, nt_status> modify_page(size_t page_size, x86::guest_paddr_t gpa_base, setting_flags flags) noexcept;

        _When_(high_irql == true, _IRQL_requires_max_(HIGH_LEVEL))
        _When_(high_irql == false, _IRQL_requires_max_(DISPATCH_LEVEL))
        [[nodiscard]]
        expected<void, nt_status> commit_page(size_t page_size, x86::guest_paddr_t gpa_base, x86::host_paddr_t hpa_base, setting_flags flags, bool high_irql) noexcept;

        [[nodiscard]]
        expected<page_description, nt_status> find_page(x86::guest_paddr_t gpa) const noexcept;

        [[nodiscard]]
        expected<void, nt_status> uncommit_page(size_t page_size, x86::guest_paddr_t gpa_base) noexcept;
    };

}
