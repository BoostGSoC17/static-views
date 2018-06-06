//          Copyright Tom Westerhout 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "testing.hpp"
#include <utility>
#include <boost/static_views/raw_view.hpp>
#include <boost/static_views/slice.hpp>

auto test_make()
{
    int xs_data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    //                          ^^^^^^^
    auto xs = boost::static_views::raw_view(xs_data);
    auto ys = boost::static_views::slice(xs, 3, 6);
    using ys_type = decltype(ys);
    BOOST_TEST_EQ(ys.size(), 3u);
    STATIC_ASSERT(boost::static_views::View<ys_type>,
        "ys does not model the View concept.");
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_copy_constructible<ys_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_constructible<ys_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_copy_assignable<ys_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_assignable<ys_type>));
    BOOST_TEST_EQ(ys[0], 4);
    BOOST_TEST_EQ(ys[2], 6);
    BOOST_TEST_THROWS(ys[3], boost::static_views::out_of_bound);
}

int main()
{
    test_make();
    return boost::report_errors();
}
