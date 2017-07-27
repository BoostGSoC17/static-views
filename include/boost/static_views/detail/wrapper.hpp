//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Implementation of boost::static_views::make_wrapper.

#ifndef BOOST_STATIC_VIEWS_DETAIL_WRAPPER_HPP
#define BOOST_STATIC_VIEWS_DETAIL_WRAPPER_HPP

#include "config.hpp"
#include "invoke.hpp"

#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

/// \brief Wraps an lvalue or an rvalue reference.

/// This is basically a `constexpr` version of
/// `std::reference_wrapper`. The difference is that #detail::wrapper
/// can wrap both lvalue and rvalue references. This is just syntactic
/// sugar.
///
/// \code{.cpp}
/// template <class T>
/// struct wrapper {
///     constexpr auto get() const& noexcept(whenever possible);
///     constexpr auto get() &      noexcept(whenever possible);
///     constexpr auto get() &&     noexcept(whenever possible);
///
///     template <class... Args>
///     constexpr auto operator() (Args&&...) const
///         noexcept(whenever possible);
/// };
/// \endcode
///
/// Constructors are intentionally not listed. Use make_wrapper(T&&)
/// to create wrappers.
template <class T> struct wrapper;

template <class T> struct is_wrapper : std::false_type {};

template <class T> struct is_wrapper<wrapper<T>> : std::true_type {};

/// \cond
template <class T> struct wrapper<T &> {
  using type = T;

  explicit BOOST_STATIC_VIEWS_CONSTEXPR wrapper(type &x) noexcept
      : _payload{&x} {}

  BOOST_STATIC_VIEWS_CONSTEXPR
  wrapper(wrapper const &other) noexcept = default;

  BOOST_STATIC_VIEWS_CONSTEXPR
  wrapper(wrapper &&) noexcept = default;

  BOOST_STATIC_VIEWS_CONSTEXPR
  wrapper &operator=(wrapper const &other) noexcept = default;

  BOOST_STATIC_VIEWS_CONSTEXPR
  wrapper &operator=(wrapper &&) noexcept = default;

  BOOST_STATIC_VIEWS_FORCEINLINE
  BOOST_STATIC_VIEWS_CONSTEXPR auto get() const noexcept -> type const & {
    return *_payload;
  }

  BOOST_STATIC_VIEWS_FORCEINLINE
  BOOST_STATIC_VIEWS_CONSTEXPR auto get() noexcept -> type & {
    return *_payload;
  }

  /*
  BOOST_STATIC_VIEWS_FORCEINLINE
  BOOST_STATIC_VIEWS_CONSTEXPR operator type&() const noexcept
  {
      return get();
  }
  */

  /*
  template <class... Args>
  BOOST_STATIC_VIEWS_FORCEINLINE
  BOOST_STATIC_VIEWS_CONSTEXPR
  decltype(auto) operator()(Args&&... args) const
      BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(invoke(
          std::declval<T const&>(), std::declval<Args&&>()...)))
  {
      return invoke(*_payload, std::forward<Args>(args)...);
  }
  */

private:
  T *_payload;
};
/// \endcond

