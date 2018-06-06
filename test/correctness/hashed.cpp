//          Copyright Tom Westerhout 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "testing.hpp"
#include <utility>
#include <boost/static_views/hashed.hpp>
#include <boost/static_views/raw_view.hpp>

auto test_make()
{
    struct hasher {
        constexpr auto operator()(int const x) const noexcept
        {
            return static_cast<std::size_t>(x);
        }
    };

    static constexpr int  xs_data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    static constexpr auto xs_lvalue   = boost::static_views::raw_view(xs_data);

    auto const ys = boost::static_views::hashed<6, 2>(xs_lvalue, hasher{});
    using ys_type = std::remove_cv_t<decltype(ys)>;
    STATIC_ASSERT(boost::static_views::View<ys_type>, "");
}


int main()
{
    test_make();
    return boost::report_errors();
}
