#pragma once

namespace siren {
    void invoke_debugger() noexcept;

    [[noreturn]]
    void invoke_debugger_noreturn() noexcept;

    [[noreturn]]
    void raise_assertion_failure() noexcept;
}
