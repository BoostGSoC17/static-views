//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_LFOLD_HPP
#define BOOST_STATIC_VIEWS_LFOLD_HPP

#include <functional>
#include <type_traits>
#include <boost/static_views/detail/config.hpp>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

struct left_fold_impl {
    template <class View, class T,
        class BinaryOperation = std::plus<T>>
    constexpr auto operator()(View&& xs, T init,
        BinaryOperation&& op = BinaryOperation{}) const
        noexcept /* TODO add specifier */
        -> T
    {
        std::size_t const n = xs.size();
        std::size_t       i = 0;

        while (i < n) {
            init = op(init, std::forward<View>(xs)[i]);
            ++i;
        }
        return init;
    }
};

} // end namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::left_fold_impl, lfold)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_LFOLD_HPP
