//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_DETAIL_FIND_FIRST_HPP
#define BOOST_STATIC_VIEWS_DETAIL_FIND_FIRST_HPP

#include <functional>
#include <type_traits>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/detail/utils.hpp>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

struct find_first_i_impl {
    // clang-format off
    template <class View, class Predicate>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs, Predicate&& p = Predicate{}) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            noexcept(xs.size()),
            noexcept(p(xs[std::declval<std::size_t>()]))
        ))
        -> std::size_t
    {
        std::size_t const n = xs.size();
        std::size_t       i = 0;

        while (i < n && !p(xs[i])) {
            ++i;
        }
        return i;
    }
    // clang-format on
};

} // namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(
    detail::find_first_i_impl, find_first_i)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DETAIL_FIND_FIRST_HPP
