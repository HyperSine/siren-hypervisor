#pragma once
#include <memory>       // need std::construct_at, std::destroy_at
#include <utility>      // need std::forward, std::move
#include "concepts.hpp"

namespace siren {
    template<typename ErrTy>
    class unexpected {
    private:
        ErrTy m_error;

    public:
        template<typename ErrTy2 = ErrTy>
            requires std::is_constructible_v<ErrTy, ErrTy2&&>
        constexpr explicit unexpected(ErrTy2&& err) noexcept(std::is_nothrow_constructible_v<ErrTy, ErrTy2&&>)
            : m_error{ std::forward<ErrTy2>(err) } {}

        template<typename... ArgTys>
            requires std::is_constructible_v<ErrTy, ArgTys&&...>
        constexpr explicit unexpected(std::in_place_t, ArgTys&&... args) noexcept(std::is_nothrow_constructible_v<ErrTy, ArgTys&&...>)
            : m_error{ std::forward<ArgTys>(args)... } {}

        [[nodiscard]]
        constexpr ErrTy& error() & noexcept {
            return m_error;
        }

        [[nodiscard]]
        constexpr const ErrTy& error() const& noexcept {
            return m_error;
        }

        [[nodiscard]]
        constexpr ErrTy&& error() && noexcept {
            return std::move(m_error);
        }

        [[nodiscard]]
        constexpr const ErrTy&& error() const&& noexcept {
            return std::move(m_error);
        }

        template<typename ErrTy2>
        [[nodiscard]]
        friend constexpr bool operator==(const unexpected& lhs, const unexpected<ErrTy2>& rhs) noexcept(noexcept(static_cast<bool>(lhs.m_error == rhs.error()))) {
            return static_cast<bool>(lhs.m_error == rhs.error());
        }
    };

    template <class ErrTy>
    unexpected(ErrTy) -> unexpected<ErrTy>;

    template<typename Ty, typename ErrTy>
    class expected {
    public:
        using value_type = Ty;
        using error_type = ErrTy;
        using unexpected_type = unexpected<ErrTy>;

    private:
        union {
            Ty m_value;
            ErrTy m_error;
        };
        bool m_has_value;

    public:
        constexpr expected() noexcept(std::is_nothrow_default_constructible_v<Ty>) requires std::is_default_constructible_v<Ty> 
            : m_value{}, m_has_value { true } {}

        template<typename Ty2 = Ty>
            requires std::is_constructible_v<Ty, Ty2>
        constexpr expected(Ty2&& value) noexcept(std::is_nothrow_constructible_v<Ty, Ty2>)
            : m_value{ std::forward<Ty2>(value) }, m_has_value{ true } {}

        // copy constructor
        constexpr expected(const expected& other) requires all_satisfy<std::is_trivially_copy_constructible, Ty, ErrTy> = default;

        // copy constructor
        constexpr expected(const expected& other) noexcept(all_satisfy<std::is_nothrow_copy_constructible, Ty, ErrTy>) 
            requires some_unsatisfy<std::is_trivially_copy_constructible, Ty, ErrTy> && all_satisfy<std::is_copy_constructible, Ty, ErrTy>
            : m_has_value{ other.m_has_value }
        {
            if (m_has_value) {
                std::construct_at(std::addressof(m_value), other.m_value);
            } else {
                std::construct_at(std::addressof(m_error), other.m_error);
            }
        }

        // move constructor
        constexpr expected(expected&& other) requires all_satisfy<std::is_trivially_move_constructible, Ty, ErrTy> = default;

        // move constructor
        constexpr expected(expected&& other) noexcept(all_satisfy<std::is_nothrow_move_constructible, Ty, ErrTy>)
            requires some_unsatisfy<std::is_trivially_move_constructible, Ty, ErrTy> && all_satisfy<std::is_move_constructible, Ty, ErrTy>
            : m_has_value{ other.m_has_value }
        {
            if (m_has_value) {
                std::construct_at(std::addressof(m_value), std::move(other.m_value));
            } else {
                std::construct_at(std::addressof(m_error), std::move(other.m_error));
            }
        }

        template<typename Ty2, typename ErrTy2>
            requires std::is_constructible_v<Ty, const Ty2&> && std::is_constructible_v<ErrTy, const ErrTy2&>
        constexpr expected(const expected<Ty2, ErrTy2>& other) noexcept(std::is_nothrow_constructible_v<Ty, const Ty2&> && std::is_nothrow_constructible_v<ErrTy, const ErrTy2&>)
            : m_has_value{ other.m_has_value }
        {
            if (m_has_value) {
                std::construct_at(std::addressof(m_value), other.m_value);
            } else {
                std::construct_at(std::addressof(m_error), other.m_error);
            }
        }

