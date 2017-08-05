//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../include/boost/static_views/raw_view.hpp"

struct MySequence {
};

int main()
{
    MySequence seq{};
    // Pass a type that does not model the Sequence concept.
    // Reason: no specialisation of sequence_traits.
    auto const view = boost::static_views::raw_view(seq);
}
