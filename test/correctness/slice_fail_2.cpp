//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../include/boost/static_views/slice.hpp"

int main()
{
    // Pass wrong number of arguments
    auto const view = boost::static_views::slice(2);
}
