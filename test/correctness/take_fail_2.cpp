//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../include/boost/static_views/take.hpp"

int main()
{
    int data[] = {1, 2, 3, 4, 5};
    // Pass a non-size_t argument
    auto const view = boost::static_views::take(data);
}
