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
    static constexpr int  xs_data[20] = {};

    // construction from an lvalue reference
    {
        static constexpr auto xs = boost::static_views::raw_view(xs_data);
        // Normal function call
        constexpr auto ys = boost::static_views::drop_exactly(xs, 3);
        using ys_type     = std::remove_cv_t<decltype(ys)>;
        STATIC_ASSERT(boost::static_views::View<ys_type>,
            "drop view constructed from an lvalue reference does not model the "
            "View concept.");
        // Curried function call
        constexpr auto zs = boost::static_views::drop_exactly(3)(xs);
        using zs_type     = std::remove_cv_t<decltype(zs)>;
        STATIC_ASSERT(boost::static_views::View<zs_type>,
            "drop view constructed from an lvalue reference does not model the "
            "View concept.");
    }

    // construction from an rvalue reference
    {
        // Normal function call
        constexpr auto ys = boost::static_views::drop_exactly(
            boost::static_views::raw_view(xs_data), 3);
        using ys_type = std::remove_cv_t<decltype(ys)>;
        STATIC_ASSERT(boost::static_views::View<ys_type>,
            "drop view constructed from an rvalue reference does not model the "
            "View concept.");
        // Curried function call
        constexpr auto zs = boost::static_views::drop_exactly(3)(
            boost::static_views::raw_view(xs_data));
        using zs_type     = std::remove_cv_t<decltype(zs)>;
        STATIC_ASSERT(boost::static_views::View<zs_type>,
            "drop view constructed from an rvalue reference does not model the "
            "View concept.");
    }

    // construction with an integral_constant index
    {
        // Normal function call
        constexpr auto ys = boost::static_views::drop_exactly(
            boost::static_views::raw_view(xs_data),
            std::integral_constant<unsigned, 8>{});
        using ys_type = std::remove_cv_t<decltype(ys)>;
        STATIC_ASSERT(
            sizeof(ys) == sizeof(boost::static_views::raw_view(xs_data)),
            "drop view is wasting memory.");
        STATIC_ASSERT(boost::static_views::View<ys_type>,
            "drop view constructed from an rvalue reference with an "
            "integral_constant index does not model the View concept.");
        // Curried function call
        constexpr auto zs =
            boost::static_views::drop_exactly(std::integral_constant<int, 1>{})(
                boost::static_views::raw_view(xs_data));
        using zs_type = std::remove_cv_t<decltype(zs)>;
        STATIC_ASSERT(
            sizeof(zs) == sizeof(boost::static_views::raw_view(xs_data)),
            "drop view is wasting memory.");
        STATIC_ASSERT(boost::static_views::View<zs_type>,
            "drop view constructed from an rvalue reference with an "
            "integral_constant index does not model the View concept.");
    }

    // copy and move
    {
        static constexpr auto xs = boost::static_views::raw_view(xs_data);
        constexpr auto        ys = boost::static_views::drop_exactly(
            xs, std::integral_constant<unsigned, 8>{});
        using ys_type = std::remove_cv_t<decltype(ys)>;
        STATIC_ASSERT(std::is_nothrow_copy_constructible<ys_type>::value,
            "drop view is not nothrow copyable.");
        STATIC_ASSERT(std::is_nothrow_move_constructible<ys_type>::value,
            "drop view is not nothrow movable.");
        STATIC_ASSERT(std::is_nothrow_copy_assignable<ys_type>::value,
            "drop view is not nothrow copyable.");
        STATIC_ASSERT(std::is_nothrow_move_assignable<ys_type>::value,
            "drop view is not nothrow movable.");
    }

    // how about throwing exceptions on invalid input?
    {
        static constexpr auto xs = boost::static_views::raw_view(xs_data);
#if defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)
        BOOST_TEST_THROWS(boost::static_views::drop_exactly(xs, 21),
            boost::static_views::assert_failure);
        BOOST_TEST_THROWS(boost::static_views::drop_exactly(-123)(xs),
            boost::static_views::assert_failure);
#endif
        // This would fail at compile-time though
        // auto ys = boost::static_views::drop_exactly(
        //     std::integral_constant<int, 21>{})(xs);
    }
}

int main()
{
    test_make();
    return boost::report_errors();
}
