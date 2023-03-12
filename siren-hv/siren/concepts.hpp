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

    template<typename Ty, template<typename Ty> typename... TraitTys>
    concept satisfy_all = std::conjunction_v<TraitTys<Ty>...>;

    template<typename Ty, template<typename Ty> typename... TraitTys>
    concept satisfy_none = std::conjunction_v<std::negation<TraitTys<Ty>>...>;

    template<typename Ty, template<typename Ty> typename... TraitTys>
    concept satisfy_some = !satisfy_none<Ty, TraitTys...>;

    template<typename Ty, template<typename Ty> typename... TraitTys>
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
}
