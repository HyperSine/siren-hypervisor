#include "dynamic_ept.hpp"
#include "../address_space.hpp"
#include <wdm.h>

namespace siren::vmx {
    bool dynamic_ept::setting_flags::is_present() const noexcept {
        return read_access == 1 || write_access == 1 || execute_access == 1;
    }

    void dynamic_ept::setting_flags::apply_to(decltype(x86::ept_pdt_entry_t::semantics.for_pml1)& entry) const noexcept {
        entry.read_access = read_access;
        entry.write_access = write_access;
        entry.execute_access = execute_access;
        entry.always_zero = 0;
        entry.accessed_flag = accessed_flag;
        entry.user_mode_execute_access = user_mode_execute_access;
    }

    void dynamic_ept::setting_flags::apply_to(decltype(x86::ept_pdpt_entry_t::semantics.for_pml2)& entry) const noexcept {
        entry.read_access = read_access;
        entry.write_access = write_access;
        entry.execute_access = execute_access;
        entry.always_zero = 0;
        entry.accessed_flag = accessed_flag;
        entry.user_mode_execute_access = user_mode_execute_access;
    }

    void dynamic_ept::setting_flags::apply_to(decltype(x86::ept_pml4_entry_t::semantics)& entry) const noexcept {
        entry.read_access = read_access;
        entry.write_access = write_access;
        entry.execute_access = execute_access;
        entry.accessed_flag = accessed_flag;
        entry.user_mode_execute_access = user_mode_execute_access;
    }

    void dynamic_ept::setting_flags::apply_to(decltype(x86::ept_pt_entry_t::semantics)& entry) const noexcept {
        entry.read_access = read_access;
        entry.write_access = write_access;
        entry.execute_access = execute_access;
        entry.memory_type = memory_type;
        entry.ignore_pat_memory_type = ignore_pat_memory_type;
        entry.accessed_flag = accessed_flag;
        entry.dirty_flag = dirty_flag;
        entry.user_mode_execute_access = user_mode_execute_access;
        entry.verify_guest_paging = verify_guest_paging;
        entry.paging_write_access = paging_write_access;
        entry.allow_supervisor_shadow_stack_access = allow_supervisor_shadow_stack_access;
        entry.sub_page_write_permissions = sub_page_write_permissions;
        entry.suppress_ve_exception = suppress_ve_exception;
    }

    void dynamic_ept::setting_flags::apply_to(decltype(x86::ept_pdt_entry_t::semantics.for_2MiB_page)& entry) const noexcept {
        entry.read_access = read_access;
        entry.write_access = write_access;
        entry.execute_access = execute_access;
        entry.memory_type = memory_type;
        entry.ignore_pat_memory_type = ignore_pat_memory_type;
        entry.always_one = 1;
        entry.accessed_flag = accessed_flag;
        entry.dirty_flag = dirty_flag;
        entry.user_mode_execute_access = user_mode_execute_access;
        entry.verify_guest_paging = verify_guest_paging;
        entry.paging_write_access = paging_write_access;
        entry.allow_supervisor_shadow_stack_access = allow_supervisor_shadow_stack_access;
        entry.suppress_ve_exception = suppress_ve_exception;
    }

    void dynamic_ept::setting_flags::apply_to(decltype(x86::ept_pdpt_entry_t::semantics.for_1GiB_page)& entry) const noexcept {
        entry.read_access = read_access;
        entry.write_access = write_access;
        entry.execute_access = execute_access;
        entry.memory_type = memory_type;
        entry.ignore_pat_memory_type = ignore_pat_memory_type;
        entry.always_one = 1;
        entry.accessed_flag = accessed_flag;
        entry.dirty_flag = dirty_flag;
        entry.user_mode_execute_access = user_mode_execute_access;
        entry.verify_guest_paging = verify_guest_paging;
        entry.paging_write_access = paging_write_access;
        entry.allow_supervisor_shadow_stack_access = allow_supervisor_shadow_stack_access;
        entry.suppress_ve_exception = suppress_ve_exception;
    }

    dynamic_ept::setting_flags dynamic_ept::setting_flags::load_from(const decltype(x86::ept_pdt_entry_t::semantics.for_pml1)& entry) noexcept {
        setting_flags flags = {};
        flags.read_access = entry.read_access;
        flags.write_access = entry.write_access;
        flags.execute_access = entry.execute_access;
        // flags.always_zero = entry.always_zero;
        flags.accessed_flag = entry.accessed_flag;
        flags.user_mode_execute_access = entry.user_mode_execute_access;
        return flags;
    }

    dynamic_ept::setting_flags dynamic_ept::setting_flags::load_from(const decltype(x86::ept_pdpt_entry_t::semantics.for_pml2)& entry) noexcept {
        setting_flags flags = {};
        flags.read_access = entry.read_access;
        flags.write_access = entry.write_access;
        flags.execute_access = entry.execute_access;
        // flags.always_zero = entry.always_zero;
        flags.accessed_flag = entry.accessed_flag;
        flags.user_mode_execute_access = entry.user_mode_execute_access;
        return flags;
    }

    dynamic_ept::setting_flags dynamic_ept::setting_flags::load_from(const decltype(x86::ept_pml4_entry_t::semantics)& entry) noexcept {
        setting_flags flags = {};
        flags.read_access = entry.read_access;
        flags.write_access = entry.write_access;
        flags.execute_access = entry.execute_access;
        flags.accessed_flag = entry.accessed_flag;
        flags.user_mode_execute_access = entry.user_mode_execute_access;
        return flags;
    }

