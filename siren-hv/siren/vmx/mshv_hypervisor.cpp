#include "mshv_hypervisor.hpp"
#include "mshv_virtual_cpu.hpp"
#include "ept_management.hpp"

#include "../../x86/multiprocessor.hpp"

namespace siren::hypervisors::vmx {

    mshv_hypervisor::mshv_hypervisor() noexcept = default;

    mshv_hypervisor::~mshv_hypervisor() {
        if (m_virtual_cpus) {
            stop();
        }
    }

    [[nodiscard]]
    status_t mshv_hypervisor::init() noexcept {
        auto status = m_ept_manager.init();
        if (status.fails()) {
            return status;
        }

        auto ept_manager = make_managed_object<false, ept_management>();
        if (!ept_manager) {
            return status_t::insufficient_memory_v;
        }

        auto virtual_cpus = make_managed_object<false, mshv_virtual_cpu[]>(x86::active_cpu_count());
        if (!virtual_cpus) {
            return status_t::insufficient_memory_v;
        }

        for (size_t i = 0; i < virtual_cpus.get_length(); ++i) {
            std::construct_at(std::addressof(virtual_cpus[i]), *this, static_cast<uint32_t>(i));
        }

        for (size_t i = 0; i < virtual_cpus.get_length(); ++i) {
            status = virtual_cpus[i].init();
            if (status.fails()) {
                return status;
            }
        }

        m_virtual_cpus = std::move(virtual_cpus);

        return status_t::success_v;
    }

    [[nodiscard]]
    implementation_e mshv_hypervisor::get_implementation() const noexcept {
        return implementation_e::X86_VMX_MICROSOFT_HV;
    }

    [[nodiscard]]
    abstract_virtual_cpu& mshv_hypervisor::get_virtual_cpu(size_t i) noexcept {
        return m_virtual_cpus[i];
    }

    [[nodiscard]]
    const abstract_virtual_cpu& mshv_hypervisor::get_virtual_cpu(size_t i) const noexcept {
        return m_virtual_cpus[i];
    }

    [[nodiscard]]
    size_t mshv_hypervisor::get_virtual_cpu_count() const noexcept {
        return m_virtual_cpus.get_length();
    }

    void mshv_hypervisor::start() noexcept {
        x86::ipi_broadcast([this]() noexcept { get_virtual_cpu(x86::current_cpu_index()).start(); });
    }

    void mshv_hypervisor::stop() noexcept {
        x86::ipi_broadcast([this]() noexcept { get_virtual_cpu(x86::current_cpu_index()).stop(); });
    }

}
