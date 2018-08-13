//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_DETAIL_UTILS_HPP
#define BOOST_STATIC_VIEWS_DETAIL_UTILS_HPP

#include "config.hpp"

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

constexpr auto all() noexcept -> bool { return true; }

template <class... Bools>
constexpr auto all(bool const x, Bools... xs) noexcept -> bool
{
    return x && all(xs...);
}

constexpr auto any() noexcept -> bool { return false; }

template <class... Bools>
constexpr auto any(bool const x, Bools... xs) noexcept -> bool
{
    return x || any(xs...);
}

} // namespace detail

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DETAIL_UTILS_HPP
