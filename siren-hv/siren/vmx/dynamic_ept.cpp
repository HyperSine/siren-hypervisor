#include "dynamic_ept.hpp"

namespace siren::hypervisors::vmx {

    [[nodiscard]]
    dynamic_ept::node_t* dynamic_ept::node_t::create(lookaside_pool* node_allocator, lookaside_pool* table_allocator, bool no_interrupt) noexcept {
        node_t* new_node = nullptr;
        any_table_t* new_table = nullptr;

        new_node = node_allocator->allocate<node_t>(no_interrupt);
        if (new_node == nullptr) {
            goto ON_FINAL;
        }

        new_table = table_allocator->allocate<any_table_t>(no_interrupt);
        if (new_table == nullptr) {
            goto ON_FINAL;
        }

        new_node->m_parent = nullptr;
        new_node->m_forward = new_node;
        new_node->m_backward = new_node;
        new_node->m_children = nullptr;
        new_node->m_table = new_table;
        new_node->m_table_level = 0;
        new_node->m_table_index = 0;
        new_node->m_table_pfn = x86::address_to_pfn(x86::get_physical_address(new_table), x86::_4kb_page_traits{});

    ON_FINAL:
        if (new_node == nullptr || new_table == nullptr) {
            table_allocator->deallocate(new_table, no_interrupt);
            new_table = nullptr;

            node_allocator->deallocate(new_node, no_interrupt);
            new_node = nullptr;
        }

        return new_node;
    }

    void dynamic_ept::node_t::attach(node_t* parent, uint32_t index) noexcept {
        if (parent->m_children) {
            node_t* insert_node = parent->get_child_lowerbound(index);
            if (insert_node == nullptr) {
                insert_node = parent->m_children;
            }

            m_parent = parent;
            m_forward = insert_node;
            m_backward = insert_node->m_backward;
            m_backward->m_forward = this;
            m_forward->m_backward = this;
            // m_children;          // don't cares
            // m_table;             // already set
            m_table_level = parent->m_table_level - 1;
            m_table_index = index;
            // m_table_pfn;         // already set

            if (parent->m_children == insert_node && get_table_index() < insert_node->get_table_index()) {
                parent->m_children = this;
            }
        } else {
            m_parent = parent;
            // m_forward = this;    // already set
            // m_backword = this;   // already set
            // m_children;          // don't cares
            // m_table;             // already set
            m_table_level = parent->m_table_level - 1;
            m_table_index = index;
            // m_table_pfn;         // already set

            parent->m_children = this;
        }

        switch (parent->get_table_level()) {
            case 2: {
                auto& pml2_entry = parent->m_table->pml2.entry[index];

                pml2_entry = {};
                pml2_entry.semantics.for_pml1.read_access = 1;
                pml2_entry.semantics.for_pml1.write_access = 1;
                pml2_entry.semantics.for_pml1.execute_access = 1;
                pml2_entry.semantics.for_pml1.always_zero = 0;
                pml2_entry.semantics.for_pml1.user_mode_execute_access = 1;
                pml2_entry.semantics.for_pml1.pml1_physical_address = x86::address_to_pfn(get_table_physical_address(), x86::_4kb_page_traits{});

                break;
            }
            case 3: {
                auto& pml3_entry = parent->m_table->pml3.entry[index];

                pml3_entry = {};
                pml3_entry.semantics.for_pml2.read_access = 1;
                pml3_entry.semantics.for_pml2.write_access = 1;
                pml3_entry.semantics.for_pml2.execute_access = 1;
                pml3_entry.semantics.for_pml2.always_zero = 0;
                pml3_entry.semantics.for_pml2.user_mode_execute_access = 1;
                pml3_entry.semantics.for_pml2.pml2_physical_address = x86::address_to_pfn(get_table_physical_address(), x86::_4kb_page_traits{});

                break;
            }
            case 4: {
                auto& pml4_entry = parent->m_table->pml4.entry[index];

                pml4_entry = {};
                pml4_entry.semantics.read_access = 1;
                pml4_entry.semantics.write_access = 1;
                pml4_entry.semantics.execute_access = 1;
                pml4_entry.semantics.user_mode_execute_access = 1;
                pml4_entry.semantics.pml3_physical_address = x86::address_to_pfn(get_table_physical_address(), x86::_4kb_page_traits{});

                break;
            }
            default:
                invoke_debugger_noreturn();
        }
    }

