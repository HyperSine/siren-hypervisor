#include "msr_bitmap.hpp"
#include "../address_space.hpp"

namespace siren::vmx {
    expected<void, nt_status> msr_bitmap::initialize() noexcept {
        auto expt_bitmap = allocate_unique<x86::vmx_msr_bitmap_t>(npaged_pool);
        if (expt_bitmap.has_value()) {
            m_bitmap = std::move(expt_bitmap.value());
            return {};
        } else {
            return unexpected{ expt_bitmap.error() };
        }
    }

    x86::paddr_t msr_bitmap::get_address() const noexcept {
        return get_physical_address(m_bitmap.get());
    }

    expected<void, nt_status> msr_bitmap::set(uint32_t msr_address, setting_flags flags) noexcept {
        if (low_msr_address_interval_v.contains(msr_address)) {
            uint32_t delta = msr_address - low_msr_address_interval_v.min;

            auto pos = delta / 8u;
            auto mask = static_cast<uint8_t>(1u << (delta % 8u));

            if (flags.activate_read) {
                uint8_t current_read = (m_bitmap->read_low[pos] & mask) != 0 ? 1 : 0;
                if (current_read != flags.read) {
                    m_bitmap->read_low[pos] ^= mask;
                }
            }

            if (flags.activate_write) {
                uint8_t current_write = (m_bitmap->write_low[pos] & mask) != 0 ? 1 : 0;
                if (current_write != flags.write) {
                    m_bitmap->write_low[pos] ^= mask;
                }
            }

            return {};
        } else if (high_msr_address_interval_v.contains(msr_address)) {
            uint32_t delta = msr_address - high_msr_address_interval_v.min;

            auto pos = delta / 8u;
            auto mask = static_cast<uint8_t>(1u << (delta % 8u));

            if (flags.activate_read) {
                uint8_t current_read = (m_bitmap->read_high[pos] & mask) != 0 ? 1 : 0;
                if (current_read != flags.read) {
                    m_bitmap->read_high[pos] ^= mask;
                }
            }

            if (flags.activate_write) {
                uint8_t current_write = (m_bitmap->write_high[pos] & mask) != 0 ? 1 : 0;
                if (current_write != flags.write) {
                    m_bitmap->write_high[pos] ^= mask;
                }
            }

            return {};
        } else {
            return unexpected{ nt_status_invalid_parameter_v };
        }
    }

    expected<msr_bitmap::setting_flags, nt_status> msr_bitmap::get(uint32_t msr_address) const noexcept {
        if (low_msr_address_interval_v.contains(msr_address)) {
            uint32_t delta = msr_address - low_msr_address_interval_v.min;

            auto pos = delta / 8u;
            auto mask = static_cast<uint8_t>(1u << (delta % 8u));

            return setting_flags{
                .activate_read = 1,
                .activate_write = 1,
                .read = (m_bitmap->read_low[pos] & mask) != 0 ? uint8_t{ 1 } : uint8_t{ 0 },
                .write = (m_bitmap->write_low[pos] & mask) != 0 ? uint8_t{ 1 } : uint8_t{ 0 },
            };
        } else if (high_msr_address_interval_v.contains(msr_address)) {
            uint32_t delta = msr_address - high_msr_address_interval_v.min;

            auto pos = delta / 8u;
            auto mask = static_cast<uint8_t>(1u << (delta % 8u));

            return setting_flags{
                .activate_read = 1,
                .activate_write = 1,
                .read = (m_bitmap->read_high[pos] & mask) != 0 ? uint8_t{ 1 } : uint8_t{ 0 },
                .write = (m_bitmap->write_high[pos] & mask) != 0 ? uint8_t{ 1 } : uint8_t{ 0 },
            };
        } else {
            return unexpected{ nt_status_invalid_parameter_v };
        }
    }
}
