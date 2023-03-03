#include "ept_manager.hpp"
#include "../../kernel_allocation.hpp"
#include "../../x86.multiprocessor.hpp"
#include "siren_hypercalls.hpp"

namespace siren::hypervisors::vmx {

    template<unsigned _Level>
    status_t ept_manager::build_ept_identity_map(x86::physical_address_t from_hpa, x86::physical_address_t max_physical_address) noexcept {
        auto status = status_t::success_v;

        constexpr size_t entry_count =
            _Level == 4 ? x86::ept_pml4_entry_count_v :
            _Level == 3 ? x86::ept_pdpt_entry_count_v :
            _Level == 2 ? x86::ept_pdt_entry_count_v :
            _Level == 1 ? x86::ept_pt_entry_count_v : 0;

        constexpr x86::physical_address_t hpa_size_per_entry =
            _Level == 4 ? x86::physical_address_t{ x86::ept_pdpt_entry_count_v } * 1_gb_size_v :
            _Level == 3 ? x86::physical_address_t{ x86::ept_pdt_entry_count_v } * 2_mb_size_v :
            _Level == 2 ? x86::physical_address_t{ x86::ept_pt_entry_count_v } * 4_kb_size_v :
            _Level == 1 ? x86::physical_address_t{ 1 } * 4_kb_size_v : 0;

        for (size_t i = 0; i < entry_count; ++i) {
            auto hpa_base = from_hpa + i * hpa_size_per_entry;

            if (max_physical_address < hpa_base) {
                break;
            }

            if constexpr (_Level == 4) {
                status = build_ept_identity_map<3>(hpa_base, max_physical_address);
                if (status.fails()) {
                    break;
                }
            } else if constexpr (_Level == 3) {
                auto memory_type = m_memory_type_advisor.get_best_memory_type(hpa_base, ~(hpa_size_per_entry - 1));

                if (memory_type.is_reserved()) {
                    status = build_ept_identity_map<2>(hpa_base, max_physical_address);
                } else {
                    status = m_dynamic_ept.commit_page<x86::for_1gb_page_t>({ hpa_base }, { hpa_base }, { .read_access = 1, .write_access = 1, .execute_access = 1, .memory_type = memory_type.to_integral() }, false);
                }

                if (status.fails()) {
                    break;
                }
            } else if constexpr (_Level == 2) {
                auto memory_type = m_memory_type_advisor.get_best_memory_type(hpa_base, ~(hpa_size_per_entry - 1));

                if (memory_type.is_reserved()) {
                    status = build_ept_identity_map<1>(hpa_base, max_physical_address);
                } else {
                    status = m_dynamic_ept.commit_page<x86::for_2mb_page_t>({ hpa_base }, { hpa_base }, { .read_access = 1, .write_access = 1, .execute_access = 1, .memory_type = memory_type.to_integral() }, false);
                }

                if (status.fails()) {
                    break;
                }
            } else if constexpr (_Level == 1) {
                auto memory_type = m_memory_type_advisor.get_best_memory_type(hpa_base, ~(hpa_size_per_entry - 1));

                if (memory_type.is_reserved()) {
                    invoke_debugger_noreturn();
                } else {
                    status = m_dynamic_ept.commit_page<x86::for_4kb_page_t>({ hpa_base }, { hpa_base }, { .read_access = 1, .write_access = 1, .execute_access = 1, .memory_type = memory_type.to_integral() }, false);
                }

                if (status.fails()) {
                    break;
                }
            } else {
                static_assert(1 <= _Level && _Level <= 4);  // always false here.
            }
        }

        return status;
    }

    ept_manager::ept_manager() noexcept = default;

    ept_manager::ept_manager(ept_manager&& other) noexcept :
        m_memory_type_advisor(std::move(other.m_memory_type_advisor)),
        m_dynamic_ept(std::move(m_dynamic_ept)) {}

    ept_manager::~ept_manager() noexcept = default;

    [[nodiscard]]
    status_t ept_manager::init() noexcept {
        auto status = status_t::success_v;

        status = m_memory_type_advisor.init();
        if (status.fails()) {
            goto ON_FINAL;
        }

        status = m_dynamic_ept.init();
        if (status.fails()) {
            goto ON_FINAL;
        }

        status = build_ept_identity_map<4>(0, x86::get_max_physical_address());
        if (status.fails()) {
            goto ON_FINAL;
        }

    ON_FINAL:
        return status;
    }