        template<typename Ty2, typename ErrTy2>
            requires std::is_constructible_v<Ty, Ty2&&> && std::is_constructible_v<ErrTy, ErrTy2&&>
        constexpr expected(expected<Ty2, ErrTy2>&& other) noexcept(std::is_nothrow_constructible_v<Ty, Ty2&&> && std::is_nothrow_constructible_v<ErrTy, ErrTy2&&>)
            : m_has_value{ other.m_has_value }
        {
            if (m_has_value) {
                std::construct_at(std::addressof(m_value), std::move(other.m_value));
            } else {
                std::construct_at(std::addressof(m_error), std::move(other.m_error));
            }
        }

        template<typename ErrTy2>
            requires std::is_constructible_v<ErrTy, const ErrTy2&>
        constexpr expected(const unexpected<ErrTy2>& err) noexcept(std::is_nothrow_constructible_v<ErrTy, const ErrTy2&>)
            : m_error{ err.error() }, m_has_value{ false } {}

        template<typename ErrTy2>
            requires std::is_constructible_v<ErrTy, ErrTy2&&>
        constexpr expected(unexpected<ErrTy2>&& err) noexcept(std::is_nothrow_constructible_v<ErrTy, ErrTy2&&>)
            : m_error{ std::move(err.error()) }, m_has_value{ false } {}

        template <typename... ArgTys>
            requires std::is_constructible_v<Ty, ArgTys&&...>
        constexpr expected(std::in_place_t, ArgTys&&... args) noexcept(std::is_nothrow_constructible_v<Ty, ArgTys&&...>)
            : m_value{ std::forward<ArgTys>(args)... }, m_has_value{ true } {}

        constexpr ~expected() requires all_satisfy<std::is_trivially_destructible, Ty, ErrTy> = default;

        constexpr ~expected() noexcept 
            requires some_unsatisfy<std::is_trivially_destructible, Ty, ErrTy>
        {
            if (m_has_value) {
                std::destroy_at(std::addressof(m_value));
            } else {
                std::destroy_at(std::addressof(m_error));
            }
        }

        constexpr expected& operator=(const expected& other) noexcept(all_satisfy<std::is_nothrow_copy_constructible, Ty, ErrTy> && all_satisfy<std::is_nothrow_copy_assignable, Ty, ErrTy>)
            requires all_satisfy<std::is_copy_constructible, Ty, ErrTy> && all_satisfy<std::is_copy_assignable, Ty, ErrTy>
        {
            if (m_has_value && other.m_has_value) {
                m_value = other.m_value;
            } else if (m_has_value) {
                std::destroy_at(std::addressof(m_value));
                std::construct_at(std::addressof(m_error), other.m_error);
            } else if (other.m_has_value) {
                std::destroy_at(std::addressof(m_error));
                std::construct_at(std::addressof(m_value), other.m_value);
            } else {
                m_error = other.m_error;
            }

            m_has_value = other.m_has_value;

            return *this;
        }

        constexpr expected& operator=(expected&& other) noexcept(all_satisfy<std::is_nothrow_move_constructible, Ty, ErrTy> && all_satisfy<std::is_nothrow_move_assignable, Ty, ErrTy>)
            requires all_satisfy<std::is_move_constructible, Ty, ErrTy> && all_satisfy<std::is_move_assignable, Ty, ErrTy>
        {
            if (m_has_value && other.m_has_value) {
                m_value = std::move(other.m_value);
            } else if (m_has_value) {
                std::destroy_at(std::addressof(m_value));
                std::construct_at(std::addressof(m_error), std::move(other.m_error));
            } else if (other.m_has_value) {
                std::destroy_at(std::addressof(m_error));
                std::construct_at(std::addressof(m_value), std::move(other.m_value));
            } else {
                m_error = std::move(other.m_error);
            }
            m_has_value = other.m_has_value;
            return *this;
        }

        template<typename Ty2>
            requires std::is_constructible_v<Ty, Ty2&&> && std::is_assignable_v<Ty&, Ty2&&>
        constexpr expected& operator=(Ty2&& val) noexcept(std::is_nothrow_constructible_v<Ty, Ty2&&> && std::is_nothrow_assignable_v<Ty&, Ty2&&>) {
            if (m_has_value) {
                m_value = std::forward<Ty2>(val);
            } else {
                std::destroy_at(std::addressof(m_error));
                std::construct_at(std::addressof(m_value), std::forward<Ty2>(val));
                m_has_value = true;
            }
            return *this;
        }

