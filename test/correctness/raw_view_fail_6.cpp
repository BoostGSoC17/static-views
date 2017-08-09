//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../include/boost/static_views/raw_view.hpp"
#include <string>

struct MySequence {
};

namespace boost {
namespace static_views {
    template <>
    struct sequence_traits<MySequence> {
        static auto at(MySequence const&, std::size_t) -> int;
        static constexpr auto extent() noexcept -> int;
        static auto size(MySequence const&) noexcept -> std::string;
    };
} // namespace static_views
} // namespace boost

int main()
{
    MySequence seq{};
    // Pass a type that does not model the Sequence concept.
    // Reason: `size` function's return type is not convertible to
    // size_t.
    auto const view = boost::static_views::raw_view(seq);
}