    dynamic_ept::setting_flags dynamic_ept::setting_flags::load_from(const decltype(x86::ept_pt_entry_t::semantics)& entry) noexcept {
        setting_flags flags = {};
        flags.read_access = entry.read_access;
        flags.write_access = entry.write_access;
        flags.execute_access = entry.execute_access;
        flags.memory_type = entry.memory_type;
        flags.ignore_pat_memory_type = entry.ignore_pat_memory_type;
        flags.accessed_flag = entry.accessed_flag;
        flags.dirty_flag = entry.dirty_flag;
        flags.user_mode_execute_access = entry.user_mode_execute_access;
        flags.verify_guest_paging = entry.verify_guest_paging;
        flags.paging_write_access = entry.paging_write_access;
        flags.allow_supervisor_shadow_stack_access = entry.allow_supervisor_shadow_stack_access;
        flags.sub_page_write_permissions = entry.sub_page_write_permissions;
        flags.suppress_ve_exception = entry.suppress_ve_exception;
        return flags;
    }

    dynamic_ept::setting_flags dynamic_ept::setting_flags::load_from(const decltype(x86::ept_pdt_entry_t::semantics.for_2MiB_page)& entry) noexcept {
        setting_flags flags = {};
        flags.read_access = entry.read_access;
        flags.write_access = entry.write_access;
        flags.execute_access = entry.execute_access;
        flags.memory_type = entry.memory_type;
        flags.ignore_pat_memory_type = entry.ignore_pat_memory_type;
        // flags.always_one = entry.always_one;
        flags.accessed_flag = entry.accessed_flag;
        flags.dirty_flag = entry.dirty_flag;
        flags.user_mode_execute_access = entry.user_mode_execute_access;
        flags.verify_guest_paging = entry.verify_guest_paging;
        flags.paging_write_access = entry.paging_write_access;
        flags.allow_supervisor_shadow_stack_access = entry.allow_supervisor_shadow_stack_access;
        flags.suppress_ve_exception = entry.suppress_ve_exception;
        return flags;
    }

    dynamic_ept::setting_flags dynamic_ept::setting_flags::load_from(const decltype(x86::ept_pdpt_entry_t::semantics.for_1GiB_page)& entry) noexcept {
        setting_flags flags = {};
        flags.read_access = entry.read_access;
        flags.write_access = entry.write_access;
        flags.execute_access = entry.execute_access;
        flags.memory_type = entry.memory_type;
        flags.ignore_pat_memory_type = entry.ignore_pat_memory_type;
        // flags.always_one = entry.always_one;
        flags.accessed_flag = entry.accessed_flag;
        flags.dirty_flag = entry.dirty_flag;
        flags.user_mode_execute_access = entry.user_mode_execute_access;
        flags.verify_guest_paging = entry.verify_guest_paging;
        flags.paging_write_access = entry.paging_write_access;
        flags.allow_supervisor_shadow_stack_access = entry.allow_supervisor_shadow_stack_access;
        flags.suppress_ve_exception = entry.suppress_ve_exception;
        return flags;
    }

    dynamic_ept::page_description dynamic_ept::page_description::load_from(const decltype(x86::ept_pt_entry_t::semantics)& entry) noexcept {
        page_description desc = {};
        desc.read_access = entry.read_access;
        desc.write_access = entry.write_access;
        desc.execute_access = entry.execute_access;
        desc.memory_type = entry.memory_type;
        desc.ignore_pat_memory_type = entry.ignore_pat_memory_type;
        desc.accessed_flag = entry.accessed_flag;
        desc.dirty_flag = entry.dirty_flag;
        desc.user_mode_execute_access = entry.user_mode_execute_access;
        desc.page_physical_pfn = entry.page_physical_address;
        desc.page_type = 0;
        desc.verify_guest_paging = entry.verify_guest_paging;
        desc.paging_write_access = entry.paging_write_access;
        desc.allow_supervisor_shadow_stack_access = entry.allow_supervisor_shadow_stack_access;
        desc.sub_page_write_permissions = entry.sub_page_write_permissions;
        desc.suppress_ve_exception = entry.suppress_ve_exception;
        return desc;
    }

    dynamic_ept::page_description dynamic_ept::page_description::load_from(const decltype(x86::ept_pdt_entry_t::semantics.for_2MiB_page)& entry) noexcept {
        page_description desc = {};
        desc.read_access = entry.read_access;
        desc.write_access = entry.write_access;
        desc.execute_access = entry.execute_access;
        desc.memory_type = entry.memory_type;
        desc.ignore_pat_memory_type = entry.ignore_pat_memory_type;
        desc.accessed_flag = entry.accessed_flag;
        desc.dirty_flag = entry.dirty_flag;
        desc.user_mode_execute_access = entry.user_mode_execute_access;
        desc.page_physical_pfn = entry.page_physical_address;
        desc.page_type = 1;
        desc.verify_guest_paging = entry.verify_guest_paging;
        desc.paging_write_access = entry.paging_write_access;
        desc.allow_supervisor_shadow_stack_access = entry.allow_supervisor_shadow_stack_access;
        desc.suppress_ve_exception = entry.suppress_ve_exception;
        return desc;
    }

