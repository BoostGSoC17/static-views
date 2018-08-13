//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Implementation of boost::static_views::make_wrapper.

#ifndef BOOST_STATIC_VIEWS_DETAIL_WRAPPER_HPP
#define BOOST_STATIC_VIEWS_DETAIL_WRAPPER_HPP

#include <type_traits>

#include "../concepts.hpp"
#include "config.hpp"

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \brief Smart reference to ease the lifetime management stuff.
template <class T>
class wrapper;

template <class T>
struct is_wrapper : std::false_type {
};

template <class T>
struct is_wrapper<wrapper<T>> : std::true_type {
};

namespace detail {

template <class T, bool IsOwner, bool IsEmpty, class = void>
class wrapper_storage;

template <class T, bool IsEmpty>
class wrapper_storage<T, /*am I the owner of T?*/ false, IsEmpty> {

    static_assert(
        std::is_object<T>::value, BOOST_STATIC_VIEWS_BUG_MESSAGE);

  public:
    using value_type = T;
    using reference  = std::add_lvalue_reference_t<value_type>;
    using const_reference =
        std::add_lvalue_reference_t<std::add_const_t<value_type>>;

    explicit BOOST_STATIC_VIEWS_CONSTEXPR wrapper_storage(
        reference x) noexcept
        : _payload{&x}
    {
    }

    wrapper_storage(wrapper_storage const& other) noexcept = default;
    wrapper_storage(wrapper_storage&&) noexcept            = default;
    wrapper_storage& operator                              =(
        wrapper_storage const& other) noexcept = default;
    wrapper_storage& operator=(wrapper_storage&&) noexcept = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto const& get() const noexcept { return *_payload; }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto& get() noexcept { return *_payload; }

    BOOST_STATIC_VIEWS_CONSTEXPR
    explicit operator reference() noexcept { return get(); }

    BOOST_STATIC_VIEWS_CONSTEXPR
    explicit operator const_reference() const noexcept
    {
        return get();
    }

