#include "mshv_hypervisor.hpp"
#include "mshv_virtual_cpu.hpp"
#include "mshv_vmexit_handler.hpp"
#include "siren_hypercalls.hpp"

#include "../multiprocessor.hpp"

#include "../x86/memory_caching.hpp"

#include <wdm.h>

namespace siren::vmx {
    expected<void, nt_status> mshv_hypervisor::setup_ept_identity_map(int level, x86::paddr_t start_hpa, x86::paddr_t max_physical_address) noexcept {
        expected<void, nt_status> retval;

        size_t entry_count;
        x86::paddr_t hpa_size_per_entry;

        switch (level) {
            case 1:
                entry_count = x86::ept_pt_t::length();
                hpa_size_per_entry = 4_Kiuz;
                break;
            case 2:
                entry_count = x86::ept_pdt_t::length();
                hpa_size_per_entry = x86::ept_pml4_t::length() * 4_Kiuz;
                break;
            case 3:
                entry_count = x86::ept_pdpt_t::length();
                hpa_size_per_entry = x86::ept_pdpt_t::length() * 2_Miuz;
                break;
            case 4:
                entry_count = x86::ept_pml4_t::length();
                hpa_size_per_entry = x86::ept_pml4_t::length() * 1_Giuz;
                break;
            default:
                std::unreachable();
        }

        for (size_t i = 0; i < entry_count; ++i) {
            x86::paddr_t hpa_base = start_hpa + i * hpa_size_per_entry;
            x86::memory_type_t hpa_memory_type;
            if (hpa_base <= max_physical_address) {
                switch (level) {
                    case 1:
                        hpa_memory_type = x86::memory_type_t::propose_for_page<4_Kiuz>(hpa_base);
                        if (hpa_memory_type.is_reserved()) {
                            retval = unexpected{ nt_status_unsuccessful_v };
                        } else {
                            KdPrint(("siren-hv: commit 4KiB-page, base = 0x%llx\n", hpa_base));
                            retval = m_dynamic_ept.commit_page(4_Kiuz, hpa_base, hpa_base, { .read_access = 1, .write_access = 1, .execute_access = 1, .memory_type = hpa_memory_type.value }, false);
                        }
                        break;
                    case 2:
                        hpa_memory_type = x86::memory_type_t::propose_for_page<2_Miuz>(hpa_base);
                        if (hpa_memory_type.is_reserved()) {
                            retval = setup_ept_identity_map(1, hpa_base, max_physical_address);
                        } else {
                            KdPrint(("siren-hv: commit 2MiB-page, base = 0x%llx\n", hpa_base));
                            retval = m_dynamic_ept.commit_page(2_Miuz, hpa_base, hpa_base, { .read_access = 1, .write_access = 1, .execute_access = 1, .memory_type = hpa_memory_type.value }, false);
                        }
                        break;
                    case 3:
                        hpa_memory_type = x86::memory_type_t::propose_for_page<1_Giuz>(hpa_base);
                        if (hpa_memory_type.is_reserved()) {
                            retval = setup_ept_identity_map(2, hpa_base, max_physical_address);
                        } else {
                            KdPrint(("siren-hv: commit 1GiB-page, base = 0x%llx\n", hpa_base));
                            retval = m_dynamic_ept.commit_page(1_Giuz, hpa_base, hpa_base, { .read_access = 1, .write_access = 1, .execute_access = 1, .memory_type = hpa_memory_type.value }, false);
                        }
                        break;
                    case 4:
                        retval = setup_ept_identity_map(3, hpa_base, max_physical_address);
                        break;
                    default:
                        std::unreachable();
                }
                if (retval.has_error()) {
                    break;
                }
            } else {
                break;
            }
        }

        return retval;
    }

    mshv_hypervisor::mshv_hypervisor() noexcept
        : m_dynamic_ept{}, m_msr_bitmap{}, m_virtual_cpus {} {}

    expected<void, nt_status> mshv_hypervisor::intialize() noexcept {
        expected<void, nt_status> retval;

        unique_npaged<mshv_virtual_cpu[]> virtual_cpus;

        retval = m_msr_bitmap.initialize();
        if (retval.has_error()) {
            return retval;
        }

        retval = m_dynamic_ept.initialize();
        if (retval.has_error()) {
            return retval;
        }

        retval = setup_ept_identity_map(4, 0, x86::get_max_physical_address());
        if (retval.has_error()) {
            return retval;
        }
        
        {
            auto expt_virtual_cpus = allocate_unique_uninitialized<mshv_virtual_cpu[]>(npaged_pool, active_cpu_count());
            if (expt_virtual_cpus.has_value()) {
                for (uint32_t i = 0; i < expt_virtual_cpus.value().get_deleter().count; ++i) {
                    std::construct_at(std::addressof(expt_virtual_cpus.value()[i]), this, i);
                }

                for (uint32_t i = 0; i < expt_virtual_cpus.value().get_deleter().count; ++i) {
                    retval = expt_virtual_cpus.value()[i].intialize();
                    if (retval.has_error()) {
                        return retval;
                    }
                }

                m_virtual_cpus = std::move(expt_virtual_cpus.value());
            } else {
                return unexpected{ expt_virtual_cpus.error() };
            }
        }

        return {};
    }

    implementation_e mshv_hypervisor::get_implementation() const noexcept {
        return implementation_e::X86_VMX_MICROSOFT_HV;
    }

    virtual_cpu* mshv_hypervisor::get_virtual_cpu(uint32_t cpu_index) noexcept {
        return cpu_index < m_virtual_cpus.get_deleter().count ? std::addressof(m_virtual_cpus[cpu_index]) : nullptr;
    }

    const virtual_cpu* mshv_hypervisor::get_virtual_cpu(uint32_t cpu_index) const noexcept {
        return cpu_index < m_virtual_cpus.get_deleter().count ? std::addressof(m_virtual_cpus[cpu_index]) : nullptr;
    }

    uint32_t mshv_hypervisor::get_virtual_cpu_count() const noexcept {
        return static_cast<uint32_t>(m_virtual_cpus.get_deleter().count);
    }

    void mshv_hypervisor::start() noexcept {
        ipi_broadcast([this]() noexcept { get_virtual_cpu(current_cpu_index())->start(); });
    }

    void mshv_hypervisor::stop() noexcept {
        ipi_broadcast([this]() noexcept { get_virtual_cpu(current_cpu_index())->stop(); });
    }
}
