//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_DETAIL_FIND_FIRST_HPP
#define BOOST_STATIC_VIEWS_DETAIL_FIND_FIRST_HPP

#include <functional>
#include <type_traits>

#include <boost/config.hpp>
#include <boost/static_views/detail/config.hpp>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

struct find_first_i_impl {
    template <class View, class Predicate>
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(
        View&& xs, Predicate&& p = Predicate{}) const
        // noexcept /* TODO add specifier */
        -> std::size_t
    {
        std::size_t const n = xs.size();
        std::size_t       i = 0;

        while (i < n && !p(xs[i])) {
            ++i;
        }
        return i;
    }
};

} // end namespace detail

#if defined(DOXYGEN_IN_HOUSE)
constexpr auto find_first_i = [](
    auto&& view, auto&& x, auto&& eq) -> std::size_t { ... };
#else
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::find_first_i_impl, find_first_i)
#endif

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DETAIL_FIND_FIRST_HPP