    dynamic_ept::page_description dynamic_ept::page_description::load_from(const decltype(x86::ept_pdpt_entry_t::semantics.for_1GiB_page)& entry) noexcept {
        page_description desc = {};
        desc.read_access = entry.read_access;
        desc.write_access = entry.write_access;
        desc.execute_access = entry.execute_access;
        desc.memory_type = entry.memory_type;
        desc.ignore_pat_memory_type = entry.ignore_pat_memory_type;
        desc.accessed_flag = entry.accessed_flag;
        desc.dirty_flag = entry.dirty_flag;
        desc.user_mode_execute_access = entry.user_mode_execute_access;
        desc.page_physical_pfn = entry.page_physical_address;
        desc.page_type = 2;
        desc.verify_guest_paging = entry.verify_guest_paging;
        desc.paging_write_access = entry.paging_write_access;
        desc.allow_supervisor_shadow_stack_access = entry.allow_supervisor_shadow_stack_access;
        desc.suppress_ve_exception = entry.suppress_ve_exception;
        return desc;
    }

    dynamic_ept::node* dynamic_ept::node::link_before(node* other) noexcept {
        this->forward = other;
        this->backward = other->backward;
        this->backward->forward = this;
        this->forward->backward = this;
        return this;
    }

    dynamic_ept::node* dynamic_ept::node::link_after(node* other) noexcept {
        this->forward = other->forward;
        this->backward = other;
        this->backward->forward = this;
        this->forward->backward = this;
        return this;
    }

    dynamic_ept::node* dynamic_ept::node::unlink() noexcept {
        this->forward->backward = this->backward;
        this->backward->forward = this->forward;
        this->forward = this;
        this->backward = this;
        return this;
    }

    dynamic_ept::node* dynamic_ept::node::get_child(uint32_t index) noexcept {
        constexpr uint32_t mid = 512 / 2;

        if (children && is_pml_present(index)) {
            if (index < mid) {
                node* p = children;
                do {
                    if (index == p->table_index) {
                        return p;
                    } else {
                        p = p->forward;
                    }
                } while (p != children);
            } else {
                node* last_child = children->backward;
                node* p = last_child;
                do {
                    if (index == p->table_index) {
                        return p;
                    } else {
                        p = p->backward;
                    }
                } while (p != last_child);
            }
        }

        return nullptr;
    }

    dynamic_ept::node* dynamic_ept::node::get_child_lowerbound(uint32_t bound) noexcept {
        constexpr uint32_t mid = 512 / 2;

        if (children) {
            if (bound < mid) {
                node* first_child = children;
                node* p = first_child;
                do {
                    if (bound <= p->table_index) {
                        return p;
                    } else {
                        p = p->forward;
                    }
                } while (p != first_child);
                return nullptr;
            } else {
                node* last_child = children->backward;
                node* p = last_child;
                do {
                    if (p->table_index < bound) {
                        return bound <= p->forward->table_index ? p->forward : nullptr;
                    } else {
                        p = p->backward;
                    }
                } while (p != last_child);
                return children;
            }
        }

        return nullptr;
    }

    dynamic_ept::node* dynamic_ept::node::get_child_upperbound(uint32_t bound) noexcept {
        constexpr uint32_t mid = 512 / 2;

        if (children) {
            if (bound < mid) {
                node* first_child = children;
                node* p = first_child;
                do {
                    if (bound < p->table_index) {
                        return p;
                    } else {
                        p = p->forward;
                    }
                } while (p != first_child);
                return nullptr;
            } else {
                node* last_child = children->backward;
                node* p = last_child;
                do {
                    if (p->table_index <= bound) {
                        return bound < p->forward->table_index ? p->forward : nullptr;
                    } else {
                        p = p->backward;
                    }
                } while (p != last_child);
                return children;
            }
        }

        return nullptr;
    }

    size_t dynamic_ept::node::count_children() const noexcept {
        if (children) {
            size_t cnt = 1;
            for (node* p = children->forward; p != children; p = p->forward) {
                ++cnt;
            }
            return cnt;
        } else {
            return 0;
        }
    }

    const dynamic_ept::node* dynamic_ept::node::get_child(uint32_t index) const noexcept {
        return const_cast<node*>(this)->get_child(index);
    }

    const dynamic_ept::node* dynamic_ept::node::get_child_lowerbound(uint32_t bound) const noexcept {
        return const_cast<node*>(this)->get_child_lowerbound(bound);
    }

    const dynamic_ept::node* dynamic_ept::node::get_child_upperbound(uint32_t bound) const noexcept {
        return const_cast<node*>(this)->get_child_upperbound(bound);
    }

    dynamic_ept::node* dynamic_ept::node::attach(node* parent_nd, uint32_t index) noexcept {
        if (parent_nd->children) {
            node* insert_nd = parent_nd->get_child_lowerbound(index);
            if (insert_nd == nullptr) {
                insert_nd = parent_nd->children;
            }

            this->parent = parent_nd;
            this->link_before(insert_nd);
            // this->children;          // don't care
            // this->table;             // already set
            this->table_level = parent_nd->table_level - 1;
            this->table_index = index;
            // this->table_pfn;         // already set

            if (parent_nd->children == insert_nd && table_index < insert_nd->table_index) {
                parent_nd->children = this;
            }
        } else {
            this->parent = parent_nd;
            // this->forward = this;    // already set
            // this->backword = this;   // already set
            // this->children;          // don't care
            // this->table;             // already set
            this->table_level = parent_nd->table_level - 1;
            this->table_index = index;
            // this->table_pfn;         // already set

            parent_nd->children = this;
        }

        switch (parent_nd->table_level) {
            case 2: {
                auto& pml2_entry = parent_nd->table->pml2.entries[index];

                pml2_entry = {};
                pml2_entry.semantics.for_pml1.read_access = 1;
                pml2_entry.semantics.for_pml1.write_access = 1;
                pml2_entry.semantics.for_pml1.execute_access = 1;
                pml2_entry.semantics.for_pml1.always_zero = 0;
                pml2_entry.semantics.for_pml1.user_mode_execute_access = 1;
                pml2_entry.semantics.for_pml1.pml1_physical_address = this->table_pfn;

                break;
            }
            case 3: {
                auto& pml3_entry = parent_nd->table->pml3.entries[index];

                pml3_entry = {};
                pml3_entry.semantics.for_pml2.read_access = 1;
                pml3_entry.semantics.for_pml2.write_access = 1;
                pml3_entry.semantics.for_pml2.execute_access = 1;
                pml3_entry.semantics.for_pml2.always_zero = 0;
                pml3_entry.semantics.for_pml2.user_mode_execute_access = 1;
                pml3_entry.semantics.for_pml2.pml2_physical_address = this->table_pfn;

                break;
            }
            case 4: {
                auto& pml4_entry = parent_nd->table->pml4.entries[index];

                pml4_entry = {};
                pml4_entry.semantics.read_access = 1;
                pml4_entry.semantics.write_access = 1;
                pml4_entry.semantics.execute_access = 1;
                pml4_entry.semantics.user_mode_execute_access = 1;
                pml4_entry.semantics.pml3_physical_address = this->table_pfn;

                break;
            }
            default:
                std::unreachable();
        }

        return this;
    }

