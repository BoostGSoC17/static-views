//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "testing.hpp"
#include <boost/static_views/raw_view.hpp>
#include <boost/static_views/view_concept.hpp>

auto test_construction()
{
    int as[]                  = {1, 2, 3, 4, 5};
    int const volatile bs[]   = {1, 2, 3};
    static constexpr int cs[] = {1, 2, 3, 4};
    constexpr int        ds[] = {1, 2, 3, 4};

    auto as_view    = boost::static_views::raw_view(as);
    using as_view_t = decltype(as_view);
    BOOST_TEST_EQ(boost::static_views::View<as_view_t>, true);
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_copy_constructible<as_view_t>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_constructible<as_view_t>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_copy_assignable<as_view_t>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_assignable<as_view_t>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_destructible<as_view_t>));
    BOOST_TEST_TRAIT_TRUE((std::is_trivial<as_view_t>));
    BOOST_TEST_EQ(as_view_t::extent(), 5);
    BOOST_TEST_EQ(as_view.size(), 5);
    BOOST_TEST_EQ(as_view[0], 1);
    BOOST_TEST_EQ(as_view[4], 5);
    BOOST_TEST_THROWS(as_view[5], boost::static_views::out_of_bound);
    BOOST_TEST_THROWS(as_view[-1], boost::static_views::out_of_bound);
    BOOST_TEST_EQ(as_view.unsafe_at(2), 3);
    as_view[3] = -8;
    BOOST_TEST_EQ(as_view.unsafe_at(3), -8);

    auto bs_view    = boost::static_views::raw_view(bs);
    using bs_view_t = decltype(bs_view);
    BOOST_TEST_EQ(boost::static_views::View<bs_view_t>, true);
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_copy_constructible<bs_view_t>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_constructible<bs_view_t>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_copy_assignable<bs_view_t>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_assignable<bs_view_t>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_destructible<bs_view_t>));
    BOOST_TEST_TRAIT_TRUE((std::is_trivial<bs_view_t>));
    BOOST_TEST_EQ(bs_view_t::extent(), 3);
    BOOST_TEST_EQ(bs_view.size(), 3);
    BOOST_TEST_EQ(bs_view[0], 1);
    BOOST_TEST_EQ(bs_view[2], 3);
    BOOST_TEST_THROWS(BOOST_STATIC_VIEWS_UNUSED auto b1 = bs_view[3],
        boost::static_views::out_of_bound);
    BOOST_TEST_THROWS(BOOST_STATIC_VIEWS_UNUSED auto b2 = bs_view[-1],
        boost::static_views::out_of_bound);
    BOOST_TEST_EQ(bs_view.unsafe_at(1), 2);
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<bs_view_t::reference, int const volatile&>));

    constexpr auto cs_view = boost::static_views::raw_view(cs);
    using cs_view_t        = std::remove_const_t<decltype(cs_view)>;
    BOOST_TEST_EQ(boost::static_views::View<cs_view_t>, true);
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_copy_constructible<cs_view_t>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_constructible<cs_view_t>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_copy_assignable<cs_view_t>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_assignable<cs_view_t>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_destructible<cs_view_t>));
    BOOST_TEST_TRAIT_TRUE((std::is_trivial<cs_view_t>));
    BOOST_TEST_EQ(cs_view_t::extent(), 4);
    STATIC_ASSERT(cs_view.size() == 4, "size() is broken.");
    BOOST_TEST_EQ(cs_view[0], 1);
    STATIC_ASSERT(cs_view[2] == 3, "operator[] is broken.");
    STATIC_ASSERT(cs_view.unsafe_at(1) == 2, "unsafe_at() is broken.");
    BOOST_TEST_TRAIT_TRUE((std::is_same<cs_view_t::reference, int const&>));

    // No constexpr here, because ds has no "address" during
    // compilation.
    BOOST_STATIC_VIEWS_UNUSED auto const ds_view =
        boost::static_views::raw_view(ds);
}

int main(void)
{
    test_construction();
    return boost::report_errors();
}
