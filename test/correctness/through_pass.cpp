//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../include/boost/static_views/detail/config.hpp"
#include "../../include/boost/static_views/detail/utils.hpp"
#include "../../include/boost/static_views/raw_view.hpp"
#include "../../include/boost/static_views/slice.hpp"
#include "../../include/boost/static_views/through.hpp"
#include <utility>
#include <boost/config.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>
#include <boost/detail/workaround.hpp>

#if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(BOOST_MSVC))
#define CONSTEXPR /* no constexpr for MSVC */
#define STATIC_ASSERT(expr, msg) BOOST_TEST(expr&& msg)
#else
#define CONSTEXPR BOOST_STATIC_VIEWS_CONSTEXPR
#define STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#endif

int main()
{
    static constexpr std::size_t data_1[] = {1, 0, 1, 2, 5};
    static constexpr auto proxy = boost::static_views::slice(1, 4)(
        boost::static_views::raw_view(data_1));

    static constexpr int data_2[] = {1, 2, 3, 4, 5};
    constexpr auto       xs =
        boost::static_views::through(boost::static_views::slice(1, 4)(
            boost::static_views::raw_view(data_1)))(
            boost::static_views::raw_view(data_2));

    static_assert(xs[0] == 1, "");
    static_assert(xs.unsafe_at(0) == 1, "");
    static_assert(noexcept(xs.unsafe_at(0)), "");

    return boost::report_errors();
}
