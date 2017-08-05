//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/detail/utils.hpp>
#include <boost/static_views/detail/wrapper.hpp>

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

int main()
{
    constexpr auto x1 =
        boost::static_views::make_wrapper(only_move{private_tag{}});
    using x1_t = std::decay_t<decltype(x1)>;
    static_assert(noexcept(boost::static_views::make_wrapper(
                      only_move{private_tag{}})),
        "");
    static_assert(std::is_nothrow_move_constructible<x1_t>(), "");
    static_assert(!std::is_copy_constructible<x1_t>(), "");
    static_assert(!std::is_move_assignable<x1_t>(), "");
    static_assert(!std::is_copy_assignable<x1_t>(), "");
    static_assert(
        std::is_same<decltype(x1.get()), only_move const&>::value,
        "");

    static constexpr nothing y1{private_tag{}};
    constexpr auto x2 = boost::static_views::make_wrapper(y1);
    using x2_t        = std::decay_t<decltype(x2)>;
    static_assert(!std::is_const<x2_t>::value, "");
    static_assert(std::is_nothrow_move_constructible<x2_t>(), "");
    static_assert(std::is_nothrow_copy_constructible<x2_t>(), "");
    static_assert(std::is_nothrow_move_assignable<x2_t>(), "");
    static_assert(std::is_nothrow_copy_assignable<x2_t>(), "");
    static_assert(
        std::is_same<decltype(x1.get()), only_move const&>::value,
        "");
    static_assert(std::is_same<decltype(std::declval<x2_t>().get()),
                      nothing const&>::value,
        "");

    constexpr auto x3 =
        boost::static_views::make_wrapper(only_move{private_tag{}})
            .get();
    using x3_t = decltype(
        boost::static_views::make_wrapper(only_move{private_tag{}})
            .get());
    static_assert(std::is_same<x3_t, only_move>::value, "");
}
