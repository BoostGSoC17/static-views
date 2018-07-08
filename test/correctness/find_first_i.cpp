//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "testing.hpp"
#include <utility>
#include <boost/static_views/detail/find_first.hpp>
#include <boost/static_views/raw_view.hpp>

auto test_1()
{
    static constexpr int data[] = {1, 7, 4, 8, 11};
    static constexpr auto  xs = boost::static_views::raw_view(data);

    struct is_8_impl {
        constexpr auto operator() (int const x) const noexcept
        {
            return x == 8;
        }
    };
    constexpr is_8_impl is_8;

    // Related to a bug in detail/invoke.hpp
    STATIC_ASSERT(boost::static_views::invoke(is_8_impl{}, 5) == false, "");
    STATIC_ASSERT(
        (boost::static_views::is_invocable<is_8_impl, int>::value), "");
    STATIC_ASSERT((boost::static_views::find_first_i(xs, is_8) == 3),
        "find_first_i is broken!");
}

int main()
{
    test_1();
    return boost::report_errors();
}
