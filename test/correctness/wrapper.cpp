//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/detail/utils.hpp>
#include <boost/static_views/detail/wrapper.hpp>
#include "testing.hpp"

struct private_tag {
};

struct nothing {
    constexpr nothing(private_tag) noexcept {}

    constexpr nothing(nothing const&) = delete;
    constexpr nothing(nothing&&)      = delete;
    constexpr nothing& operator=(nothing const&) = delete;
    constexpr nothing& operator=(nothing&&) = delete;
};

struct only_move {
    constexpr only_move(private_tag) noexcept {}

    constexpr only_move(only_move const&)     = delete;
    constexpr only_move(only_move&&) noexcept = default;
    constexpr only_move& operator=(only_move const&) = delete;
    constexpr only_move& operator=(only_move&&) = delete;
};

auto construction_lvalue()
{
    static constexpr int payload = 123;
    CONSTEXPR auto x = boost::static_views::make_wrapper(payload);
    // STATIC_ASSERT(noexcept(boost::static_views::make_wrapper(payload)),
    //     "make_wrapper may throw.");
    using x_type =
        std::remove_const_t<std::remove_reference_t<decltype(x)>>;
    BOOST_TEST_TRAIT_TRUE((std::is_same<decltype(x.get()), int const&>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<x_type::value_type, int const>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<x_type::reference, int const&>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<x_type::const_reference, int const&>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_copy_constructible<x_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_copy_assignable<x_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_constructible<x_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_assignable<x_type>));
    BOOST_TEST_EQ(sizeof(x), sizeof(void*));
    BOOST_STATIC_VIEWS_UNUSED auto& x2int1 = static_cast<int const&>(x);
    STATIC_ASSERT(x.get() == payload,
        "Construction does not preserve the content.");

    nothing hehe{private_tag{}};
    auto y = boost::static_views::make_wrapper(hehe);
    using y_type =
        std::remove_const_t<std::remove_reference_t<decltype(y)>>;
    BOOST_TEST_TRAIT_TRUE((std::is_same<decltype(y.get()), nothing&>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<y_type::value_type, nothing>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<y_type::reference, nothing&>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<y_type::const_reference, nothing const&>));
    BOOST_TEST_TRAIT_TRUE((std::is_copy_constructible<y_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_copy_assignable<y_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_constructible<y_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_assignable<y_type>));
    BOOST_STATIC_VIEWS_UNUSED auto& y2nothing1 = static_cast<nothing&>(y);
    BOOST_STATIC_VIEWS_UNUSED auto& y2nothing2 = static_cast<nothing const&>(y);
}

auto construction_rvalue_empty()
{
    struct foo {};

    CONSTEXPR auto x = boost::static_views::make_wrapper(foo{});
    using x_type =
        std::remove_const_t<std::remove_reference_t<decltype(x)>>;
    STATIC_ASSERT(boost::static_views::MoveConstructible<foo>,
        "foo does not model MoveConstructible :)");
    BOOST_TEST_EQ(sizeof(x_type), sizeof(foo));
    BOOST_TEST_TRAIT_TRUE((std::is_empty<x_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<decltype(x.get()), foo const&>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<x_type::value_type, foo>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<x_type::reference, foo&>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<x_type::const_reference, foo const&>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_copy_constructible<x_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_copy_assignable<x_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_constructible<x_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_assignable<x_type>));
    BOOST_STATIC_VIEWS_UNUSED auto const& x2foo1 = static_cast<foo const&>(x);

    struct bar {
        // bar() = default;
        bar(bar const&) = delete;
        bar(bar&&) = default;
        bar& operator=(bar const&) = default;
        bar& operator=(bar&&) = delete;
    };

    auto y = boost::static_views::make_wrapper(bar{});
    using y_type =
        std::remove_const_t<std::remove_reference_t<decltype(y)>>;
    BOOST_TEST_EQ(sizeof(y_type), sizeof(bar));
    BOOST_TEST_TRAIT_TRUE((std::is_empty<y_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<decltype(y.get()), bar&>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<y_type::value_type, bar>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<y_type::reference, bar&>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<y_type::const_reference, bar const&>));
    BOOST_TEST_TRAIT_FALSE((std::is_copy_constructible<y_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_copy_assignable<y_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_constructible<y_type>));
    BOOST_STATIC_VIEWS_UNUSED auto const& y2bar1 = static_cast<bar const&>(y);
    BOOST_STATIC_VIEWS_UNUSED auto const& y2bar2 = static_cast<bar&>(y);
    BOOST_STATIC_VIEWS_UNUSED auto const y2bar3 = static_cast<bar>(std::move(y));
}