    [[nodiscard]]
    x86::physical_address_t ept_manager::get_pml4_physical_address() const noexcept {
        return m_dynamic_ept.get_pml4_physical_address();
    }

    template<typename _PageTraits>
    [[nodiscard]]
    status_t ept_manager::guest_commit_page(x86::guest_physical_address_t guest_page_base, x86::host_physical_address_t host_page_base, dynamic_ept::page_flags_t flags) noexcept {
        status_t status;

        status = m_dynamic_ept.precommit_page<_PageTraits>(guest_page_base);
        if (status.fails()) {
            return status;
        }

        x86::ipi_broadcast(
            [once_flag = std::atomic_int{}, guest_page_base, host_page_base, flags, &status]() mutable noexcept {
                x86::cpu_call_once(
                    once_flag, 
                    [guest_page_base, host_page_base, flags, &status]() noexcept {
                        if constexpr (_PageTraits::page_size_v == 1_gb_size_v) {
                            status = siren_hypercalls::ept_commit_1gb_page(guest_page_base.storage, host_page_base, std::bit_cast<uint32_t>(flags));
                        } else if constexpr (_PageTraits::page_size_v == 2_mb_size_v) {
                            status = siren_hypercalls::ept_commit_2mb_page(guest_page_base.storage, host_page_base, std::bit_cast<uint32_t>(flags));
                        } else if constexpr (_PageTraits::page_size_v == 4_kb_size_v) {
                            status = siren_hypercalls::ept_commit_4kb_page(guest_page_base.storage, host_page_base, std::bit_cast<uint32_t>(flags));
                        } else {
                            []<bool dummy_flag = false>() { static_assert(dummy_flag, "Only 1gb, 2mb and 4kb page are allowed."); }();
                        }
                    }
                );

                siren_hypercalls::ept_flush();
            }
        );

        return status;
    }

    template
    [[nodiscard]]
    status_t ept_manager::guest_commit_page<x86::for_1gb_page_t>(x86::guest_physical_address_t guest_page_base, x86::host_physical_address_t host_page_base, dynamic_ept::page_flags_t flags) noexcept;
    
    template
    [[nodiscard]]
    status_t ept_manager::guest_commit_page<x86::for_2mb_page_t>(x86::guest_physical_address_t guest_page_base, x86::host_physical_address_t host_page_base, dynamic_ept::page_flags_t flags) noexcept;

    template
    [[nodiscard]]
    status_t ept_manager::guest_commit_page<x86::for_4kb_page_t>(x86::guest_physical_address_t guest_page_base, x86::host_physical_address_t host_page_base, dynamic_ept::page_flags_t flags) noexcept;

    template<typename _PageTraits>
    [[nodiscard]]
    status_t ept_manager::guest_uncommit_page(x86::guest_physical_address_t guest_page_base) noexcept {
        status_t status;

        x86::ipi_broadcast(
            [once_flag = std::atomic_int{}, guest_page_base, &status]() mutable noexcept {
                x86::cpu_call_once(
                    once_flag,
                    [guest_page_base, &status]() noexcept {
                        if constexpr (_PageTraits::page_size_v == 1_gb_size_v) {
                            status = siren_hypercalls::ept_uncommit_1gb_page(guest_page_base.storage);
                        } else if constexpr (_PageTraits::page_size_v == 2_mb_size_v) {
                            status = siren_hypercalls::ept_uncommit_2mb_page(guest_page_base.storage);
                        } else if constexpr (_PageTraits::page_size_v == 4_kb_size_v) {
                            status = siren_hypercalls::ept_uncommit_4kb_page(guest_page_base.storage);
                        } else {
                            [] <bool dummy_flag = false>() { static_assert(dummy_flag, "Only 1gb, 2mb and 4kb page are allowed."); }();
                        }
                    }
                );

                siren_hypercalls::ept_flush();
            }
        );

        return status;
    }

    template
    [[nodiscard]]
    status_t ept_manager::guest_uncommit_page<x86::for_1gb_page_t>(x86::guest_physical_address_t guest_page_base) noexcept;

    template
    [[nodiscard]]
    status_t ept_manager::guest_uncommit_page<x86::for_2mb_page_t>(x86::guest_physical_address_t guest_page_base) noexcept;

    template
    [[nodiscard]]
    status_t ept_manager::guest_uncommit_page<x86::for_4kb_page_t>(x86::guest_physical_address_t guest_page_base) noexcept;

}
