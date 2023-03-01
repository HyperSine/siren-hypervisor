#pragma once
#include "../../siren_global.hpp"
#include "../../siren_memory.hpp"
#include "../../lookaside_pool.hpp"
#include "../../x86/paging.hpp"
#include "../../x86/intel_ept.hpp"

namespace siren::hypervisors::vmx {

    class dynamic_ept {
    public:
        struct flags_t {
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
            uint32_t reserved3 : 2;
            uint32_t suppress_ve_exception : 1;

            static constexpr uint32_t pml_entry_mask_low_v = 0x00000507u;
            static constexpr uint32_t pml_entry_mask_high_v = 0x00000000u;

            static constexpr uint32_t page_entry_mask_low_v = 0x0000077fu;
            static constexpr uint32_t page_entry_mask_high_v = 0x96000000u;

            template<typename _Ty>
                requires(std::disjunction_v<std::is_same<_Ty, x86::ept_pt_entry_t>, std::is_same<_Ty, x86::ept_pdt_entry_t>, std::is_same<_Ty, x86::ept_pdpt_entry_t>>)
            [[nodiscard]]
            static constexpr flags_t load_from_page_entry(const _Ty& entry) noexcept {
                uint32_t entry_low = static_cast<uint32_t>(entry.storage);
                uint32_t entry_high = static_cast<uint32_t>(entry.storage >> 32u);
                return std::bit_cast<flags_t>(entry_low & page_entry_mask_low_v | entry_high & page_entry_mask_high_v);
            }

            template<typename _Ty>
                requires(std::disjunction_v<std::is_same<_Ty, x86::ept_pdt_entry_t>, std::is_same<_Ty, x86::ept_pdpt_entry_t>, std::is_same<_Ty, x86::ept_pml4_entry_t>>)
            [[nodiscard]]
            static constexpr flags_t load_from_pml_entry(const _Ty& entry) noexcept {
                uint32_t entry_low = static_cast<uint32_t>(entry.storage);
                uint32_t entry_high = static_cast<uint32_t>(entry.storage >> 32u);
                return std::bit_cast<flags_t>(entry_low & pml_entry_mask_low_v | entry_high & pml_entry_mask_high_v);
            }

            [[nodiscard]]
            constexpr bool is_present() const noexcept {
                return read_access == 1 || write_access == 1 || execute_access == 1;
            }

            template<typename _Ty>
                requires(std::disjunction_v<std::is_same<_Ty, x86::ept_pt_entry_t>, std::is_same<_Ty, x86::ept_pdt_entry_t>, std::is_same<_Ty, x86::ept_pdpt_entry_t>>)
            void apply_to_page_entry(_Ty& entry) const noexcept {
                uint32_t entry_low = static_cast<uint32_t>(entry.storage);
                uint32_t entry_high = static_cast<uint32_t>(entry.storage >> 32u);

                entry_low &= ~page_entry_mask_low_v;
                entry_low |= std::bit_cast<uint32_t>(*this) & page_entry_mask_low_v;

                entry_high &= ~page_entry_mask_high_v;
                entry_high |= std::bit_cast<uint32_t>(*this) & page_entry_mask_high_v;

                entry.storage = static_cast<uint64_t>(entry_low) | static_cast<uint64_t>(entry_high) << 32u;
            }

            template<typename _Ty>
                requires(std::disjunction_v<std::is_same<_Ty, x86::ept_pdt_entry_t>, std::is_same<_Ty, x86::ept_pdpt_entry_t>, std::is_same<_Ty, x86::ept_pml4_entry_t>>)
            void apply_to_pml_entry(_Ty& entry) const noexcept {
                uint32_t entry_low = static_cast<uint32_t>(entry.storage);
                uint32_t entry_high = static_cast<uint32_t>(entry.storage >> 32u);

                entry_low &= ~pml_entry_mask_low_v;
                entry_low |= std::bit_cast<uint32_t>(*this) & pml_entry_mask_low_v;

                entry_high &= ~pml_entry_mask_high_v;
                entry_high |= std::bit_cast<uint32_t>(*this) & pml_entry_mask_high_v;

                entry.storage = static_cast<uint64_t>(entry_low) | static_cast<uint64_t>(entry_high) << 32u;
            }
        };

        static_assert(sizeof(flags_t) == sizeof(uint32_t));

