#include "siren_global.hpp"
#include <wdm.h>

namespace siren {
    
    template<>
    [[nodiscard]]
    NTSTATUS status_t::to_native_error<NTSTATUS>() const noexcept {
        switch (storage) {
            case success_v.storage:
                return STATUS_SUCCESS;
            case access_denied_v.storage:
                return STATUS_ACCESS_DENIED;
            case invalid_argument_v.storage:
                return STATUS_INVALID_PARAMETER;
            case not_implemented_v.storage:
                return STATUS_NOT_IMPLEMENTED;
            case not_supported_v.storage:
                return STATUS_NOT_SUPPORTED;
            case insufficient_memory_v.storage:
                return STATUS_INSUFFICIENT_RESOURCES;
            case buffer_too_small_v.storage:
                return STATUS_BUFFER_TOO_SMALL;
            case not_exist_v.storage:
                return STATUS_NOT_FOUND;
            case ambiguous_memory_type_v.storage:
                return STATUS_UNSUCCESSFUL;
            default:
                SIREN_UNREACHABLE();
        }
    }

    void invoke_debugger() noexcept {
        DbgBreakPoint();
    }

    void invoke_debugger_if(bool condition) noexcept {
        if (condition) {
            DbgBreakPoint();
        }
    }

    void invoke_debugger_if_not(bool condition) noexcept {
        if (!condition) {
            DbgBreakPoint();
        }
    }

    [[noreturn]]
    void invoke_debugger_noreturn() noexcept {
        DbgBreakPoint();
        SIREN_UNREACHABLE();
    }

    [[noreturn]]
    void raise_assertion_failure() noexcept {
        DbgRaiseAssertionFailure();
    }

    [[noreturn]]
    void raise_assertion_failure_if(bool condition) noexcept {
        if (condition) {
            DbgRaiseAssertionFailure();
        }
    }

    [[noreturn]]
    void raise_assertion_failure_if_not(bool condition) noexcept {
        if (!condition) {
            DbgRaiseAssertionFailure();
        }
    }

}
