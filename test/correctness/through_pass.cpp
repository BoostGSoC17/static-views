//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../include/boost/static_views/detail/config.hpp"
#include "../../include/boost/static_views/detail/utils.hpp"
#include "../../include/boost/static_views/raw_view.hpp"
#include "../../include/boost/static_views/through.hpp"
#include "testing.hpp"
#include <utility>

// Construction
// -------------------------------------------------------------------

auto test_make()
{
    static constexpr int  xs_data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    static constexpr auto xs_lvalue   = boost::static_views::raw_view(xs_data);

    static constexpr std::size_t ys_data[4] = {3, 5, 2, 8};
    static constexpr auto ys_lvalue = boost::static_views::raw_view(ys_data);
    constexpr auto zs_1 = boost::static_views::through(xs_lvalue, ys_lvalue);
    using zs_1_type     = std::remove_cv_t<decltype(zs_1)>;
    STATIC_ASSERT(zs_1_type::extent() == 4, "");
    STATIC_ASSERT(zs_1.size() == 4, "");
    STATIC_ASSERT(boost::static_views::View<zs_1_type>, "");
    STATIC_ASSERT(zs_1[0] == 4, "");
}


int main()
{
    test_make();
    return boost::report_errors();
}