        struct any_table_t {
            union {
                x86::ept_pt_t pml1;
                x86::ept_pdt_t pml2;
                x86::ept_pdpt_t pml3;
                x86::ept_pml4_t pml4;
            };
        };

        static_assert(sizeof(any_table_t) == 4_kb_size_v);

        struct node_t {
            friend class dynamic_ept;
        private:
            node_t* m_parent;
            node_t* m_forward;
            node_t* m_backward;
            node_t* m_children;

            any_table_t* m_table;
            x86::physical_address_t m_table_level : 3;
            x86::physical_address_t m_table_index : 9;
            x86::physical_address_t m_table_pfn : 52;

            [[nodiscard]]
            static node_t* create(lookaside_pool* node_allocator, lookaside_pool* table_allocator, bool no_interrupt) noexcept;

            // make sure current node is detached before calling
            void attach(node_t* parent, uint32_t index) noexcept;

            // make sure current node is attached before calling
            void detach() noexcept;

            // make sure current node is detached before calling
            void destroy(lookaside_pool* node_allocator, lookaside_pool* table_allocator, bool no_interrupt) noexcept;

            // make sure the entry pointed by `index` is a 1gb/2mb-page entry
            // return nullptr if and only if insufficient memory
            node_t* split_page_entry(uint32_t index, lookaside_pool* node_allocator, lookaside_pool* table_allocator, bool no_interrupt) noexcept;

        public:
            [[nodiscard]]
            const auto* get_table() const noexcept {
                return m_table;
            }

            template<uint32_t _CurrentNodeLevel>
                requires(1 <= _CurrentNodeLevel && _CurrentNodeLevel <= 4)
            const auto* get_table() const noexcept {
                if constexpr (_CurrentNodeLevel == 1) {
                    return &m_table->pml1;
                } else if constexpr (_CurrentNodeLevel == 2) {
                    return &m_table->pml2;
                } else if constexpr (_CurrentNodeLevel == 3) {
                    return &m_table->pml3;
                } else if constexpr (_CurrentNodeLevel == 4) {
                    return &m_table->pml4;
                } else {
                    static_assert(1 <= _CurrentNodeLevel && _CurrentNodeLevel <= 4);
                }
            }

            [[nodiscard]]
            uint32_t get_table_level() const noexcept {
                return static_cast<uint32_t>(m_table_level);
            }

            [[nodiscard]]
            uint32_t get_table_index() const noexcept {
                return static_cast<uint32_t>(m_table_index);
            }

            [[nodiscard]]
            x86::physical_address_t get_table_physical_address() const noexcept {
                return x86::pfn_to_address(m_table_pfn, x86::_4kb_page_traits{});
            }

            [[nodiscard]]
            bool has_page_entry(uint32_t index) const noexcept {
                switch (get_table_level()) {
                    case 1:
                        return has_page_entry<1>(index);
                    case 2:
                        return has_page_entry<2>(index);
                    case 3:
                        return has_page_entry<3>(index);
                    default:
                        return false;
                }
            }

            template<uint32_t _CurrentNodeLevel>
                requires(1 <= _CurrentNodeLevel && _CurrentNodeLevel <= 4)
            [[nodiscard]]
            bool has_page_entry(uint32_t index) const noexcept {
                if constexpr (_CurrentNodeLevel == 1) {
                    return m_table->pml1.entry[index].semantics.is_present();
                } else if constexpr (_CurrentNodeLevel == 2) {
                    return m_table->pml2.entry[index].semantics.for_2mb_page.is_present();
                } else if constexpr (_CurrentNodeLevel == 3) {
                    return m_table->pml3.entry[index].semantics.for_1gb_page.is_present();
                } else {
                    return false;
                }
            }

            [[nodiscard]]
            bool has_pml_entry(uint32_t index) const noexcept {
                switch (get_table_level()) {
                    case 2:
                        return has_pml_entry<2>(index);
                    case 3:
                        return has_pml_entry<3>(index);
                    case 4:
                        return has_pml_entry<4>(index);
                    default:
                        return false;
                }
            }