    dynamic_ept::node* dynamic_ept::node::detach() noexcept {
        node* parent_nd = this->parent;

        switch (table_level) {
            case 1:
                parent_nd->table->pml2.entries[table_index] = {}; break;
            case 2:
                parent_nd->table->pml3.entries[table_index] = {}; break;
            case 3:
                parent_nd->table->pml4.entries[table_index] = {}; break;
            default:
                std::unreachable();
        }

        this->parent = nullptr;
        if (this->forward == this) {
            parent_nd->children = nullptr;
            // this->forward = this;    // already set
            // this->backward = this;   // already set
        } else {
            if (parent_nd->children == this) {
                parent_nd->children = forward;
            }
            this->unlink();
        }
        // this->children;     // don't care
        this->table_level = 0;
        this->table_index = 0;
        // this->table_pfn;    // already set

        return this;
    }

    bool dynamic_ept::node::is_pml_present(uint32_t index) const noexcept {
        switch (table_level) {
            case 2:
                return table->pml2.entries[index].semantics.for_pml1.is_present();
            case 3:
                return table->pml3.entries[index].semantics.for_pml2.is_present();
            case 4:
                return table->pml4.entries[index].semantics.is_present();
            default:
                return false;
        }
    }

    bool dynamic_ept::node::is_page_present(uint32_t index) const noexcept {
        switch (table_level) {
            case 1:
                return table->pml1.entries[index].semantics.is_present();
            case 2:
                return table->pml2.entries[index].semantics.for_2MiB_page.is_present();
            case 3:
                return table->pml3.entries[index].semantics.for_1GiB_page.is_present();
            default:
                return false;
        }
    }

    void dynamic_ept::node::split_page_entry(uint32_t index, node* new_node) noexcept {
        switch (table_level) {
            case 2: {
                auto& page_entry = table->pml2.entries[index];
                auto& pml1_table = new_node->table->pml1;

                for (size_t i = 0; i < pml1_table.length(); ++i) {
                    pml1_table.entries[i] = {};
                    pml1_table.entries[i].semantics.read_access = page_entry.semantics.for_2MiB_page.read_access;
                    pml1_table.entries[i].semantics.write_access = page_entry.semantics.for_2MiB_page.write_access;
                    pml1_table.entries[i].semantics.execute_access = page_entry.semantics.for_2MiB_page.read_access;
                    pml1_table.entries[i].semantics.memory_type = page_entry.semantics.for_2MiB_page.memory_type;
                    pml1_table.entries[i].semantics.ignore_pat_memory_type = page_entry.semantics.for_2MiB_page.ignore_pat_memory_type;
                    pml1_table.entries[i].semantics.user_mode_execute_access = page_entry.semantics.for_2MiB_page.user_mode_execute_access;

                    x86::paddr_t _2MiB_page_paddr = x86::pfn_to_address<4_Kiuz>(page_entry.semantics.for_2MiB_page.page_physical_address);
                    x86::paddr_t _sub_4KiB_page_paddr = _2MiB_page_paddr + i * 4_Kiuz;

                    pml1_table.entries[i].semantics.page_physical_address = x86::address_to_pfn<4_Kiuz>(_sub_4KiB_page_paddr);
                    pml1_table.entries[i].semantics.verify_guest_paging = page_entry.semantics.for_2MiB_page.verify_guest_paging;
                    pml1_table.entries[i].semantics.paging_write_access = page_entry.semantics.for_2MiB_page.paging_write_access;
                    pml1_table.entries[i].semantics.allow_supervisor_shadow_stack_access = page_entry.semantics.for_2MiB_page.allow_supervisor_shadow_stack_access;
                    pml1_table.entries[i].semantics.suppress_ve_exception = page_entry.semantics.for_2MiB_page.suppress_ve_exception;
                }

                break;
            }
            case 3: {
                auto& pml3_entry = table->pml3.entries[index];
                auto& pml2_table = new_node->table->pml2;

                for (size_t i = 0; i < pml2_table.length(); ++i) {
                    pml2_table.entries[i] = {};
                    pml2_table.entries[i].semantics.for_2MiB_page.read_access = pml3_entry.semantics.for_1GiB_page.read_access;
                    pml2_table.entries[i].semantics.for_2MiB_page.write_access = pml3_entry.semantics.for_1GiB_page.write_access;
                    pml2_table.entries[i].semantics.for_2MiB_page.execute_access = pml3_entry.semantics.for_1GiB_page.read_access;
                    pml2_table.entries[i].semantics.for_2MiB_page.memory_type = pml3_entry.semantics.for_1GiB_page.memory_type;
                    pml2_table.entries[i].semantics.for_2MiB_page.ignore_pat_memory_type = pml3_entry.semantics.for_1GiB_page.ignore_pat_memory_type;
                    pml2_table.entries[i].semantics.for_2MiB_page.always_one = 1;
                    pml2_table.entries[i].semantics.for_2MiB_page.user_mode_execute_access = pml3_entry.semantics.for_1GiB_page.user_mode_execute_access;

                    auto _1GiB_page_paddr = x86::pfn_to_address<4_Kiuz>(pml3_entry.semantics.for_1GiB_page.page_physical_address);
                    auto _sub_2MiB_page_paddr = _1GiB_page_paddr + i * 2_Miuz;

                    pml2_table.entries[i].semantics.for_2MiB_page.page_physical_address = x86::address_to_pfn<4_Kiuz>(_sub_2MiB_page_paddr);
                    pml2_table.entries[i].semantics.for_2MiB_page.verify_guest_paging = pml3_entry.semantics.for_1GiB_page.verify_guest_paging;
                    pml2_table.entries[i].semantics.for_2MiB_page.paging_write_access = pml3_entry.semantics.for_1GiB_page.paging_write_access;
                    pml2_table.entries[i].semantics.for_2MiB_page.allow_supervisor_shadow_stack_access = pml3_entry.semantics.for_1GiB_page.allow_supervisor_shadow_stack_access;
                    pml2_table.entries[i].semantics.for_2MiB_page.suppress_ve_exception = pml3_entry.semantics.for_1GiB_page.suppress_ve_exception;
                }
            }
            default:
                std::unreachable();
        }

        new_node->attach(this, index);
    }

