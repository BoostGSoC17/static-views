//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include <iostream>
#include <utility>
#include <boost/config.hpp>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/raw_view.hpp>
#include <boost/static_views/slice.hpp>

namespace {

template <class Slice, class T>
// BOOST_FORCEINLINE
BOOST_STATIC_VIEWS_CONSTEXPR auto bsearch_impl(Slice&& ys, T const& x)
    -> std::size_t
{
    auto const b = ys.map(0);
    auto const e = ys.map(ys.size() - 1);
    auto const i = ys.size() / 2;

    if (x < ys[i])
        return bsearch_impl(boost::static_views::slice(b, ys.map(i))(
                                std::forward<Slice>(ys).parent()),
            x);
    else if (x > ys[i])
        return bsearch_impl(boost::static_views::slice(ys.map(i) + 1, e)(
                                std::forward<Slice>(ys).parent()),
            x);
    return ys.map(i);
}

template <class View, class T>
BOOST_FORCEINLINE BOOST_STATIC_VIEWS_CONSTEXPR auto bsearch(
    View&& ys, T const& x) -> std::size_t
{
    return bsearch_impl(
        boost::static_views::slice(0ul, ys.size())(std::forward<View>(ys)), x);
}

} // unnamed namespace

BOOST_NOINLINE
void test1()
{
    static constexpr int xs[] = {1, 3, 5, 6, 7, 9, 10};
    if (bsearch(boost::static_views::raw_view(xs), 7) != 4) {
        std::exit(1);
    }
}

int main(void)
{
    test1();

    return 0;
}
