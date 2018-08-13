//          Copyright Tom Westerhout 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "testing.hpp"
#include <utility>
#include <boost/static_views/raw_view.hpp>
#include <boost/static_views/transform.hpp>

auto test_make()
{
    static constexpr int  xs_data[20] = {};

    struct foo {
        foo()           = default;
        foo(foo const&) = delete;
        foo(foo&&)      = default;
        foo& operator=(foo&&) = default;
        foo& operator=(foo const&) = delete;

        constexpr auto operator()(int const x) const noexcept
        {
            return 1.0f / static_cast<float>(x);
        }
    };

    // construction from an lvalue reference
    {
        static constexpr auto xs = boost::static_views::raw_view(xs_data);
        // Normal function call
        constexpr auto ys = boost::static_views::transform(xs, foo{});
        using ys_type     = std::remove_cv_t<decltype(ys)>;
        STATIC_ASSERT(boost::static_views::View<ys_type>,
            "transform view constructed from an lvalue reference does not "
            "model the View concept.");
        STATIC_ASSERT((boost::static_views::Same<ys_type::reference, float>),
            "But foo::operator() returns a float!");
        STATIC_ASSERT((boost::static_views::Same<ys_type::value_type, float>),
            "But foo::operator() returns a float!");
        // Curried function call
        constexpr auto zs = boost::static_views::transform(foo{})(xs);
        using zs_type     = std::remove_cv_t<decltype(zs)>;
        STATIC_ASSERT(boost::static_views::View<zs_type>,
            "transform view constructed from an lvalue reference does not "
            "model the View concept.");
    }

    // construction from an rvalue reference
    {
        static constexpr auto foo_c = foo{};
        // Normal function call
        constexpr auto ys = boost::static_views::transform(
            boost::static_views::raw_view(xs_data), foo_c);
        using ys_type = std::remove_cv_t<decltype(ys)>;
        STATIC_ASSERT(boost::static_views::View<ys_type>,
            "transform view constructed from an rvalue reference does not "
            "model the View concept.");
        // Curried function call
        // This one is tricky, because foo_c is a reference and foo's copy
        // constructor is deleted, so the reference has to be propagated all the
        // way.
        constexpr auto zs = boost::static_views::transform(foo_c)(
            boost::static_views::raw_view(xs_data));
        using zs_type     = std::remove_cv_t<decltype(zs)>;
        STATIC_ASSERT(boost::static_views::View<zs_type>,
            "transform view constructed from an rvalue reference does not "
            "model the View concept.");
    }

    // copy and move
    {
        static constexpr auto foo_c = foo{};
        static constexpr auto xs    = boost::static_views::raw_view(xs_data);
        constexpr auto        ys    = boost::static_views::transform(xs, foo{});
        constexpr auto        zs    = boost::static_views::transform(xs, foo_c);
        using ys_type               = std::remove_cv_t<decltype(ys)>;
        using zs_type               = std::remove_cv_t<decltype(zs)>;
        STATIC_ASSERT(!std::is_copy_constructible<ys_type>::value,
            "ys should not be copy-constructible, because foo is not.");
        STATIC_ASSERT(std::is_nothrow_copy_constructible<zs_type>::value,
            "zs is not nothrow copyable.");
        STATIC_ASSERT(std::is_nothrow_move_constructible<ys_type>::value,
            "ys is not nothrow movable.");
        STATIC_ASSERT(std::is_nothrow_move_constructible<zs_type>::value,
            "zs is not nothrow movable.");
        STATIC_ASSERT(!std::is_copy_assignable<ys_type>::value,
            "ys should not be copy-assignable, because foo is not.");
        STATIC_ASSERT(std::is_nothrow_copy_assignable<zs_type>::value,
            "zs is not nothrow copyable.");
        STATIC_ASSERT(std::is_nothrow_move_assignable<ys_type>::value,
            "drop view is not nothrow movable.");
        STATIC_ASSERT(std::is_nothrow_move_assignable<zs_type>::value,
            "drop view is not nothrow movable.");
    }
}

int main()
{
    test_make();
    return boost::report_errors();
}

