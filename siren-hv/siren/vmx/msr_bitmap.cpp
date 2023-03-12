#include "msr_bitmap.hpp"
#include "../utility.hpp"

//#pragma warning(disable: 4296) // suppress `error C4296: '<=': expression is always true`

namespace siren::vmx {
    nt_status msr_bitmap_t::set(uint32_t msr_address, read_t read) noexcept {
        if (low_msr_address_interval_v.contains(msr_address)) {
            uint32_t delta = msr_address - low_msr_address_interval_v.min;

            uint32_t pos = delta / 8;
            uint8_t mask = 1u << (delta % 8);

            if (bool current_read = (read_low[pos] & mask) != 0; current_read != std::to_underlying(read)) {
                if (std::to_underlying(read)) {
                    read_low[pos] |= mask;
                } else {
                    read_low[pos] &= ~mask;
                }
            }

            return nt_status_success_v;
        } else if (high_msr_address_interval_v.contains(msr_address)) {
            uint32_t delta = msr_address - high_msr_address_interval_v.min;

            uint32_t pos = delta / 8;
            uint8_t mask = 1u << (delta % 8);

            if (bool current_read = (read_high[pos] & mask) != 0; current_read != std::to_underlying(read)) {
                if (std::to_underlying(read)) {
                    read_high[pos] |= mask;
                } else {
                    read_high[pos] &= ~mask;
                }
            }

            return nt_status_success_v;
        } else {
            return nt_status_invalid_parameter_v;
        }
    }

    nt_status msr_bitmap_t::set(uint32_t msr_address, write_t write) noexcept {
        if (low_msr_address_interval_v.contains(msr_address)) {
            uint32_t delta = msr_address - low_msr_address_interval_v.min;

            uint32_t pos = delta / 8;
            uint8_t mask = 1u << (delta % 8);

            if (bool current_write = (write_low[pos] & mask) != 0; current_write != std::to_underlying(write)) {
                if (std::to_underlying(write)) {
                    write_low[pos] |= mask;
                } else {
                    write_low[pos] &= ~mask;
                }
            }

            return nt_status_success_v;
        } else if (high_msr_address_interval_v.contains(msr_address)) {
            uint32_t delta = msr_address - high_msr_address_interval_v.min;

            uint32_t pos = delta / 8;
            uint8_t mask = 1u << (delta % 8);

            if (bool current_write = (write_high[pos] & mask) != 0; current_write != std::to_underlying(write)) {
                if (std::to_underlying(write)) {
                    write_high[pos] |= mask;
                } else {
                    write_high[pos] &= ~mask;
                }
            }

            return nt_status_success_v;
        } else {
            return nt_status_invalid_parameter_v;
        }
    }

    nt_status msr_bitmap_t::set(uint32_t msr_address, read_t read, write_t write) noexcept {
        if (low_msr_address_interval_v.contains(msr_address)) {
            uint32_t delta = msr_address - low_msr_address_interval_v.min;

            uint32_t pos = delta / 8;
            uint8_t mask = 1u << (delta % 8);

            if (bool current_read = (read_low[pos] & mask) != 0; current_read != std::to_underlying(read)) {
                if (std::to_underlying(read)) {
                    read_low[pos] |= mask;
                } else {
                    read_low[pos] &= ~mask;
                }
            }

            if (bool current_write = (write_low[pos] & mask) != 0; current_write != std::to_underlying(write)) {
                if (std::to_underlying(write)) {
                    write_low[pos] |= mask;
                } else {
                    write_low[pos] &= ~mask;
                }
            }

            return nt_status_success_v;
        } else if (high_msr_address_interval_v.contains(msr_address)) {
            uint32_t delta = msr_address - high_msr_address_interval_v.min;

            uint32_t pos = delta / 8;
            uint8_t mask = 1u << (delta % 8);

            if (bool current_read = (read_high[pos] & mask) != 0; current_read != std::to_underlying(read)) {
                if (std::to_underlying(read)) {
                    read_high[pos] |= mask;
                } else {
                    read_high[pos] &= ~mask;
                }
            }

            if (bool current_write = (write_high[pos] & mask) != 0; current_write != std::to_underlying(write)) {
                if (std::to_underlying(write)) {
                    write_high[pos] |= mask;
                } else {
                    write_high[pos] &= ~mask;
                }
            }

            return nt_status_success_v;
        } else {
            return nt_status_invalid_parameter_v;
        }
    }
}
