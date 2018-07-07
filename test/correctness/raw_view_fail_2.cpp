//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/static_views/raw_view.hpp>

struct MySequence {
};

namespace boost {
namespace static_views {
    template <>
    struct sequence_traits<MySequence> {
    };
} // namespace static_views
} // namespace boost

int main()
{
    MySequence seq{};
    // Pass a type that does not model the Sequence concept.
    // Reason: sequence_traits specialisation is missing a lot of stuff.
    auto const view = boost::static_views::raw_view(seq);
}
