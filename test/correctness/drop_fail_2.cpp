//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../include/boost/static_views/drop.hpp"

int main()
{
    static constexpr int data[] = {1, 2, 3, 4, 5};
    // Pass a non-size_t argument
    constexpr auto fn = boost::static_views::drop(data);
}
