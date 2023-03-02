#include "once_flag.hpp"
#include <wdm.h>

namespace siren {
    void once_flag_t::yield() noexcept { YieldProcessor(); }
}
