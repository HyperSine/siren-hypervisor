#include "debugging.hpp"
#include "utility.hpp"
#include <wdm.h>

namespace siren {
    void invoke_debugger() noexcept {
        DbgBreakPoint();
    }

    [[noreturn]]
    void invoke_debugger_noreturn() noexcept {
        DbgBreakPoint();
        unreachable();
    }

    [[noreturn]]
    void raise_assertion_failure() noexcept {
        DbgRaiseAssertionFailure();
    }
}
