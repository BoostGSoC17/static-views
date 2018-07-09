//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <boost/static_views/drop.hpp> // to get boost::static_views::drop_exactly
#include <boost/static_views/raw_view.hpp> // to get boost::static_views::raw_view
#include <boost/static_views/pipe.hpp> // for | syntax

namespace sv = boost::static_views;

int main()
{
    // First of all, we need some view of the data
    static constexpr int  data[] = {1, 2, 3, 4, 5};
    static constexpr auto raw = sv::raw_view(data);

    // Now, let's create a view of raw with the first 3 elements
    // dropped
    constexpr auto v1 = sv::drop_exactly(raw, 3);
    // Alternative syntax
    constexpr auto v2 = sv::drop_exactly(3)(raw);
    // Or even
    constexpr auto v3 = raw | sv::drop_exactly(3);

    // Size is changed
    static_assert(raw.size() - 3 == v1.size(), "");
    // Though not the extent, because we're passing 3 as a function argument
    // which implies it's not being constexpr.
    static_assert(raw.extent() == v1.extent(), "");
    // But if you _do_ want the extent to change as well, just pass
    // integral_constant rather than size_t to drop:
    constexpr auto v4 =
        sv::drop_exactly(std::integral_constant<std::size_t, 3>{})(raw);
    static_assert(raw.extent() - 3 == v4.extent(), "");

    // You can do all the common stuff like
    //
    // accessing elements
    static_assert(v1[1] == 5, "");
    // copying the view
    constexpr auto v5 = v1;
    // accessing the parent view
    static_assert(&raw == &v4.parent(), "");
    // etc.
}