        template<typename ErrTy2>
            requires std::is_constructible_v<ErrTy, const ErrTy2&> && std::is_assignable_v<ErrTy&, const ErrTy2&>
        constexpr expected& operator=(const unexpected<ErrTy2>& err) noexcept(std::is_nothrow_constructible_v<ErrTy, const ErrTy2&> && std::is_nothrow_assignable_v<ErrTy&, const ErrTy2&>) {
            if (m_has_value) {
                std::destroy_at(std::addressof(m_value));
                std::construct_at(std::addressof(m_error), err.error());
                m_has_value = false;
            } else {
                m_error = err.error();
            }
            return *this;
        }

        template<typename ErrTy2>
            requires std::is_constructible_v<ErrTy, ErrTy2&&> && std::is_assignable_v<ErrTy&, ErrTy2&&>
        constexpr expected& operator=(unexpected<ErrTy2>&& err) noexcept(std::is_nothrow_constructible_v<ErrTy, ErrTy2&&> && std::is_nothrow_assignable_v<ErrTy&, ErrTy2&&>) {
            if (m_has_value) {
                std::destroy_at(std::addressof(m_value));
                std::construct_at(std::addressof(m_error), std::move(err.error()));
                m_has_value = false;
            } else {
                m_error = std::move(err.error());
            }
            return *this;
        }

        [[nodiscard]]
        constexpr Ty* operator->() noexcept {
            return std::addressof(m_value);
        }

        [[nodiscard]]
        constexpr const Ty* operator->() const noexcept {
            return std::addressof(m_value);
        }

        [[nodiscard]]
        constexpr Ty& operator*() & noexcept {
            return m_value;
        }

        [[nodiscard]]
        constexpr const Ty& operator*() const& noexcept {
            return m_value;
        }

        [[nodiscard]]
        constexpr Ty&& operator*() && noexcept {
            return std::move(m_value);
        }

        [[nodiscard]]
        constexpr const Ty&& operator*() const&& noexcept {
            return std::move(m_value);
        }

        template<typename Ty2, typename ErrTy2>
            requires std::negation_v<std::is_void<Ty2>>
        [[nodiscard]]
        friend constexpr bool operator==(const expected& lhs, const expected<Ty2, ErrTy2>& rhs) 
            noexcept(noexcept(static_cast<bool>(lhs.m_value == rhs.value())) && noexcept(static_cast<bool>(lhs.m_error == rhs.error())))
        {
            if (lhs.m_has_value == rhs.has_value()) {
                return lhs.m_has_value ? static_cast<bool>(lhs.m_value == rhs.value()) : static_cast<bool>(lhs.m_error == rhs.error());
            } else {
                return false;
            }
        }

        template<typename Ty2>
        [[nodiscard]]
        friend constexpr bool operator==(const expected& lhs, const Ty2& rhs) noexcept(noexcept(static_cast<bool>(lhs.m_value == rhs))) {
            return lhs.m_has_value && static_cast<bool>(lhs.m_value == rhs);
        }

        template<typename ErrTy2>
        [[nodiscard]]
        friend constexpr bool operator==(const expected& lhs, const unexpected<ErrTy2>& rhs) noexcept(noexcept(static_cast<bool>(lhs.m_error == rhs.error()))) {
            return !lhs.m_has_value && static_cast<bool>(lhs.m_error == rhs.error());
        }

        [[nodiscard]]
        constexpr bool has_value() const noexcept {
            return m_has_value;
        }

        [[nodiscard]]
        constexpr Ty& value() & noexcept {
            return m_value;
        }

        [[nodiscard]]
        constexpr const Ty& value() const& noexcept {
            return m_value;
        }

        [[nodiscard]]
        constexpr Ty&& value() && noexcept {
            return std::move(m_value);
        }

        [[nodiscard]]
        constexpr const Ty&& value() const&& noexcept {
            return std::move(m_value);
        }

        template<std::convertible_to<Ty> Ty2>
            requires std::is_copy_constructible_v<Ty>
        [[nodiscard]]
        constexpr Ty value_or(Ty2&& default_value) const& noexcept(std::is_nothrow_copy_constructible_v<Ty> && std::is_nothrow_convertible_v<Ty2, Ty>) {
            if (m_has_value) {
                return m_value;
            } else {
                return static_cast<Ty>(std::forward<Ty2>(default_value));
            }
        }

