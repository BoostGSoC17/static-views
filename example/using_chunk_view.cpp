//          Copyright Tom Westerhout 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <boost/static_views/pipe.hpp>     // for | syntax
#include <boost/static_views/raw_view.hpp> // to get boost::static_views::raw_view
#include <boost/static_views/chunk.hpp>    // to get boost::static_views::chunk

namespace sv = boost::static_views;

int main()
{
    // First of all, we need some view of the data
    static constexpr int  data[] = {1, 2, 3, 4, 5, 6};
    static constexpr auto raw    = sv::raw_view(data);

    // Splits raw into chunks of 3 elements
    constexpr auto v1 = sv::chunk(raw, 3);

    // The docs promised to return a View
    using v1_type = std::remove_const_t<decltype(v1)>;
    static_assert(sv::View<v1_type>, "Since when is a chunk view not a View?");

    // Each element of the chunk view is itself a view, so we can iterate over
    // it.
    for (auto x : v1[1])
        std::cout << x << '\n';

    // If a compile-time constant is passed instead
    constexpr auto v2 = sv::chunk(raw, std::integral_constant<int, 3>{});
    // Then elements will have static extent
    static_assert(v2.unsafe_at(0).extent() == 3,
        "The size of v2[0] _is_ known at compile-time!");
}
