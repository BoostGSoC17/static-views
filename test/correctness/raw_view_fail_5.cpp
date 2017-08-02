//          copyright tom westerhout 2017.
// distributed under the boost software license, version 1.0.
//    (see accompanying file license_1_0.txt or copy at
//          http://www.boost.org/license_1_0.txt)

#include "../../include/boost/static_views/raw_view.hpp"

struct MySequence {
};

namespace boost {
namespace static_views {
    template <>
    struct sequence_traits<MySequence> {
        using type = double;

        static auto at(double const&, std::size_t) -> int;
        static auto size() -> std::size_t;
    };
} // namespace static_views
} // namespace boost

int main()
{
    MySequence seq{};
    // Pass a type that does not model the Sequence concept.
    // Reason: `size` function not noexcept.
    auto const view = boost::static_views::raw_view(seq);
}
