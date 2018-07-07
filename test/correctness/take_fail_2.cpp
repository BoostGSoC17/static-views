//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/static_views/raw_view.hpp>
#include <boost/static_views/take.hpp>

int main()
{
    int data[] = {1, 2, 3, 4, 5};
    // Out of bound errors should be caught at compile-time.
    auto const view = boost::static_views::take_exactly(
        std::integral_constant<int, 8>{})(boost::static_views::raw_view(data));
}
