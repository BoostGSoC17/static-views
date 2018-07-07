//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/static_views/drop.hpp>
#include <boost/static_views/raw_view.hpp>

int main()
{
    static constexpr int data[] = {1, 2, 3, 4, 5};
    // Index out of bound error should be caught at compile-time
    constexpr auto fn = boost::static_views::drop_exactly(
        std::integral_constant<int, 8>{})(boost::static_views::raw_view(data));
}
