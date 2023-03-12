#pragma once
#include <concepts>
#include <type_traits>
#include <limits>

namespace siren {
    template<typename Ty, typename... Tys>
    concept one_of = std::disjunction_v<std::is_same<Ty, Tys>...>;

    template<typename Ty, typename... Tys>
    concept any_of = one_of<Ty, Tys...>;

    template<template<typename Ty> typename TraitTy, typename... Tys>
    concept all_satisfy = std::conjunction_v<TraitTy<Tys>...>;

    template<template<typename Ty> typename TraitTy, typename... Tys>
    concept none_satisfy = std::conjunction_v<std::negation<TraitTy<Tys>>...>;

    template<template<typename Ty> typename TraitTy, typename... Tys>
    concept some_satisfy = !none_satisfy<TraitTy, Tys...>;

    template<template<typename Ty> typename TraitTy, typename... Tys>
    concept some_unsatisfy = !all_satisfy<TraitTy, Tys...>;

    template<typename Ty, template<typename> typename... TraitTys>
    concept satisfy_all = std::conjunction_v<TraitTys<Ty>...>;

    template<typename Ty, template<typename> typename... TraitTys>
    concept satisfy_none = std::conjunction_v<std::negation<TraitTys<Ty>>...>;

    template<typename Ty, template<typename> typename... TraitTys>
    concept satisfy_some = !satisfy_none<Ty, TraitTys...>;

    template<typename Ty, template<typename> typename... TraitTys>
    concept unsatisfy_some = !satisfy_all<Ty, TraitTys...>;

    template<typename Ty, size_t Alignment>
    concept aligned_exact = alignof(Ty) == Alignment;

    template<typename Ty, size_t Alignment>
    concept aligned_most = alignof(Ty) <= Alignment;

    template<typename Ty, size_t Alignment>
    concept aligned_least = alignof(Ty) >= Alignment;

    template<typename Ty, size_t Bits>
    concept integral_exact = std::integral<Ty> && sizeof(Ty) * CHAR_BIT == Bits;

    template<typename Ty, size_t Bits>
    concept integral_most = std::integral<Ty> && sizeof(Ty) * CHAR_BIT <= Bits;

    template<typename Ty, size_t Bits>
    concept integral_least = std::integral<Ty> && sizeof(Ty) * CHAR_BIT >= Bits;

    template<typename Ty, size_t Bits = 0>
    concept signed_integral_exact = std::signed_integral<Ty> && sizeof(Ty) * CHAR_BIT == Bits;

    template<typename Ty, size_t Bits>
    concept signed_integral_most = std::signed_integral<Ty> && sizeof(Ty) * CHAR_BIT <= Bits;

    template<typename Ty, size_t Bits>
    concept signed_integral_least = std::signed_integral<Ty> && sizeof(Ty) * CHAR_BIT >= Bits;

    template<typename Ty, size_t Bits>
    concept unsigned_integral_exact = std::unsigned_integral<Ty> && sizeof(Ty) * CHAR_BIT == Bits;

    template<typename Ty, size_t Bits>
    concept unsigned_integral_most = std::unsigned_integral<Ty> && sizeof(Ty) * CHAR_BIT <= Bits;

    template<typename Ty, size_t Bits>
    concept unsigned_integral_least = std::unsigned_integral<Ty> && sizeof(Ty) * CHAR_BIT >= Bits;

    template<typename Ty>
    concept non_dimensional = !std::is_array_v<Ty>;

    template<typename Ty>
    concept one_dimensional = std::rank_v<Ty> == 1;

    template<typename Ty>
    concept multi_dimensional = std::rank_v<Ty> > 1;

    template<typename Ty>
    concept decayed = std::same_as<Ty, std::decay_t<Ty>>;

    template<typename Ty>
    concept const_qualified = std::same_as<Ty, std::add_const_t<Ty>>;

    template<typename Ty>
    concept volatile_qualified = std::same_as<Ty, std::add_volatile_t<Ty>>;

    template<typename Ty>
    concept cv_qualified = std::same_as<Ty, std::add_cv_t<Ty>>;

    template<typename Ty>
    concept no_const_qualified = std::same_as<Ty, std::remove_const_t<Ty>>;

    template<typename Ty>
    concept no_volatile_qualified = std::same_as<Ty, std::remove_volatile_t<Ty>>;

    template<typename Ty>
    concept no_cv_qualified = std::same_as<Ty, std::remove_cv_t<Ty>>;

    template<typename Ty, template<typename> typename... ModifierTys>
    struct _Type_modification_compose;

    template<typename Ty, template<typename> typename FirstModifierTy>
    struct _Type_modification_compose<Ty, FirstModifierTy> {
        using type = typename FirstModifierTy<Ty>::type;
    };

    template<typename Ty, template<typename> typename FirstModifierTy, template<typename> typename... LeftModifierTy>
    struct _Type_modification_compose<Ty, FirstModifierTy, LeftModifierTy...> {
        using type = typename _Type_modification_compose<typename FirstModifierTy<Ty>::type, LeftModifierTy...>::type;
    };

    template<typename Ty, template<typename> typename... ModifierTys>
    concept same_after = std::same_as<Ty, typename _Type_modification_compose<Ty, ModifierTys...>::type>;

    template<typename Ty, typename Uy, template<typename> typename... ModifierTys>
    concept same_as_after = std::same_as<Uy, typename _Type_modification_compose<Ty, ModifierTys...>::type>;
}