auto construction_rvalue()
{
    static CONSTEXPR auto x = boost::static_views::make_wrapper(123);
    using x_type =
        std::remove_const_t<std::remove_reference_t<decltype(x)>>;
    STATIC_ASSERT(boost::static_views::MoveConstructible<int>,
        "int does not model MoveConstructible :)");
    BOOST_TEST_TRAIT_TRUE((std::is_same<decltype(x.get()), int const&>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<x_type::value_type, int>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<x_type::reference, int&>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<x_type::const_reference, int const&>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_copy_constructible<x_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_copy_assignable<x_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_constructible<x_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_assignable<x_type>));
    BOOST_STATIC_VIEWS_UNUSED constexpr auto& x2int1 = static_cast<int const&>(x);

    struct bar final : only_move {
        constexpr bar() noexcept : only_move{{}} {}
    };

    auto y = boost::static_views::make_wrapper(bar{});
    using y_type =
        std::remove_const_t<std::remove_reference_t<decltype(y)>>;
    BOOST_TEST_TRAIT_TRUE((std::is_same<decltype(y.get()), bar&>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<y_type::value_type, bar>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<y_type::reference, bar&>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<y_type::const_reference, bar const&>));
    BOOST_TEST_TRAIT_FALSE((std::is_copy_constructible<y_type>));
    BOOST_TEST_TRAIT_FALSE((std::is_copy_assignable<y_type>));
    BOOST_TEST_TRAIT_TRUE((std::is_nothrow_move_constructible<y_type>));
    BOOST_STATIC_VIEWS_UNUSED auto const& y2bar1 = static_cast<bar const&>(y);
    BOOST_STATIC_VIEWS_UNUSED auto const& y2bar2 = static_cast<bar&>(y);
    auto y2bar3 = static_cast<bar&&>(std::move(y));
    BOOST_TEST_TRAIT_TRUE((std::is_same<bar, decltype(y2bar3)>));
    BOOST_STATIC_VIEWS_UNUSED auto y2bar4 = std::move(y).get();
}

auto using_raw_wrapper()
{
    using x_type = boost::static_views::wrapper<nothing>;
    x_type x{private_tag{}};
    BOOST_TEST_TRAIT_TRUE((std::is_same<decltype(x.get()), nothing&>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<x_type::value_type, nothing>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<x_type::reference, nothing&>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<x_type::const_reference, nothing const&>));
    BOOST_TEST_TRAIT_FALSE((std::is_copy_constructible<x_type>));
    BOOST_TEST_TRAIT_FALSE((std::is_copy_assignable<x_type>));
    BOOST_TEST_TRAIT_FALSE((std::is_move_constructible<x_type>));
    BOOST_TEST_TRAIT_FALSE((std::is_move_assignable<x_type>));
    BOOST_STATIC_VIEWS_UNUSED auto const& x2nothing1 = static_cast<nothing const&>(x);
    BOOST_STATIC_VIEWS_UNUSED auto const& y2nothing2 = static_cast<nothing&>(x);
    BOOST_STATIC_VIEWS_UNUSED auto const& y2nothing3 = x.get();
}

int main()
{
    construction_lvalue();
    construction_rvalue();
    construction_rvalue_empty();
    using_raw_wrapper();
    return boost::report_errors();
}
