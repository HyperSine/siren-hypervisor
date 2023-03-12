#pragma once
#include <stdint.h>
#include "../literals.hpp"
#include "../utility.hpp"
#include "../nt_status.hpp"

namespace siren::vmx {
    using namespace ::siren::size_literals;

    struct alignas(4_KiB_size_v) msr_bitmap_t {
        enum class read_t : bool {};
        enum class write_t : bool {};

        static constexpr closed_interval_t low_msr_address_interval_v = { 0x00000000ui32, 0x00001fffu };
        static constexpr closed_interval_t high_msr_address_interval_v = { 0xc0000000ui32, 0xc0001fffu };

        uint8_t read_low[1024];
        uint8_t read_high[1024];
        uint8_t write_low[1024];
        uint8_t write_high[1024];

        [[nodiscard]]
        nt_status set(uint32_t msr_address, read_t read) noexcept;

        [[nodiscard]]
        nt_status set(uint32_t msr_address, write_t write) noexcept;

        [[nodiscard]]
        nt_status set(uint32_t msr_address, read_t read, write_t write) noexcept;

        [[nodiscard]]
        nt_status get(uint32_t msr_address, read_t& read) noexcept;

        [[nodiscard]]
        nt_status get(uint32_t msr_address, write_t& write) noexcept;

        [[nodiscard]]
        nt_status get(uint32_t msr_address, read_t& read, write_t& write) noexcept;
    };

    static_assert(sizeof(msr_bitmap_t) == 4_KiB_size_v);
    static_assert(alignof(msr_bitmap_t) == 4_KiB_size_v);
}
