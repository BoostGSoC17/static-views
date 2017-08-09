//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../include/boost/static_views/raw_view.hpp"
#include <array>

int main()
{
    // Pass an rvalue in place of an lvalue.
    auto const view =
        boost::static_views::raw_view(std::array<int, 2>{4, 5});
}
