//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_DETAIL_FIND_FIRST_HPP
#define BOOST_STATIC_VIEWS_DETAIL_FIND_FIRST_HPP

#include "../view_concept.hpp"

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
struct find_first_i_impl {
    // clang-format off
    template <class V, class Predicate
        BOOST_STATIC_VIEWS_REQUIRES(View<V>
            && is_invocable_r<bool, Predicate, typename V::reference>::value)
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(V const& xs, Predicate&& p) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            invoke(p, xs.unsafe_at(std::declval<typename V::index_type>()))))
    {
        using index_type   = typename V::index_type;
        index_type const n = static_cast<index_type>(xs.size());
        index_type       i = 0;
        while (i < n && !invoke(p, xs.unsafe_at(i))) {
            ++i;
        }
        return i;
    }
};
} // namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::find_first_i_impl, find_first_i)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DETAIL_FIND_FIRST_HPP
