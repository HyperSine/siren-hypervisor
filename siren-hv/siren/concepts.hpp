#pragma once
#include <concepts>
#include <type_traits>

namespace siren::concepts {
    template<typename Ty, typename... Tys>
    concept one_of = std::disjunction_v<std::is_same<Ty, Tys>...>;

    template<typename Ty, typename... Tys>
    concept any_of = one_of<Ty, Tys...>;
}