/// \cond
template <class T> struct wrapper<T &&> {
  using type = T;

  static_assert(std::is_move_constructible<T>::value,
                "wrapper<T&&>: I'm refusing to work with non-movable types. "
                "If you have problems with that, submit an issue here "
                "https://github.com/BoostGSoC17/static-views/issues.");

  explicit BOOST_STATIC_VIEWS_CONSTEXPR wrapper(type &&x)
      BOOST_STATIC_VIEWS_NOEXCEPT_IF(
          std::is_nothrow_move_constructible<type>::value)
      : _payload{std::move(x)} {}

  // clang-format off
    BOOST_STATIC_VIEWS_CONSTEXPR wrapper(wrapper const&)            = default;
    BOOST_STATIC_VIEWS_CONSTEXPR wrapper(wrapper &&)                = default;
    BOOST_STATIC_VIEWS_CONSTEXPR wrapper& operator=(wrapper const&) = default;
    BOOST_STATIC_VIEWS_CONSTEXPR wrapper& operator=(wrapper &&)     = default;
// clang-format on

#if 0 // defined(BOOST_STATIC_VIEWS_GCC)
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto get() const & BOOST_STATIC_VIEWS_NOEXCEPT_IF(
        std::is_nothrow_copy_constructible<type>::value) -> type
    {
        return _payload;
    }
#else
  BOOST_STATIC_VIEWS_FORCEINLINE
  BOOST_STATIC_VIEWS_CONSTEXPR
  auto get() & noexcept -> type & { return _payload; }

  BOOST_STATIC_VIEWS_FORCEINLINE
  BOOST_STATIC_VIEWS_CONSTEXPR
  auto get() const & noexcept -> type const & { return _payload; }
#endif

  // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto get() &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<T>::value) -> type
    {
        return std::move(_payload);
    }
  // clang-format on

  /*
  BOOST_STATIC_VIEWS_CONSTEXPR operator type() const noexcept
  {
      return get();
  }

  BOOST_STATIC_VIEWS_CONSTEXPR operator type() noexcept
  {
      return get();
  }
  */

  // clang-format off
    /*
    template <class... Args>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) operator()(Args&&... args) &
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(invoke(std::declval<wrapper&>().get(),
                std::declval<Args&&>()...)))
    {
        return invoke(std::forward<wrapper>(*this).get(),
            std::forward<Args>(args)...);
    }
    */
  // clang-format on

  // clang-format off
    /*
    template <class... Args>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) operator()(Args&&... args) const&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(invoke(std::declval<wrapper const&>().get(),
                std::declval<Args&&>()...)))
    {
        return invoke(std::forward<wrapper>(*this).get(),
            std::forward<Args>(args)...);
    }
    */
  // clang-format on

  // clang-format off
    /*
    template <class... Args>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) operator()(Args&&... args) &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(invoke(std::declval<wrapper&&>().get(),
                std::declval<Args&&>()...)))
    {
        return invoke(std::forward<wrapper>(*this).get(),
            std::forward<Args>(args)...);
    }
    */
  // clang-format on

private:
  T _payload;
};
/// \endcond

struct make_wrapper_impl {

private:
  // If an rvalue of type T is passed to make_wrapper, require T to
  // be move-constructible.
  template <class T>
  using reference_or_move_constructible = std::integral_constant<
      bool, std::is_lvalue_reference<T>::value ||
                std::is_move_constructible<std::decay_t<T>>::value>;

  // clang-format off
    template <class T>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(T&& /*unused*/,
        std::false_type /*T is not move constructible*/) const noexcept
    {
        static_assert(std::is_rvalue_reference<T&&>::value
            && !std::is_move_constructible<T>::value,
            "Congratulations, you've found a bug in the StaticViews "
            "library. Please, be so kind to submit it here "
            "https://github.com/BoostGSoC17/static-views/issues.");
        static_assert(std::is_move_constructible<T>::value,
            "`boost::static_views::make_wrapper(T&&)` requires `T` "
            "to be move constructible.");
    }
  // clang-format on

  // clang-format off
    template <class T>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(T&& x,
        std::true_type /*all good*/) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(detail::wrapper<decltype(x)>{std::forward<T>(x)}))
    {
        return detail::wrapper<decltype(x)>{std::forward<T>(x)};
    }
  // clang-format on

public:
  // clang-format off
    template <class T>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(T&& x) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<make_wrapper_impl const&>().call_impl(
                std::forward<T>(x),
                reference_or_move_constructible<decltype(x)>{})
       ))
    {
        return call_impl(std::forward<T>(x),
            reference_or_move_constructible<decltype(x)>{});
    }
  // clang-format on
};

} // end namespace detail

/// \brief Makes a wrapper around an rvalue or lvalue reference.

/// Creates a #detail::wrapper of `T&` or `T&&` depending on the type
/// of `x`.
// clang-format off
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_wrapper_impl, make_wrapper)


BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DETAIL_WRAPPER_HPP