            template<uint32_t _CurrentNodeLevel>
                requires(1 <= _CurrentNodeLevel && _CurrentNodeLevel <= 4)
            [[nodiscard]]
            bool has_pml_entry(uint32_t index) const noexcept {
                if constexpr (_CurrentNodeLevel == 2) {
                    return m_table->pml2.entry[index].semantics.for_pml1.is_present();
                } else if constexpr (_CurrentNodeLevel == 3) {
                    return m_table->pml3.entry[index].semantics.for_pml2.is_present();
                } else if constexpr (_CurrentNodeLevel == 4) {
                    return m_table->pml4.entry[index].semantics.is_present();
                } else {
                    return false;
                }
            }

            [[nodiscard]]
            status_t set_page_entry(uint32_t index, flags_t flags) noexcept {
                switch (get_table_level()) {
                    case 1:
                        return set_page_entry<1>(index, flags);
                    case 2:
                        return set_page_entry<2>(index, flags);
                    case 3:
                        return set_page_entry<3>(index, flags);
                    default:
                        return status_t::not_supported_v;
                }
            }

            [[nodiscard]]
            status_t set_page_entry(uint32_t index, x86::host_physical_address_t host_page_base, flags_t flags) noexcept {
                switch (get_table_level()) {
                    case 1:
                        return set_page_entry<1>(index, host_page_base, flags);
                    case 2:
                        return set_page_entry<2>(index, host_page_base, flags);
                    case 3:
                        return set_page_entry<3>(index, host_page_base, flags);
                    default:
                        return status_t::not_supported_v;
                }
            }

            template<uint32_t _CurrentNodeLevel>
                requires(1 <= _CurrentNodeLevel && _CurrentNodeLevel <= 3)
            [[nodiscard]]
            status_t set_page_entry(uint32_t index, flags_t flags) noexcept {
                if constexpr (_CurrentNodeLevel == 1) {
                    auto& pml1_entry = m_table->pml1.entry[index];

                    if (!pml1_entry.semantics.is_present()) {
                        return status_t::not_exist_v;
                    }

                    if (flags.is_present()) {
                        flags.apply_to_page_entry(pml1_entry);
                    } else {
                        pml1_entry = {};
                    }
                } else if constexpr (_CurrentNodeLevel == 2) {
                    auto& pml2_entry = m_table->pml2.entry[index];

                    if (!pml2_entry.semantics.for_2mb_page.is_present()) {
                        return status_t::not_exist_v;
                    }

                    if (flags.is_present()) {
                        flags.apply_to_page_entry(pml2_entry);
                    } else {
                        pml2_entry = {};
                    }
                } else if constexpr (_CurrentNodeLevel == 3) {
                    auto& pml3_entry = m_table->pml3.entry[index];

                    if (!pml3_entry.semantics.for_1gb_page.is_present()) {
                        return status_t::not_exist_v;
                    }

                    if (flags.is_present()) {
                        flags.apply_to_page_entry(pml3_entry);
                    } else {
                        pml3_entry = {};
                    }
                } else {
                    static_assert(1 <= _CurrentNodeLevel && _CurrentNodeLevel <= 3);
                }

                return status_t::success_v;
            }

            template<uint32_t _CurrentNodeLevel>
                requires(1 <= _CurrentNodeLevel && _CurrentNodeLevel <= 3)
            [[nodiscard]]
            status_t set_page_entry(uint32_t index, x86::host_physical_address_t host_page_base, flags_t flags) noexcept {
                if constexpr (_CurrentNodeLevel == 1) {
                    if (x86::guest_physical_address_t{ host_page_base }.offset_of_4kb_page() != 0) {
                        return status_t::not_aligned_v;
                    }

                    auto& pml1_entry = m_table->pml1.entry[index];

                    pml1_entry = {};
                    if (flags.is_present()) {
                        pml1_entry.semantics.page_physical_address = x86::address_to_pfn(host_page_base, x86::_4kb_page_traits{});
                        flags.apply_to_page_entry(pml1_entry);
                    }
                } else if constexpr (_CurrentNodeLevel == 2) {
                    if (x86::guest_physical_address_t{ host_page_base }.offset_of_2mb_page() != 0) {
                        return status_t::not_aligned_v;
                    }

                    auto& pml2_entry = m_table->pml2.entry[index];

                    if (pml2_entry.semantics.for_pml1.is_present()) {
                        return status_t::not_available_yet_v;
                    }

                    pml2_entry = {};
                    if (flags.is_present()) {
                        pml2_entry.semantics.for_2mb_page.always_one = 1;
                        pml2_entry.semantics.for_2mb_page.page_physical_address = x86::address_to_pfn(host_page_base, x86::_4kb_page_traits{});
                        flags.apply_to_page_entry(pml2_entry);
                    }
                } else if constexpr (_CurrentNodeLevel == 3) {
                    if (x86::guest_physical_address_t{ host_page_base }.offset_of_1gb_page() != 0) {
                        return status_t::not_aligned_v;
                    }

                    auto& pml3_entry = m_table->pml3.entry[index];

                    if (pml3_entry.semantics.for_pml2.is_present()) {
                        return status_t::not_available_yet_v;
                    }

                    pml3_entry = {};
                    if (flags.is_present()) {
                        pml3_entry.semantics.for_1gb_page.always_one = 1;
                        pml3_entry.semantics.for_1gb_page.page_physical_address = x86::address_to_pfn(host_page_base, x86::_4kb_page_traits{});
                        flags.apply_to_page_entry(pml3_entry);
                    }
                } else {
                    static_assert(1 <= _CurrentNodeLevel && _CurrentNodeLevel <= 3);
                }

                return status_t::success_v;
            }

