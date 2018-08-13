//          Copyright Tom Westerhout 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <boost/static_views/pipe.hpp>     // for | syntax
#include <boost/static_views/raw_view.hpp> // to get boost::static_views::raw_view
#include <boost/static_views/flatten.hpp>    // to get boost::static_views::chunk

namespace sv = boost::static_views;

int main()
{
    // First of all, we need some view of the data
    static constexpr int  data[] = {1, 2, 3, 4, 5, 6};
    static constexpr decltype(sv::raw_view(data)) data_2[] = {
        sv::raw_view(data), sv::raw_view(data), sv::raw_view(data)};

    // Splits raw into chunks of 3 elements
    constexpr auto v1 = sv::flatten(sv::raw_view(data_2));

    // The docs promised to return a View
    using v1_type = std::remove_const_t<decltype(v1)>;
    static_assert(sv::View<v1_type>, "Since when is a chunk view not a View?");

    static_assert(v1.size() == 6 * 3, "");
    // Each element of the chunk view is itself a view, so we can iterate over
    // it.
    for (auto x : v1)
        std::cout << x << '\n';

}
