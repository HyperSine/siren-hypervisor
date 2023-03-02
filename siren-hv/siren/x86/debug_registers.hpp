#pragma once
#include "address_space.hpp"

namespace siren::x86 {
    struct dr0_long_t {
        union {
            uint64_t storage;
            struct {
                laddr64_t linear_address;
            } semantics;
        };
    };

    static_assert(sizeof(dr0_long_t) == 8);
    static_assert(sizeof(dr0_long_t::storage) == sizeof(dr0_long_t::semantics));

    struct dr0_legacy_t {
        union {
            uint32_t storage;
            struct {
                laddr32_t linear_address;
            } semantics;
        };
    };

    static_assert(sizeof(dr0_legacy_t) == 4);
    static_assert(sizeof(dr0_legacy_t::storage) == sizeof(dr0_legacy_t::semantics));

#if defined(_M_X64)
    using dr0_t = dr0_long_t;
#elif defined(_M_IX86)
    using dr0_t = dr0_legacy_t;
#endif

    struct dr1_long_t {
        union {
            uint64_t storage;
            struct {
                laddr64_t linear_address;
            } semantics;
        };
    };

    static_assert(sizeof(dr1_long_t) == 8);
    static_assert(sizeof(dr1_long_t::storage) == sizeof(dr1_long_t::semantics));

    struct dr1_legacy_t {
        union {
            uint32_t storage;
            struct {
                laddr32_t linear_address;
            } semantics;
        };
    };

    static_assert(sizeof(dr1_legacy_t) == 4);
    static_assert(sizeof(dr1_legacy_t::storage) == sizeof(dr1_legacy_t::semantics));

#if defined(_M_X64)
    using dr1_t = dr1_long_t;
#elif defined(_M_IX86)
    using dr1_t = dr1_legacy_t;
#endif

    struct dr2_long_t {
        union {
            uint64_t storage;
            struct {
                laddr64_t linear_address;
            } semantics;
        };
    };

    static_assert(sizeof(dr2_long_t) == 8);
    static_assert(sizeof(dr2_long_t::storage) == sizeof(dr2_long_t::semantics));

    struct dr2_legacy_t {
        union {
            uint32_t storage;
            struct {
                laddr32_t linear_address;
            } semantics;
        };
    };

    static_assert(sizeof(dr2_legacy_t) == 4);
    static_assert(sizeof(dr2_legacy_t::storage) == sizeof(dr2_legacy_t::semantics));

#if defined(_M_X64)
    using dr2_t = dr2_long_t;
#elif defined(_M_IX86)
    using dr2_t = dr2_legacy_t;
#endif

    struct dr3_long_t {
        union {
            uint64_t storage;
            struct {
                laddr64_t linear_address;
            } semantics;
        };
    };

    static_assert(sizeof(dr3_long_t) == 8);
    static_assert(sizeof(dr3_long_t::storage) == sizeof(dr3_long_t::semantics));

    struct dr3_legacy_t {
        union {
            uint32_t storage;
            struct {
                laddr32_t linear_address;
            } semantics;
        };
    };

    static_assert(sizeof(dr3_legacy_t) == 4);
    static_assert(sizeof(dr3_legacy_t::storage) == sizeof(dr3_legacy_t::semantics));

#if defined(_M_X64)
    using dr3_t = dr3_long_t;
#elif defined(_M_IX86)
    using dr3_t = dr3_legacy_t;
#endif

    struct dr6_long_t {
        union {
            uint64_t storage;
            // todo: semantics
        };
    };

    static_assert(sizeof(dr6_long_t) == 8);
    //static_assert(sizeof(dr6_long_t::storage) == sizeof(dr6_long_t::semantics));

    struct dr6_legacy_t {
        union {
            uint32_t storage;
            // todo: semantics
        };
    };

    static_assert(sizeof(dr6_legacy_t) == 4);
    //static_assert(sizeof(dr6_legacy_t::storage) == sizeof(dr6_legacy_t::semantics));

#if defined(_M_X64)
    using dr6_t = dr6_long_t;
#elif defined(_M_IX86)
    using dr6_t = dr6_legacy_t;
#endif

    struct dr7_long_t {
        union {
            uint64_t storage;
            // todo: semantics
        };
    };

    static_assert(sizeof(dr7_long_t) == 8);
    //static_assert(sizeof(dr7_long_t::storage) == sizeof(dr7_long_t::semantics));

    struct dr7_legacy_t {
        union {
            uint32_t storage;
            // todo: semantics
        };
    };

    static_assert(sizeof(dr7_legacy_t) == 4);
    //static_assert(sizeof(dr7_legacy_t::storage) == sizeof(dr7_legacy_t::semantics));

#if defined(_M_X64)
    using dr7_t = dr7_long_t;
#elif defined(_M_IX86)
    using dr7_t = dr7_legacy_t;
#endif

    template<typename Ty = dr0_t>
    [[nodiscard]]
    Ty read_dr0() noexcept {
        return Ty{ .storage = __readdr(0) };
    }

    template<typename Ty = dr0_t>
    void write_dr0(Ty value) noexcept {
        return __writedr(0, value.storage);
    }

    template<typename Ty = dr1_t>
    [[nodiscard]]
    Ty read_dr1() noexcept {
        return Ty{ .storage = __readdr(1) };
    }

    template<typename Ty = dr1_t>
    void write_dr1(Ty value) noexcept {
        return __writedr(1, value.storage);
    }

    template<typename Ty = dr2_t>
    [[nodiscard]]
    Ty read_dr2() noexcept {
        return Ty{ .storage = __readdr(2) };
    }

    template<typename Ty = dr2_t>
    void write_dr2(Ty value) noexcept {
        return __writedr(2, value.storage);
    }

    template<typename Ty = dr3_t>
    [[nodiscard]]
    Ty read_dr3() noexcept {
        return Ty{ .storage = __readdr(3) };
    }

    template<typename Ty = dr3_t>
    void write_dr3(Ty value) noexcept {
        return __writedr(3, value.storage);
    }

    template<typename Ty = dr6_t>
    [[nodiscard]]
    Ty read_dr6() noexcept {
        return Ty{ .storage = __readdr(6) };
    }

    template<typename Ty = dr6_t>
    void write_dr6(Ty value) noexcept {
        return __writedr(6, value.storage);
    }

    template<typename Ty = dr7_t>
    [[nodiscard]]
    auto read_dr7() noexcept {
        return Ty{ .storage = __readdr(7) };
    }

    template<typename Ty = dr7_t>
    void write_dr7(Ty value) noexcept {
        return __writedr(7, value.storage);
    }
}