        template<std::convertible_to<Ty> Ty2>
            requires std::is_move_constructible_v<Ty>
        [[nodiscard]]
        constexpr Ty value_or(Ty2&& default_value) && noexcept(std::is_nothrow_move_constructible_v<Ty> && std::is_nothrow_convertible_v<Ty2, Ty>) {
            if (m_has_value) {
                return std::move(m_value);
            } else {
                return static_cast<Ty>(std::forward<Ty2>(default_value));
            }
        }

        [[nodiscard]]
        constexpr ErrTy& error() & noexcept {
            return m_error;
        }

        [[nodiscard]]
        constexpr ErrTy& error() const& noexcept {
            return m_error;
        }

        [[nodiscard]]
        constexpr ErrTy&& error() && noexcept {
            return std::move(m_error);
        }

        [[nodiscard]]
        constexpr const ErrTy&& error() const&& noexcept {
            return std::move(m_error);
        }

        template<typename... ArgTys>
            requires std::is_constructible_v<Ty, ArgTys&&...>
        constexpr Ty& emplace(ArgTys&&... args) noexcept(std::is_nothrow_constructible_v<Ty, ArgTys&&...>) {
            if (m_has_value) {
                std::destroy_at(std::addressof(m_value));
            } else {
                std::destroy_at(std::addressof(m_error));
                m_has_value = true;
            }
            return *std::construct_at(std::addressof(m_value), std::forward<ArgTys>(args)...);
        }
    };

    template<typename Ty, typename ErrTy>
        requires std::is_void_v<Ty>
    class expected<Ty, ErrTy> {
    public:
        using value_type = Ty;
        using error_type = ErrTy;
        using unexpected_type = unexpected<ErrTy>;

    private:
        union {
            ErrTy m_error;
        };
        bool m_has_value;

    public:
        constexpr expected() noexcept : m_has_value{ true } {}

        constexpr expected(const expected& other)
            requires std::is_trivially_copy_constructible_v<ErrTy> = default;

        constexpr expected(const expected& other) noexcept(std::is_nothrow_copy_constructible_v<ErrTy>)
            requires unsatisfy_some<ErrTy, std::is_trivially_copy_constructible> && std::is_copy_constructible_v<ErrTy>
            : m_has_value{ other.m_has_value }
        {
            if (!m_has_value) {
                std::construct_at(std::addressof(m_error), other.m_error);
            }
        }

        constexpr expected(expected&& other)
            requires std::is_trivially_move_constructible_v<ErrTy> = default;

        constexpr expected(expected&& other) noexcept(std::is_nothrow_move_constructible_v<ErrTy>) 
            requires unsatisfy_some<ErrTy, std::is_trivially_constructible> && std::is_move_constructible_v<ErrTy>
            : m_has_value{ other.m_has_value }
        {
            if (!m_has_value) {
                std::construct_at(std::addressof(m_error), std::move(other.m_error));
            }
        }

        template<typename Ty2, typename ErrTy2>
            requires std::is_void_v<Ty2> && std::is_constructible_v<ErrTy, const ErrTy2&>
        constexpr expected(const expected<Ty2, ErrTy2>& other) noexcept(std::is_nothrow_constructible_v<ErrTy, const ErrTy2&>)
            : m_has_value{ other.has_value() }
        {
            if (!m_has_value) {
                std::construct_at(std::addressof(m_error), other.error());
            }
        }

        template<typename Ty2, typename ErrTy2>
            requires std::is_void_v<Ty2> && std::is_constructible_v<ErrTy, ErrTy2&&>
        constexpr expected(expected<Ty2, ErrTy2>&& other) noexcept(std::is_nothrow_constructible_v<ErrTy, ErrTy2&&>)
            : m_has_value{ other.has_value() }
        {
            if (!m_has_value) {
                std::construct_at(std::addressof(m_error), std::move(other.error()));
            }
        }

        template<typename ErrTy2>
            requires std::is_constructible_v<ErrTy, const ErrTy2&>
        constexpr expected(const unexpected<ErrTy2>& other) noexcept(std::is_nothrow_constructible_v<ErrTy, const ErrTy2&>)
            : m_error{ other.error() }, m_has_value{false} {}

        template<typename ErrTy2>
            requires std::is_constructible_v<ErrTy, ErrTy2&&>
        constexpr expected(unexpected<ErrTy2>&& other) noexcept(std::is_nothrow_constructible_v<ErrTy, ErrTy2&&>)
            : m_error{ std::move(other.error()) }, m_has_value{ false } {}

        constexpr explicit expected(std::in_place_t) noexcept : m_has_value{ true } {}

        constexpr ~expected() noexcept requires std::is_trivially_destructible_v<ErrTy> = default;