    void dynamic_ept::cache_push(node* nd) noexcept {
        if (m_cache_nodes) {
            nd->link_before(m_cache_nodes);
        } else {
            m_cache_nodes = nd;
        }
    }

    dynamic_ept::node* dynamic_ept::cache_pop() noexcept {
        if (m_cache_nodes) {
            node* nd = m_cache_nodes->backward;
            if (nd == m_cache_nodes) {
                m_cache_nodes = nullptr;
            } else {
                nd->unlink();
            }
            return nd;
        } else {
            return nullptr;
        }
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
    expected<void, nt_status> dynamic_ept::cache_reserve_at_least(size_t require_size) noexcept {
        for (size_t size = cache_size(); size < require_size; ++size) {
            expected<node*, nt_status> expt_new_node = node_new();
            if (expt_new_node.has_value()) {
                node_free_to_cache(expt_new_node.value());
            } else {
                return unexpected{ expt_new_node.error() };
            }
        }
        return {};
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
    void dynamic_ept::cache_shrink(size_t keep_size) noexcept {
        if (m_cache_nodes) {
            if (keep_size == 0) {
                while (m_cache_nodes->backward != m_cache_nodes) {
                    node_free(m_cache_nodes->backward->unlink());
                }
                node_free(m_cache_nodes);   
                m_cache_nodes = nullptr;
            } else {
                node* p = m_cache_nodes;

                for (size_t cnt = 1; p->forward != m_cache_nodes && cnt < keep_size; p = p->forward) {
                    ++cnt;
                }

                while (p->forward != m_cache_nodes) {
                    node_free(p->forward->unlink());
                }
            }
        }
    }

    size_t dynamic_ept::cache_size() const noexcept {
        if (m_cache_nodes) {
            size_t cnt = 1;
            for (node* p = m_cache_nodes->forward; p != m_cache_nodes; p = p->forward) {
                ++cnt;
            }
            return cnt;
        } else {
            return 0;
        }
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
    expected<dynamic_ept::node*, nt_status> dynamic_ept::node_new() noexcept {
        KdPrint(("siren-hv: siren::dynamic_ept::node_new()\n"));

        auto unique_node = allocate_unique<node>(npaged_pool);
        if (!unique_node.has_value()) {
            return unexpected{ unique_node.error() };
        }

        auto unique_node_data = allocate_unique<node_data>(npaged_pool);
        if (!unique_node_data.has_value()) {
            return unexpected{ unique_node_data.error() };
        }

        node* nd = unique_node.value().release();

        nd->parent = nullptr;
        nd->forward = nd;
        nd->backward = nd;
        nd->children = nullptr;

        nd->table = unique_node_data.value().release();
        nd->table_level = 0;
        nd->table_index = 0;
        nd->table_pfn = x86::address_to_pfn<4_Kiuz>(get_physical_address(nd->table));

        return nd;
    }

    expected<dynamic_ept::node*, nt_status> dynamic_ept::node_new_from_cache() noexcept {
        node* nd = cache_pop();
        if (nd) {
            nd->parent = nullptr;
            nd->forward = nd;
            nd->backward = nd;
            nd->children = nullptr;

            memset(nd->table, 0, sizeof(node_data));

            nd->table_level = 0;
            nd->table_index = 0;

            return nd;
        } else {
            return unexpected{ nt_status_insufficient_resources_v };
        }
    }

    dynamic_ept::node* dynamic_ept::node_get(int level, x86::guest_paddr_t gpa) const noexcept {
        node* parent_node;

        if (level == 3) {
            parent_node = m_top_level_node;
        } else {
            parent_node = node_get(level + 1, gpa);
        }

        if (parent_node) {
            if (level == 1) {
                return parent_node->get_child(x86::pml_index<2>(gpa));
            } else if (level == 2) {
                return parent_node->get_child(x86::pml_index<3>(gpa));
            } else if (level == 3) {
                return parent_node->get_child(x86::pml_index<4>(gpa));
            } else {
                std::unreachable();
            }
        } else {
            return nullptr;
        }
    }

    _When_(high_irql == true, _IRQL_requires_max_(HIGH_LEVEL))
    _When_(high_irql == false, _IRQL_requires_max_(DISPATCH_LEVEL))
    expected<dynamic_ept::node*, nt_status> dynamic_ept::node_ensure(int level, x86::guest_paddr_t gpa, bool high_irql) noexcept {
        node* parent_node;

        if (level == 3) {
            parent_node = m_top_level_node;
        } else {
            expected<node*, nt_status> expt_parent_node = node_ensure(level + 1, gpa, high_irql);
            if (expt_parent_node.has_value()) {
                parent_node = expt_parent_node.value();
            } else {
                return unexpected{ expt_parent_node.error() };
            }
        }

        uint32_t target_index;
        switch (level) {
            case 1:
                target_index = x86::pml_index<2>(gpa); break;
            case 2:
                target_index = x86::pml_index<3>(gpa); break;
            case 3:
                target_index = x86::pml_index<4>(gpa); break;
            default:
                std::unreachable();
        }

        node* target_node = parent_node->get_child(target_index);

        if (target_node) {
            return target_node;
        } else {
            expected<node*, nt_status> expt_new_node = high_irql ? node_new_from_cache() : node_new();
            if (expt_new_node.has_error()) {
                return unexpected{ expt_new_node.error() };
            }

            if (parent_node->is_page_present(target_index)) {
                //
                // `parent_node` is guaranteed to be a PML2/PML3/PML4 node.
                // So if the entry at `target_index` is a page entry, the entry is guaranteed to be a 1GiB-page entry or 2MiB-page page,
                //   in other words, the page entry is splitable
                //
                parent_node->split_page_entry(target_index, expt_new_node.value());
                return expt_new_node.value();
            } else {
                return expt_new_node.value()->attach(parent_node, target_index);
            }
        }
    }

    void dynamic_ept::node_free_to_cache(node* nd) noexcept {
        NT_ASSERT(nd->parent == nullptr);
        NT_ASSERT(nd->forward == nd);
        NT_ASSERT(nd->backward == nd);

        while (nd->children) {
            node_free_to_cache(nd->children->backward->detach());
        }

        cache_push(nd);
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
    void dynamic_ept::node_free(node* nd) noexcept {
        NT_ASSERT(nd->parent == nullptr);
        NT_ASSERT(nd->forward == nd);
        NT_ASSERT(nd->backward == nd);

        while (nd->children) {
            node_free(nd->children->backward->detach());
        }

        allocator_delete(npaged_pool, nd->table);
        allocator_delete(npaged_pool, nd);
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
    void dynamic_ept::terminate() noexcept {
        cache_shrink(0);
        if (m_top_level_node) {
            node_free(m_top_level_node);
            m_top_level_node = nullptr;
        }
    }

    dynamic_ept::dynamic_ept() noexcept
        : m_cache_nodes{}, m_top_level_node{} {}

    dynamic_ept::dynamic_ept(dynamic_ept&& other) noexcept
        : m_cache_nodes{}, m_top_level_node{}
    {
        m_cache_nodes = other.m_cache_nodes;
        m_top_level_node = other.m_top_level_node;

        other.m_cache_nodes = nullptr;
        other.m_top_level_node = nullptr;
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
    dynamic_ept& dynamic_ept::operator=(dynamic_ept&& other) noexcept {
        if (this != std::addressof(other)) {
            terminate();

            m_cache_nodes = other.m_cache_nodes;
            m_top_level_node = other.m_top_level_node;

            other.m_cache_nodes = nullptr;
            other.m_top_level_node = nullptr;
        }
        return *this;
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
    dynamic_ept::~dynamic_ept() noexcept {
        terminate();
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
    expected<void, nt_status> dynamic_ept::initialize() noexcept {
        expected<node*, nt_status> top_level_node = node_new();

        if (top_level_node.has_value()) {
            m_top_level_node = top_level_node.value();
        } else {
            return unexpected{ top_level_node.error() };
        }
        
        m_top_level_node->table_level = 4;

        return {};
    }

    x86::paddr_t dynamic_ept::get_top_level_address() const noexcept {
        return x86::pfn_to_address<4_Kiuz>(m_top_level_node->table_pfn);
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
    expected<void, nt_status> dynamic_ept::prepare_page(size_t page_size, x86::guest_paddr_t gpa_base) noexcept {
        int level;

        switch (page_size) {
            case 4_Kiuz:
                if (x86::page_offset<4_Kiuz>(gpa_base) == 0) {
                    level = 1;
                    break;
                } else {
                    return unexpected{ nt_status_invalid_address_v };
                }
            case 2_Miuz:
                if (x86::page_offset<2_Miuz>(gpa_base) == 0) {
                    level = 2;
                    break;
                } else {
                    return unexpected{ nt_status_invalid_address_v };
                }
            case 1_Giuz:
                if (x86::page_offset<1_Giuz>(gpa_base) == 0) {
                    level = 3;
                    break;
                } else {
                    return unexpected{ nt_status_invalid_address_v };
                }
            default:
                return unexpected{ nt_status_invalid_parameter_v };
        }

        size_t required_node_count = 4 - level;

        node* pml3_node = nullptr;
        node* pml2_node = nullptr;
        node* pml1_node = nullptr;

        do {
            pml3_node = m_top_level_node->get_child(x86::pml_index<4>(gpa_base));
            if (pml3_node) {
                --required_node_count;
            } else {
                break;
            }

            if (level <= 2) {
                pml2_node = pml3_node->get_child(x86::pml_index<3>(gpa_base));
                if (pml2_node) {
                    --required_node_count;
                } else {
                    break;
                }
            }

            if (level <= 1) {
                pml1_node = pml2_node->get_child(x86::pml_index<2>(gpa_base));
                if (pml1_node) {
                    --required_node_count;
                } else {
                    break;
                }
            }
        } while (false);

        return cache_reserve_at_least(required_node_count);
    }

    expected<void, nt_status> dynamic_ept::modify_page(size_t page_size, x86::guest_paddr_t gpa_base, x86::host_paddr_t hpa_base) noexcept {
        int level;
        node* target_node;
        uint32_t target_index;

        switch (page_size) {
            case 4_Kiuz:
                if (x86::page_offset<4_Kiuz>(gpa_base) == 0 && x86::page_offset<4_Kiuz>(hpa_base) == 0) {
                    level = 1;
                    target_index = x86::pml_index<1>(gpa_base);
                    break;
                } else {
                    return unexpected{ nt_status_invalid_address_v };
                }
            case 2_Miuz:
                if (x86::page_offset<2_Miuz>(gpa_base) == 0 && x86::page_offset<2_Miuz>(hpa_base) == 0) {
                    level = 2;
                    target_index = x86::pml_index<2>(gpa_base);
                    break;
                } else {
                    return unexpected{ nt_status_invalid_address_v };
                }
            case 1_Giuz:
                if (x86::page_offset<1_Giuz>(gpa_base) == 0 && x86::page_offset<1_Giuz>(hpa_base) == 0) {
                    level = 3;
                    target_index = x86::pml_index<3>(gpa_base);
                    break;
                } else {
                    return unexpected{ nt_status_invalid_address_v };
                }
            default:
                return unexpected{ nt_status_invalid_parameter_v };
        }

        target_node = node_get(level, gpa_base);

        if (target_node) {
            switch (page_size) {
                case 4_Kiuz:
                    if (target_node->is_page_present<1>(target_index)) {
                        target_node->set_page_entry<1>(target_index, hpa_base);
                    }
                    break;
                case 2_Miuz:
                    if (target_node->is_page_present<2>(target_index)) {
                        target_node->set_page_entry<2>(target_index, hpa_base);
                    }
                    break;
                case 1_Giuz:
                    if (target_node->is_page_present<3>(target_index)) {
                        target_node->set_page_entry<3>(target_index, hpa_base);
                    }
                    break;
                default:
                    std::unreachable();
            }
        }

        return unexpected{ nt_status_not_found_v };
    }

    expected<void, nt_status> dynamic_ept::modify_page(size_t page_size, x86::guest_paddr_t gpa_base, setting_flags flags) noexcept {
        int level;
        node* target_node;
        uint32_t target_index;

        if (flags.is_present() == false) {
            return unexpected{ nt_status_invalid_parameter_v };
        }

        switch (page_size) {
            case 4_Kiuz:
                if (x86::page_offset<4_Kiuz>(gpa_base) == 0) {
                    level = 1;
                    target_index = x86::pml_index<1>(gpa_base);
                    break;
                } else {
                    return unexpected{ nt_status_invalid_address_v };
                }
            case 2_Miuz:
                if (x86::page_offset<2_Miuz>(gpa_base) == 0) {
                    level = 2;
                    target_index = x86::pml_index<2>(gpa_base);
                    break;
                } else {
                    return unexpected{ nt_status_invalid_address_v };
                }
            case 1_Giuz:
                if (x86::page_offset<1_Giuz>(gpa_base) == 0) {
                    level = 3;
                    target_index = x86::pml_index<3>(gpa_base);
                    break;
                } else {
                    return unexpected{ nt_status_invalid_address_v };
                }
            default:
                return unexpected{ nt_status_invalid_parameter_v };
        }

        target_node = node_get(level, gpa_base);
        if (target_node == nullptr) {
            return unexpected{ nt_status_not_found_v };
        }

        switch (page_size) {
            case 4_Kiuz:
                if (target_node->is_page_present<1>(target_index)) {
                    target_node->set_page_entry<1>(target_index, flags);
                }
                break;
            case 2_Miuz:
                if (target_node->is_page_present<2>(target_index)) {
                    target_node->set_page_entry<2>(target_index, flags);
                }
                break;
            case 1_Giuz:
                if (target_node->is_page_present<3>(target_index)) {
                    target_node->set_page_entry<3>(target_index, flags);
                }
                break;
            default:
                std::unreachable();
        }

        return unexpected{ nt_status_not_found_v };
    }

    _When_(high_irql == true, _IRQL_requires_max_(HIGH_LEVEL))
    _When_(high_irql == false, _IRQL_requires_max_(DISPATCH_LEVEL))
    expected<void, nt_status> dynamic_ept::commit_page(size_t page_size, x86::guest_paddr_t gpa_base, x86::host_paddr_t hpa_base, setting_flags flags, bool high_irql) noexcept {
        int level;
        node* target_node;
        uint32_t target_index;

        if (flags.is_present() == false) {
            return unexpected{ nt_status_invalid_parameter_v };
        }

        switch (page_size) {
            case 4_Kiuz:
                if (x86::page_offset<4_Kiuz>(gpa_base) == 0 && x86::page_offset<4_Kiuz>(hpa_base) == 0) {
                    level = 1;
                    target_index = x86::pml_index<1>(gpa_base);
                    break;
                } else {
                    return unexpected{ nt_status_invalid_address_v };
                }
            case 2_Miuz:
                if (x86::page_offset<2_Miuz>(gpa_base) == 0 && x86::page_offset<2_Miuz>(hpa_base) == 0) {
                    level = 2;
                    target_index = x86::pml_index<2>(gpa_base);
                    break;
                } else {
                    return unexpected{ nt_status_invalid_address_v };
                }
            case 1_Giuz:
                if (x86::page_offset<1_Giuz>(gpa_base) == 0 && x86::page_offset<1_Giuz>(hpa_base) == 0) {
                    level = 3;
                    target_index = x86::pml_index<3>(gpa_base);
                    break;
                } else {
                    return unexpected{ nt_status_invalid_address_v };
                }
            default:
                return unexpected{ nt_status_invalid_parameter_v };
        }

        {
            expected<node*, nt_status> expt_target_node = node_ensure(level, gpa_base, high_irql);
            if (expt_target_node.has_value()) {
                target_node = expt_target_node.value();
            } else {
                return unexpected{ expt_target_node.error() };
            }
        }

        if (node* next_level_node = target_node->get_child(target_index)) {
            if (high_irql) {
                node_free_to_cache(next_level_node->detach());
            } else {
                node_free(next_level_node->detach());
            }
        }

        switch (page_size) {
            case 4_Kiuz:
                //target_node->table->pml1.entries[target_index] = {};
                target_node->set_page_entry<1>(target_index, hpa_base);
                target_node->set_page_entry<1>(target_index, flags);
                break;
            case 2_Miuz:
                //target_node->table->pml2.entries[target_index] = {};
                target_node->set_page_entry<2>(target_index, hpa_base);
                target_node->set_page_entry<2>(target_index, flags);
                break;
            case 1_Giuz:
                //target_node->table->pml3.entries[target_index] = {};
                target_node->set_page_entry<3>(target_index, hpa_base);
                target_node->set_page_entry<3>(target_index, flags);
                break;
            default:
                std::unreachable();
        }

        return {};
    }

    expected<dynamic_ept::page_description, nt_status> dynamic_ept::find_page(x86::guest_paddr_t gpa) const noexcept {
        node* pml3_node;
        node* pml2_node;
        node* pml1_node;

        uint32_t pml4_index;
        uint32_t pml3_index;
        uint32_t pml2_index;
        uint32_t pml1_index;
        
        pml4_index = x86::pml_index<4>(gpa);

        pml3_node = m_top_level_node->get_child(pml4_index);
        if (pml3_node == nullptr) {
            return unexpected{ nt_status_not_found_v };
        }

        pml3_index = x86::pml_index<3>(gpa);

        if (pml3_node->is_page_present<3>(pml3_index)) {
            return page_description::load_from(pml3_node->table->pml3.entries[pml3_index].semantics.for_1GiB_page);
        }

        pml2_node = pml3_node->get_child(pml3_index);
        if (pml2_node == nullptr) {
            return unexpected{ nt_status_not_found_v };
        }

        pml2_index = x86::pml_index<2>(gpa);

        if (pml2_node->is_page_present<2>(pml2_index)) {
            return page_description::load_from(pml2_node->table->pml2.entries[pml2_index].semantics.for_2MiB_page);
        }

        pml1_node = pml2_node->get_child(pml2_index);
        if (pml1_node == nullptr) {
            return unexpected{ nt_status_not_found_v };
        }

        pml1_index = x86::pml_index<1>(gpa);

        if (pml1_node->is_page_present<1>(pml1_index)) {
            return page_description::load_from(pml1_node->table->pml1.entries[pml1_index].semantics);
        } else {
            return unexpected{ nt_status_not_found_v };
        }
    }

    expected<void, nt_status> dynamic_ept::uncommit_page(size_t page_size, x86::guest_paddr_t gpa_base) noexcept {
        node* pml3_node;
        node* pml2_node;
        node* pml1_node;

        uint32_t pml4_index;
        uint32_t pml3_index;
        uint32_t pml2_index;
        uint32_t pml1_index;

        if (page_size == 4_Kiuz || page_size == 2_Miuz || page_size == 1_Giuz) {
            pml4_index = x86::pml_index<4>(gpa_base);

            pml3_node = m_top_level_node->get_child(pml4_index);
            if (pml3_node == nullptr) {
                return unexpected{ nt_status_not_found_v };
            }

            pml3_index = x86::pml_index<3>(gpa_base);

            if (page_size == 1_Giuz) {
                if (pml3_node->is_page_present<3>(pml3_index)) {
                    pml3_node->table->pml3.entries[pml3_index] = {};
                    return {};
                } else {
                    return unexpected{ nt_status_not_found_v };
                }
            }

            pml2_node = pml3_node->get_child(pml3_index);
            if (pml2_node == nullptr) {
                return unexpected{ nt_status_not_found_v };
            }

            pml2_index = x86::pml_index<2>(gpa_base);

            if (page_size == 2_Miuz) {
                if (pml2_node->is_page_present<2>(pml2_index)) {
                    pml2_node->table->pml2.entries[pml2_index] = {};
                    return {};
                } else {
                    return unexpected{ nt_status_not_found_v };
                }
            }

            pml1_node = pml2_node->get_child(pml2_index);
            if (pml1_node == nullptr) {
                return unexpected{ nt_status_not_found_v };
            }

            pml1_index = x86::pml_index<1>(gpa_base);

            if (pml1_node->is_page_present<1>(pml1_index)) {
                pml1_node->table->pml1.entries[pml1_index] = {};
                return {};
            } else {
                return unexpected{ nt_status_not_found_v };
            }
        } else {
            return unexpected{ nt_status_invalid_parameter_v };
        }
    }
}
