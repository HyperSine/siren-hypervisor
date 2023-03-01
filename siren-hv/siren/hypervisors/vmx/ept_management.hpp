#pragma once
#include "../../siren_global.hpp"
#include "../../x86.memory_caching.hpp"
#include "../../x86.paging.hpp"
#include "memory_type_advisor.hpp"
#include "dynamic_ept.hpp"

namespace siren::hypervisors::vmx {

    class ept_management {
    private:
        memory_type_advisor m_memory_type_advisor;
        dynamic_ept m_dynamic_ept;

        template<unsigned _Level>
        [[nodiscard]]
        status_t build_ept_identity_map(x86::physical_address_t from_base, x86::physical_address_t max_physical_address) noexcept;

    public:
        ept_management() noexcept;

        ept_management(ept_management&& other) noexcept;

        ept_management& operator=(ept_management&& other) = delete;

        ~ept_management() noexcept;

        [[nodiscard]]
        status_t init() noexcept;

        [[nodiscard]]
        x86::physical_address_t get_pml4_physical_address() const noexcept;

        template<typename _PageTraits>
        [[nodiscard]]
        status_t host_commit_page(x86::guest_physical_address_t guest_page_base, x86::host_physical_address_t host_page_base, dynamic_ept::page_flags_t flags, bool no_interrupt) noexcept {
            auto host_page_memory_type = m_memory_type_advisor.get_best_memory_type(guest_page_base, _PageTraits::page_size_v - 1);
            
            if (host_page_memory_type.is_reserved()) {
                return status_t::ambiguous_memory_type_v;
            } else {
                flags.memory_type = host_page_memory_type.to_integral();
            }

            return m_dynamic_ept.commit_page<_PageTraits>(guest_page_base, host_page_base, flags, no_interrupt);
        }

        template<typename _PageTraits>
        [[nodiscard]]
        status_t guest_commit_page(x86::guest_physical_address_t guest_page_base, x86::host_physical_address_t host_page_base, dynamic_ept::page_flags_t flags) noexcept;

        template<typename _PageTraits>
        [[nodiscard]]
        status_t host_uncommit_page(x86::guest_physical_address_t guest_page_base) noexcept {
            return m_dynamic_ept.uncommit_page<_PageTraits>(guest_page_base);
        }

        template<typename _PageTraits>
        [[nodiscard]]
        status_t guest_uncommit_page(x86::guest_physical_address_t guest_page_base) noexcept;
    };

}