    void dynamic_ept::node_t::detach() noexcept {
        auto* parent_node = m_parent;

        switch (get_table_level()) {
            case 1:
                parent_node->m_table->pml2.entry[get_table_index()] = {}; break;
            case 2:
                parent_node->m_table->pml3.entry[get_table_index()] = {}; break;
            case 3:
                parent_node->m_table->pml4.entry[get_table_index()] = {}; break;
            default:
                invoke_debugger_noreturn();
        }

        m_parent = nullptr;

        if (m_forward == this) {
            parent_node->m_children = nullptr;
            // forward = this;    // already set
            // backward = this;   // already set
        } else {
            if (parent_node->m_children == this) {
                parent_node->m_children = m_forward;
            }

            m_forward->m_backward = m_backward;
            m_backward->m_forward = m_forward;

            m_forward = this;
            m_backward = this;
        }

        // m_table_level;   // leave it untouched
        m_table_index = 0;
        // m_table_pfn;    // already set
    }

    void dynamic_ept::node_t::destroy(lookaside_pool* node_allocator, lookaside_pool* table_allocator, bool no_interrupt) noexcept {
        while (m_children) {
            auto* last_child = m_children->m_backward;

            last_child->detach();
            last_child->destroy(node_allocator, table_allocator, no_interrupt);
        }

        table_allocator->deallocate(m_table, no_interrupt);
        node_allocator->deallocate(this, no_interrupt);
    }

    [[nodiscard]]
    dynamic_ept::node_t* dynamic_ept::node_t::split_page_entry(uint32_t index, lookaside_pool* node_allocator, lookaside_pool* table_allocator, bool no_interrupt) noexcept {
        node_t* new_node = node_t::create(node_allocator, table_allocator, no_interrupt);

        if (new_node) {
            switch (get_table_level()) {
                case 2: {
                    x86::ept_pdt_entry_t pml2_entry = m_table->pml2.entry[index];
                    x86::ept_pt_t& pml1_table = new_node->m_table->pml1;

                    for (size_t i = 0; i < x86::ept_pt_entry_count_v; ++i) {
                        pml1_table.entry[i] = {};
                        pml1_table.entry[i].semantics.read_access = pml2_entry.semantics.for_2mb_page.read_access;
                        pml1_table.entry[i].semantics.write_access = pml2_entry.semantics.for_2mb_page.write_access;
                        pml1_table.entry[i].semantics.execute_access = pml2_entry.semantics.for_2mb_page.read_access;
                        pml1_table.entry[i].semantics.memory_type = pml2_entry.semantics.for_2mb_page.memory_type;
                        pml1_table.entry[i].semantics.ignore_pat_memory_type = pml2_entry.semantics.for_2mb_page.ignore_pat_memory_type;
                        pml1_table.entry[i].semantics.user_mode_execute_access = pml2_entry.semantics.for_2mb_page.user_mode_execute_access;

                        auto _2mb_page_physical_address = x86::pfn_to_address(pml2_entry.semantics.for_2mb_page.page_physical_address, x86::_4kb_page_traits{});
                        auto _sub_1kb_page_physical_address = _2mb_page_physical_address + i * 4_kb_size_v;

                        pml1_table.entry[i].semantics.page_physical_address = x86::address_to_pfn(_sub_1kb_page_physical_address, x86::_4kb_page_traits{});
                        pml1_table.entry[i].semantics.verify_guest_paging = pml2_entry.semantics.for_2mb_page.verify_guest_paging;
                        pml1_table.entry[i].semantics.paging_write_access = pml2_entry.semantics.for_2mb_page.paging_write_access;
                        pml1_table.entry[i].semantics.allow_supervisor_shadow_stack_access = pml2_entry.semantics.for_2mb_page.allow_supervisor_shadow_stack_access;
                        pml1_table.entry[i].semantics.suppress_ve_exception = pml2_entry.semantics.for_2mb_page.suppress_ve_exception;
                    }

                    break;
                }
                case 3: {
                    x86::ept_pdpt_entry_t pml3_entry = m_table->pml3.entry[index];
                    x86::ept_pdt_t& pml2_table = new_node->m_table->pml2;

                    for (size_t i = 0; i < x86::ept_pdt_entry_count_v; ++i) {
                        pml2_table.entry[i] = {};
                        pml2_table.entry[i].semantics.for_2mb_page.read_access = pml3_entry.semantics.for_1gb_page.read_access;
                        pml2_table.entry[i].semantics.for_2mb_page.write_access = pml3_entry.semantics.for_1gb_page.write_access;
                        pml2_table.entry[i].semantics.for_2mb_page.execute_access = pml3_entry.semantics.for_1gb_page.read_access;
                        pml2_table.entry[i].semantics.for_2mb_page.memory_type = pml3_entry.semantics.for_1gb_page.memory_type;
                        pml2_table.entry[i].semantics.for_2mb_page.ignore_pat_memory_type = pml3_entry.semantics.for_1gb_page.ignore_pat_memory_type;
                        pml2_table.entry[i].semantics.for_2mb_page.always_one = 1;
                        pml2_table.entry[i].semantics.for_2mb_page.user_mode_execute_access = pml3_entry.semantics.for_1gb_page.user_mode_execute_access;

                        auto _1gb_page_physical_address = x86::pfn_to_address(pml3_entry.semantics.for_1gb_page.page_physical_address, x86::_4kb_page_traits{});
                        auto _sub_2mb_page_physical_address = _1gb_page_physical_address + i * 2_mb_size_v;

                        pml2_table.entry[i].semantics.for_2mb_page.page_physical_address = x86::address_to_pfn(_sub_2mb_page_physical_address, x86::_4kb_page_traits{});
                        pml2_table.entry[i].semantics.for_2mb_page.verify_guest_paging = pml3_entry.semantics.for_1gb_page.verify_guest_paging;
                        pml2_table.entry[i].semantics.for_2mb_page.paging_write_access = pml3_entry.semantics.for_1gb_page.paging_write_access;
                        pml2_table.entry[i].semantics.for_2mb_page.allow_supervisor_shadow_stack_access = pml3_entry.semantics.for_1gb_page.allow_supervisor_shadow_stack_access;
                        pml2_table.entry[i].semantics.for_2mb_page.suppress_ve_exception = pml3_entry.semantics.for_1gb_page.suppress_ve_exception;
                    }
                }
                default:
                    invoke_debugger_noreturn();
            }

            new_node->attach(this, index);
        }

        return new_node;
    }