  private:
    value_type* _payload;
};

template <class T>
class wrapper_storage<T, /*am I the owner of T?*/ true,
    /*is T empty and non-final?*/ true> : public T {

    static_assert(
        std::is_object<T>::value, BOOST_STATIC_VIEWS_BUG_MESSAGE);
    static_assert(std::is_empty<T>::value && !std::is_final<T>::value,
        BOOST_STATIC_VIEWS_BUG_MESSAGE);

    using base = T;

  public:
    using value_type = T;
    using reference  = std::add_lvalue_reference_t<value_type>;
    using const_reference =
        std::add_lvalue_reference_t<std::add_const_t<value_type>>;

    // clang-format off
    template <class U
        BOOST_STATIC_VIEWS_REQUIRES(Constructible<base, U>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    wrapper_storage(U&& x)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_constructible<base, U>::value)
        : base{std::forward<U>(x)}
    {
    }
    // clang-format on

    wrapper_storage(wrapper_storage const& other) = default;
    wrapper_storage(wrapper_storage&&)            = default;
    wrapper_storage& operator                     =(
        wrapper_storage const& other) = default;
    wrapper_storage& operator=(wrapper_storage&&) = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto const& get() const& noexcept
    {
        return static_cast<base const&>(*this);
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto& get() & noexcept { return static_cast<base&>(*this); }

    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(_MoveConstructible<value_type, Dummy>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto get() && BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                     std::is_nothrow_move_constructible<base>::value)
    {
        return static_cast<base&&>(*this);
    }
    // clang-format on

    BOOST_STATIC_VIEWS_CONSTEXPR
    explicit operator reference() & noexcept { return get(); }

    BOOST_STATIC_VIEWS_CONSTEXPR
    explicit operator const_reference() const& noexcept
    {
        return get();
    }
};

template <class T>
class wrapper_storage<T, /*am I the owner of T?*/ true,
    /*is T empty and non-final?*/ false> {

    static_assert(
        std::is_object<T>::value, BOOST_STATIC_VIEWS_BUG_MESSAGE);
    static_assert(!std::is_empty<T>::value || std::is_final<T>::value,
        BOOST_STATIC_VIEWS_BUG_MESSAGE);

  public:
    using value_type = T;
    using reference  = std::add_lvalue_reference_t<value_type>;
    using const_reference =
        std::add_lvalue_reference_t<std::add_const_t<value_type>>;

    // clang-format off
    template <class U
        BOOST_STATIC_VIEWS_REQUIRES(Constructible<value_type, U>)
    explicit BOOST_STATIC_VIEWS_CONSTEXPR wrapper_storage(U&& x)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_constructible<value_type, U>::value)
        : _payload{std::forward<U>(x)}
    {
    }
    // clang-format on

    wrapper_storage(wrapper_storage const&) = default;
    wrapper_storage(wrapper_storage&&)      = default;
    wrapper_storage& operator=(wrapper_storage const&) = default;
    wrapper_storage& operator=(wrapper_storage&&) = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto const& get() const& noexcept { return _payload; }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto& get() & noexcept { return _payload; }

    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(_MoveConstructible<value_type, Dummy>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto get() && BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                      std::is_nothrow_move_constructible<value_type>::value)
    {
        return std::move(_payload);
    }
    // clang-format on

    BOOST_STATIC_VIEWS_CONSTEXPR
    explicit operator reference() & noexcept { return get(); }

    BOOST_STATIC_VIEWS_CONSTEXPR
    explicit operator const_reference() const& noexcept
    {
        return get();
    }

    // NOTE: This should instead be
    // explicit operator value_type() && ...
    // but buggy GCC disagrees :)
    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(_MoveConstructible<value_type, Dummy>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    explicit operator value_type&&() &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<value_type>::value)
    {
        return std::move(_payload);
    }
    // clang-format on

  private:
    value_type _payload;
};

template <class T>
using wrapper_base_t = wrapper_storage<std::remove_reference_t<T>,
    !std::is_lvalue_reference<T>::value,
    std::is_empty<std::remove_reference_t<T>>::value
        && !std::is_final<std::remove_reference_t<T>>::value>;

} // namespace detail

template <class T>
class wrapper : public detail::wrapper_base_t<T> {

    static_assert(!std::is_rvalue_reference<T>::value,
        "boost::static_views::wrapper: Please, do not use rvalue "
        "references as template parameters. If you want a `wrapper` "
        "to own an object of type `T`, use `wrapper<T>` instead.");

    static_assert(!std::is_void<std::remove_reference_t<T>>::value,
        "boost::static_views::wrapper: `void` is not supported. If "
        "you have a problem with that, please, submit an issue "
        "here " BOOST_STATIC_VIEWS_ISSUES_LINK ".");

    using base = detail::wrapper_base_t<T>;

  public:
    using typename base::const_reference;
    using typename base::reference;
    using typename base::value_type;

    // clang-format off
    template <class U
        BOOST_STATIC_VIEWS_REQUIRES(Constructible<base, U>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    wrapper(U&& x) BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                       std::is_nothrow_constructible<base, U>::value)
        : base{std::forward<U>(x)}
    {
    }
    // clang-format on

    wrapper(wrapper const&) = default;
    wrapper(wrapper&&)      = default;
    wrapper& operator=(wrapper const&) = default;
    wrapper& operator=(wrapper&&) = default;
};

namespace detail {

struct make_wrapper_impl {
    // clang-format off
    template <class T
        BOOST_STATIC_VIEWS_REQUIRES(
            std::is_lvalue_reference<T>::value || MoveConstructible<T>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(T&& x) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::is_nothrow_constructible<wrapper<T>, T>::value))
    // clang-format on
    {
        return wrapper<T>{std::forward<T>(x)};
    }
};

} // end namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(
    detail::make_wrapper_impl, make_wrapper)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DETAIL_WRAPPER_HPP