            [[nodiscard]]
            status_t set_pml_entry(uint32_t index, flags_t flags) noexcept {
                switch (get_table_level()) {
                    case 2:
                        return set_pml_entry<2>(index, flags);
                    case 3:
                        return set_pml_entry<3>(index, flags);
                    case 4:
                        return set_pml_entry<4>(index, flags);
                    default:
                        return status_t::not_supported_v;
                }
            }

            template<uint32_t _CurrentNodeLevel>
                requires(2 <= _CurrentNodeLevel && _CurrentNodeLevel <= 4)
            [[nodiscard]]
            status_t set_pml_entry(uint32_t index, flags_t flags) noexcept {
                if (flags.is_present()) {
                    return status_t::invalid_argument_v;
                }

                if constexpr (_CurrentNodeLevel == 2) {
                    auto& pml2_entry = m_table->pml2.entry[index];
                    if (pml2_entry.semantics.for_pml1.is_present()) {
                        flags.apply_to_pml_entry(pml2_entry);
                    } else {
                        return status_t::not_exist_v;
                    }
                } else if constexpr (_CurrentNodeLevel == 3) {
                    auto& pml3_entry = m_table->pml3.entry[index];
                    if (pml3_entry.semantics.for_pml2.is_present()) {
                        flags.apply_to_pml_entry(pml3_entry);
                    } else {
                        return status_t::not_exist_v;
                    }
                } else if constexpr (_CurrentNodeLevel == 4) {
                    auto& pml4_entry = m_table->pml4.entry[index];
                    if (pml4_entry.semantics.is_present()) {
                        flags.apply_to_pml_entry(pml4_entry);
                    } else {
                        return status_t::not_exist_v;
                    }
                } else {
                    static_assert(2 <= _CurrentNodeLevel && _CurrentNodeLevel <= 4);
                }

                return status_t::success_v;
            }

            // make sure `m_children` is not nullptr before calling
            [[nodiscard]]
            node_t* get_child(uint32_t index) const noexcept;

            // make sure `m_children` is not nullptr before calling
            [[nodiscard]]
            node_t* get_child_lowerbound(uint32_t bound) const noexcept;

            // make sure `m_children` is not nullptr before calling
            [[nodiscard]]
            node_t* get_child_upperbound(uint32_t bound) const noexcept;
        };

    private:
        managed_object_ptr<lookaside_pool> m_node_allocator;
        managed_object_ptr<lookaside_pool> m_table_allocator;
        node_t* m_root_node;

    public:
        dynamic_ept() noexcept : m_root_node(nullptr) {}

        // move constructor
        dynamic_ept(dynamic_ept&& other) noexcept :
            m_node_allocator(std::move(other.m_node_allocator)),
            m_table_allocator(std::move(other.m_table_allocator)),
            m_root_node(other.m_root_node) { other.m_root_node = nullptr; }

        // move assigment operator
        dynamic_ept& operator=(dynamic_ept&& other) noexcept {
            if (m_root_node) {
                m_root_node->destroy(m_node_allocator.get(), m_table_allocator.get(), false);
                m_root_node = nullptr;
            }

            m_node_allocator = std::move(other.m_node_allocator);
            m_table_allocator = std::move(other.m_table_allocator);

            m_root_node = other.m_root_node;
            other.m_root_node = nullptr;

            return *this;
        }