        constexpr ~expected() noexcept requires some_unsatisfy<std::is_trivially_destructible, ErrTy> {
            if (m_has_value) {
                // nothing to do
            } else {
                std::destroy_at(std::addressof(m_error));
            }
        }

        constexpr expected& operator=(const expected& other) noexcept(satisfy_all<ErrTy, std::is_nothrow_copy_constructible, std::is_nothrow_copy_assignable>)
            requires std::is_copy_constructible_v<ErrTy> && std::is_copy_assignable_v<ErrTy>
        {
            if (m_has_value && other.m_has_value) {
                // nothing to do
            } else if (m_has_value) {
                std::construct_at(std::addressof(m_error), other.m_error);
                m_has_value = false;
            } else if (other.m_has_value) {
                std::destroy_at(std::addressof(m_error));
                m_has_value = true;
            } else {
                m_error = other.m_error;
            }
            return *this;
        }

        constexpr expected& operator=(expected&& other) noexcept(satisfy_all<ErrTy, std::is_nothrow_move_constructible, std::is_nothrow_move_assignable>)
            requires std::is_move_constructible_v<ErrTy>&& std::is_move_assignable_v<ErrTy>
        {
            if (m_has_value && other.m_has_value) {
                // nothing to do
            } else if (m_has_value) {
                std::construct_at(std::addressof(m_error), std::move(other.m_error));
                m_has_value = false;
            } else if (other.m_has_value) {
                std::destroy_at(std::addressof(m_error));
                m_has_value = true;
            } else {
                m_error = std::move(other.m_error);
            }
            return *this;
        }

        template<typename ErrTy2>
            requires std::is_constructible_v<ErrTy, const ErrTy2&> && std::is_assignable_v<ErrTy&, const ErrTy2&>
        constexpr expected& operator=(const unexpected<ErrTy2>& other) noexcept(std::is_nothrow_constructible_v<ErrTy, const ErrTy2&> && std::is_nothrow_assignable_v<ErrTy&, const ErrTy2&>) {
            if (m_has_value) {
                std::construct_at(std::addressof(m_error), other.error());
                m_has_value = false;
            } else {
                m_error = other.error();
            }
            return *this;
        }

        template<typename ErrTy2>
            requires std::is_constructible_v<ErrTy, ErrTy2&&> && std::is_assignable_v<ErrTy&, ErrTy2&&>
        constexpr expected& operator=(unexpected<ErrTy2>&& other) noexcept(std::is_nothrow_constructible_v<ErrTy, ErrTy2&&> && std::is_nothrow_assignable_v<ErrTy&, ErrTy2&&>) {
            if (m_has_value) {
                std::construct_at(std::addressof(m_error), std::move(other.error()));
                m_has_value = false;
            } else {
                m_error = std::move(other.error());
            }
            return *this;
        }

        template<typename Ty2, typename ErrTy2>
            requires std::is_void_v<Ty2>
        [[nodiscard]]
        friend constexpr bool operator==(const expected& lhs, const expected<Ty2, ErrTy2>& rhs) noexcept(noexcept(static_cast<bool>(lhs.m_error == rhs.error()))) {
            return lhs.m_has_value == rhs.has_value() && (lhs.m_has_value || static_cast<bool>(lhs.m_error == rhs.error()));
        }

        template<typename ErrTy2>
        [[nodiscard]]
        friend constexpr bool operator==(const expected& lhs, const unexpected<ErrTy2>& rhs) noexcept(noexcept(static_cast<bool>(lhs.m_error == rhs.error()))) {
            return !lhs.m_has_value && static_cast<bool>(lhs.m_error == rhs.error());
        }

        constexpr void emplace() noexcept {
            if (!m_has_value) {
                std::destroy_at(std::addressof(m_error));
                m_has_value = true;
            }
        }

        [[nodiscard]]
        constexpr bool has_value() const noexcept {
            return m_has_value;
        }

        constexpr void value() & noexcept {}

        constexpr void value() const& noexcept {}

        constexpr void value() && noexcept {}

        constexpr void value() const&& noexcept {};

        [[nodiscard]]
        constexpr ErrTy& error() & noexcept {
            return m_error;
        }

        [[nodiscard]]
        constexpr const ErrTy& error() const& noexcept {
            return m_error;
        }

        [[nodiscard]]
        constexpr ErrTy&& error() && noexcept {
            return std::move(m_error);
        }

        [[nodiscard]]
        constexpr const ErrTy&& error() const&& noexcept {
            return std::move(m_error);
        }
    };
}
