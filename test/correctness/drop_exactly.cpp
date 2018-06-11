//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "testing.hpp"
#include <utility>
#include <boost/static_views/drop.hpp>
#include <boost/static_views/raw_view.hpp>

auto test_make()
{
    int  xs_data[20] = {};
    auto xs          = boost::static_views::raw_view(xs_data);
    auto ys          = boost::static_views::drop_exactly(xs, 3);
    BOOST_TEST_EQ(ys.size(), 17u);
    STATIC_ASSERT(boost::static_views::View<decltype(ys)>,
        "ys does not model the View concept.");
    BOOST_TEST_THROWS(ys[17], boost::static_views::out_of_bound);
    auto zs = boost::static_views::drop_exactly(ys, 14);
    BOOST_TEST_EQ(zs.size(), 3u);
    auto bs = boost::static_views::drop_exactly(
        xs, std::integral_constant<std::size_t, 2>{});
    STATIC_ASSERT(sizeof(bs) == sizeof(xs), "EBO in drop doesn't work.");
    STATIC_ASSERT(decltype(bs)::extent() == 18u, "extent() is broken.");
}

int main()
{
    test_make();
    return boost::report_errors();
}