        ~dynamic_ept() noexcept {
            if (m_root_node) {
                m_root_node->destroy(m_node_allocator.get(), m_table_allocator.get(), false);
            }
        }

        [[nodiscard]]
        status_t init() noexcept;

        [[nodiscard]]
        node_t* get_root_node() const noexcept {
            return m_root_node;
        }

        // Get PML3, PML2 or PML1 node based on given GPA.
        // Return nullptr if and only if not found.
        template<uint32_t _Level>
            requires(1 <= _Level && _Level <= 3)
        [[nodiscard]]
        node_t* get_node(x86::guest_physical_address_t gpa) const noexcept {
            __assume(m_root_node != nullptr);

            node_t* parent_node = 
                _Level == 3 ? m_root_node : get_node<_Level + 1>(gpa);

            if (parent_node) {
                uint32_t parent_index = gpa.index_of_pml<_Level + 1>();
                return parent_node->has_pml_entry<_Level + 1>(parent_index) ? parent_node->get_child(parent_index) : nullptr;
            } else {
                return nullptr;
            }
        }
        
        // return nullptr if and only if insufficient memory
        template<uint32_t _Level>
            requires(1 <= _Level && _Level <= 3)
        node_t* ensure_node(x86::guest_physical_address_t gpa, bool no_interrupt) noexcept {
            __assume(m_root_node != nullptr);

            node_t* parent_node = 
                _Level == 3 ? m_root_node : ensure_node<_Level + 1>(gpa, no_interrupt);

            if (parent_node) {
                uint32_t parent_index = gpa.index_of_pml<_Level + 1>();

                if (parent_node->has_pml_entry<_Level + 1>(parent_index)) {
                    return parent_node->get_child(parent_index);
                } else if (parent_node->has_page_entry<_Level + 1>(parent_index)) {
                    return parent_node->split_page_entry(parent_index, m_node_allocator.get(), m_table_allocator.get(), no_interrupt);
                } else {
                    auto* new_node = node_t::create(m_node_allocator.get(), m_table_allocator.get(), no_interrupt);

                    if (new_node) {
                        new_node->attach(m_root_node, parent_index);
                    }

                    return new_node;
                }
            } else {
                return nullptr;
            }
        }

        template<uint32_t _Level>
            requires(1 <= _Level && _Level <= 3)
        [[nodiscard]]
        void erase_node(x86::guest_physical_address_t gpa, bool no_interrupt) noexcept {
            node_t* node = get_node<_Level>(gpa);
            if (node) {
                node->detach();
                node->destroy(m_node_allocator.get(), m_table_allocator.get(), no_interrupt);
            }
        }

        template<uint32_t _Level>
            requires(1 <= _Level && _Level <= 3)
        [[nodiscard]]
        status_t prepare_node(x86::guest_physical_address_t gpa) noexcept {
            size_t required_node_count = 4 - _Level;

            node_t* pml3_node = nullptr;
            node_t* pml2_node = nullptr;
            node_t* pml1_node = nullptr;

            do {
                if (uint32_t pml4_index = gpa.index_of_pml<4>(); m_root_node->has_pml_entry<4>(pml4_index)) {
                    pml3_node = m_root_node->get_child(pml4_index);
                    --required_node_count;
                } else {
                    break;
                }

                if constexpr (_Level <= 2) {
                    if (uint32_t pml3_index = gpa.index_of_pml<3>(); pml3_node->has_pml_entry<3>(pml3_index)) {
                        pml2_node = pml3_node->get_child(pml3_index);
                        --required_node_count;
                    } else {
                        break;
                    }
                }

                if constexpr (_Level <= 1) {
                    if (uint32_t pml2_index = gpa.index_of_pml<2>(); pml2_node->has_pml_entry<2>(pml2_index)) {
                        pml1_node = pml2_node->get_child(pml2_index);
                        --required_node_count;
                    } else {
                        break;
                    }
                }
            } while (false);

            while (m_node_allocator->preallocated_count() < required_node_count) {
                if (m_node_allocator->preallocate() == false) {
                    return status_t::insufficient_memory_v;
                }
            }

            while (m_table_allocator->preallocated_count() < required_node_count) {
                if (m_table_allocator->preallocate() == false) {
                    return status_t::insufficient_memory_v;
                }
            }

            return status_t::success_v;
        }
    };

}
