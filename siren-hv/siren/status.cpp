#include "status.hpp"
#include "utility.hpp"
#include <wdm.h>

namespace siren {
    template<>
    NTSTATUS status_t::native_error<NTSTATUS>() const noexcept {
        switch (code) {
            case status_success_v.code:                 return STATUS_SUCCESS;
            case status_access_denied_v.code:           return STATUS_ACCESS_DENIED;
            case status_invalid_argument_v.code:        return STATUS_INVALID_PARAMETER;
            case status_not_implemented_v.code:         return STATUS_NOT_IMPLEMENTED;
            case status_not_supported_v.code:           return STATUS_NOT_SUPPORTED;
            case status_insufficient_memory_v.code:     return STATUS_INSUFFICIENT_RESOURCES;
            case status_buffer_too_small_v.code:        return STATUS_BUFFER_TOO_SMALL;
            case status_not_exist_v.code:               return STATUS_NOT_FOUND;
            case status_ambiguous_memory_type_v.code:   return STATUS_UNSUCCESSFUL;
            default: unreachable();
        }
    }
}
