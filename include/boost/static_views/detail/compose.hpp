//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_COMPOSE_HPP
#define BOOST_STATIC_VIEWS_COMPOSE_HPP

#include "../concepts.hpp"
#include "invoke.hpp"
#include "wrapper.hpp"

#include <algorithm>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
template <class Second, class First>
struct compose_impl
    : private Second
    , private First {

    using first_fn = typename First::value_type;
    using second_fn = typename Second::value_type;

    BOOST_STATIC_VIEWS_CONSTEXPR
    compose_impl(Second&& second, First&& first) BOOST_STATIC_VIEWS_NOEXCEPT_IF(
        std::is_nothrow_move_constructible<First>::value&&
            std::is_nothrow_move_constructible<Second>::value)
        : Second{std::move(second)}, First{std::move(first)}
    {
    }

    compose_impl(compose_impl const&) = default;
    compose_impl(compose_impl&&) = default;
    compose_impl& operator=(compose_impl const&) = default;
    compose_impl& operator=(compose_impl&&) = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto const& first() const& noexcept
    {
        return static_cast<First const&>(*this).get();
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto const& second() const& noexcept
    {
        return static_cast<Second const&>(*this).get();
    }

    // clang-format off
    template <class... Args
        BOOST_STATIC_VIEWS_REQUIRES(
            is_invocable<first_fn const&, Args...>::value
         && is_invocable<second_fn const&,
                invoke_t<first_fn const&, Args...>>::value)
    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) operator()(Args&&... args) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(invoke(std::declval<second_fn const&>(),
                invoke(std::declval<first_fn const&>(),
                    std::forward<Args>(args)...))))
    {
        return invoke(second(), invoke(first(), std::forward<Args>(args)...));
    }
};

struct make_compose {
  private:
    template <class Second, class First>
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(
        Second second, First first) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            compose_impl<Second, First>{std::move(second), std::move(first)});

  public:
    // clang-format off
    template <class Second, class First
        BOOST_STATIC_VIEWS_REQUIRES(
            !Same<std::remove_cv_t<std::remove_reference_t<Second>>,
                  std::remove_cv_t<std::remove_reference_t<First>>>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Second&& second, First&& first) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<make_compose const&>().call_impl(
                make_wrapper(std::forward<Second>(second)),
                make_wrapper(std::forward<First>(first)))))
    {
        return call_impl(make_wrapper(std::forward<Second>(second)),
            make_wrapper(std::forward<First>(first)));
    }
};

} // end namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_compose, compose);

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DROP_HPP