    [[nodiscard]]
    dynamic_ept::node_t* dynamic_ept::node_t::get_child(uint32_t index) const noexcept {
        constexpr uint32_t mid = 512 / 2;

        if (index < mid) {
            auto* p = m_children;
            do {
                if (index == p->get_table_index()) {
                    return p;
                } else {
                    p = p->m_forward;
                }
            } while (p != m_children);
        } else {
            auto* list_last = m_children->m_backward;
            auto* p = list_last;
            do {
                if (index == p->get_table_index()) {
                    return p;
                } else {
                    p = p->m_backward;
                }
            } while (p != list_last);
        }

        return nullptr;
    }

    [[nodiscard]]
    dynamic_ept::node_t* dynamic_ept::node_t::get_child_lowerbound(uint32_t bound) const noexcept {
        constexpr uint32_t mid = 512 / 2;

        if (bound < mid) {
            auto* list_first = m_children;
            auto* p = list_first;

            do {
                if (bound <= p->get_table_index()) {
                    return p;
                } else {
                    p = p->m_forward;
                }
            } while (p != list_first);

            return nullptr;
        } else {
            auto* list_last = m_children->m_backward;
            auto* p = list_last;

            do {
                if (p->get_table_index() < bound) {
                    return bound <= p->m_forward->get_table_index() ? p->m_forward : nullptr;
                } else {
                    p = p->m_backward;
                }
            } while (p != list_last);

            return m_children;
        }
    }

    [[nodiscard]]
    dynamic_ept::node_t* dynamic_ept::node_t::get_child_upperbound(uint32_t bound) const noexcept {
        constexpr uint32_t mid = 512 / 2;

        if (bound < mid) {
            auto* list_first = m_children;
            auto* p = list_first;

            do {
                if (bound < p->get_table_index()) {
                    return p;
                } else {
                    p = p->m_forward;
                }
            } while (p != list_first);

            return nullptr;
        } else {
            auto* list_last = m_children->m_backward;
            auto* p = list_last;

            do {
                if (p->get_table_index() <= bound) {
                    return bound < p->m_forward->get_table_index() ? p->m_forward : nullptr;
                } else {
                    p = p->m_backward;
                }
            } while (p != list_last);

            return m_children;
        }
    }

    [[nodiscard]]
    status_t dynamic_ept::init() noexcept {
        auto node_allocator = lookaside_pool::create<node_t>(false);
        if (!node_allocator) {
            return status_t::insufficient_memory_v;
        }

        auto table_allocator = lookaside_pool::create<any_table_t>(false);
        if (!table_allocator) {
            return status_t::insufficient_memory_v;
        }

        node_t* root_node = node_t::create(node_allocator.get(), table_allocator.get(), false);
        if (root_node) {
            root_node->m_table_level = 4;
        } else {
            return status_t::insufficient_memory_v;
        }

        std::swap(m_node_allocator, node_allocator);
        std::swap(m_table_allocator, table_allocator);
        std::swap(m_root_node, root_node);

        return status_t::success_v;
    }

}
