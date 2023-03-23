#pragma once
#include "../expected.hpp"
#include "../nt_status.hpp"
#include "../memory.hpp"
#include "../utility.hpp"

#include "../x86/intel_vmx.hpp"

namespace siren::vmx {
    class msr_bitmap {
    private:
        unique_npaged<x86::vmx_msr_bitmap_t> m_bitmap;

    public:
        struct setting_flags {
            uint8_t activate_read : 1;
            uint8_t activate_write : 1;
            uint8_t read : 1;
            uint8_t write : 1;
        };

        static constexpr closed_interval_t low_msr_address_interval_v = { 0x00000000ui32, 0x00001fffui32 };
        static constexpr closed_interval_t high_msr_address_interval_v = { 0xc0000000ui32, 0xc0001fffui32 };

        msr_bitmap() noexcept = default;

        // copy constructor is not allowed
        msr_bitmap(const msr_bitmap&) = delete;

        // move constructor
        msr_bitmap(msr_bitmap&&) noexcept = default;

        // copy assignment is not allowed
        msr_bitmap& operator=(const msr_bitmap&) = delete;

        // move assignment
        msr_bitmap& operator=(msr_bitmap&&) noexcept = default;

        ~msr_bitmap() noexcept = default;

        [[nodiscard]]
        expected<void, nt_status> initialize() noexcept;

        [[nodiscard]]
        x86::paddr_t get_address() const noexcept;

        [[nodiscard]]
        expected<void, nt_status> set(uint32_t msr_address, setting_flags flags) noexcept;

        [[nodiscard]]
        expected<setting_flags, nt_status> get(uint32_t msr_address) const noexcept;
    };
}
