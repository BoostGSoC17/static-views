//          copyright tom westerhout 2017.
// distributed under the boost software license, version 1.0.
//    (see accompanying file license_1_0.txt or copy at
//          http://www.boost.org/license_1_0.txt)

#include "../../include/boost/static_views/raw_view.hpp"
#include <string>

struct MySequence {
};

namespace boost {
namespace static_views {
    template <>
    struct sequence_traits<MySequence> {
        using type = double;

        static auto at(double const&, std::size_t) -> int;
        static auto size() noexcept -> std::size_t;
    };
} // namespace static_views
} // namespace boost

int main()
{
    // Pass an rvalue in place of an lvalue.
    auto const view = boost::static_views::raw_view(MySequence{});
}
