//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_DETAIL_UTILS_HPP
#define BOOST_STATIC_VIEWS_DETAIL_UTILS_HPP

#include "config.hpp"

#include <tuple>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
namespace utils {

constexpr auto all() noexcept -> bool { return true; }

template <class... Bools>
constexpr auto all(bool const x, Bools... xs) noexcept -> bool {
  return x && all(xs...);
}

} // namespace utils

#if defined(__cpp_lib_void_t) && __cpp_lib_void_t >= 201411
using std::void_t;
#else
template <class...> using void_t = void;
#endif

struct nonesuch {
  nonesuch() = delete;
  ~nonesuch() = delete;
  nonesuch(nonesuch const &) = delete;
  nonesuch(nonesuch &&) = delete;
  nonesuch &operator=(nonesuch const &) = delete;
  nonesuch &operator=(nonesuch &&) = delete;
};

struct not_copy_constructible {
  // clang-format off
    not_copy_constructible(not_copy_constructible const&) = delete;

    constexpr not_copy_constructible()
        noexcept = default;
    constexpr not_copy_constructible(not_copy_constructible&&)
        noexcept = default;
    constexpr not_copy_constructible& operator=(not_copy_constructible const&)
        noexcept = default;
    constexpr not_copy_constructible& operator=(not_copy_constructible&&)
        noexcept = default;
  // clang-format on
};

struct not_move_constructible {
  // clang-format off
    not_move_constructible(not_move_constructible&&) = delete;

    constexpr not_move_constructible()
        noexcept = default;
    constexpr not_move_constructible(not_move_constructible const&)
        noexcept = default;
    constexpr not_move_constructible& operator=(not_move_constructible const&)
        noexcept = default;
    constexpr not_move_constructible& operator=(not_move_constructible&&)
        noexcept = default;
  // clang-format on
};

struct not_copy_assignable {
  // clang-format off
    not_copy_assignable& operator=(not_copy_assignable const&) = delete;

    constexpr not_copy_assignable()
        noexcept = default;
    constexpr not_copy_assignable(not_copy_assignable const&)
        noexcept = default;
    constexpr not_copy_assignable(not_copy_assignable&&)
        noexcept = default;
    constexpr not_copy_assignable& operator=(not_copy_assignable&&)
        noexcept = default;
  // clang-format on
};

struct not_move_assignable {
  // clang-format off
    not_move_assignable& operator=(not_move_assignable&&) = delete;

    constexpr not_move_assignable()
        noexcept = default;
    constexpr not_move_assignable(not_move_assignable const&)
        noexcept = default;
    constexpr not_move_assignable(not_move_assignable&&)
        noexcept = default;
    constexpr not_move_assignable& operator=(not_move_assignable const&)
        noexcept = default;
  // clang-format on
};

template <class T, class List> struct push_front;

template <template <class...> class List, class T, class... Args>
struct push_front<T, List<Args...>> {
  using type = List<T, Args...>;
};

template <class T, class List>
using push_front_t = typename push_front<T, List>::type;

template <class List> struct filter_out_void;

template <template <class...> class List> struct filter_out_void<List<>> {
  using type = List<>;
};

template <template <class...> class List, class Arg, class... Args>
struct filter_out_void<List<Arg, Args...>> {
  using type = std::conditional_t<
      std::is_same<Arg, void>::value,
      typename filter_out_void<List<Args...>>::type,
      push_front_t<Arg, typename filter_out_void<List<Args...>>::type>>;
};

template <class List>
using filter_out_void_t = typename filter_out_void<List>::type;

template <class T> struct conditional_copy_and_move_impl;

template <template <class...> class List, class... Ts>
struct conditional_copy_and_move_impl<List<Ts...>> : Ts... {
  constexpr conditional_copy_and_move_impl() = default;
  constexpr conditional_copy_and_move_impl(
      conditional_copy_and_move_impl const &) = default;
  constexpr conditional_copy_and_move_impl(conditional_copy_and_move_impl &&) =
      default;
  constexpr conditional_copy_and_move_impl &
  operator=(conditional_copy_and_move_impl const &) = default;
  constexpr conditional_copy_and_move_impl &
  operator=(conditional_copy_and_move_impl &&) = default;
};

template <class T>
using conditional_copy_and_move =
    conditional_copy_and_move_impl<filter_out_void_t<
        std::tuple<std::conditional_t<!std::is_copy_constructible<T>::value,
                                      not_copy_constructible, void>,
                   std::conditional_t<!std::is_move_constructible<T>::value,
                                      not_move_constructible, void>,
                   std::conditional_t<!std::is_copy_assignable<T>::value,
                                      not_copy_assignable, void>,
                   std::conditional_t<!std::is_move_assignable<T>::value,
                                      not_move_assignable, void>>>>;

// clang-format off
template < class Default
         , class
         , template <class...> class Op
         , class... Args
         >
struct detector {
    using value_t = std::false_type;
    using type    = Default;
};

template < class Default
         , template <class...> class Op
         , class... Args
         >
struct detector<Default, void_t<Op<Args...>>, Op, Args...> {
    using value_t = std::true_type;
    using type    = Op<Args...>;
};
// clang-format on

template <template <class...> class Op, class... Args>
using is_detected = typename detector<nonesuch, void, Op, Args...>::value_t;

template <template <class...> class Op, class... Args>
using detected_t = typename detector<nonesuch, void, Op, Args...>::type;

namespace concepts {

constexpr auto check() noexcept -> bool { return true; }

template <class Check, class... Checks>
constexpr auto check(Check &&head, Checks &&... tail) noexcept -> bool {
  bool const passed = std::forward<Check>(head)();
  return passed ? check(std::forward<Checks>(tail)...) : false;
}

} // namespace concepts

} // namespace detail

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DETAIL_UTILS_HPP
