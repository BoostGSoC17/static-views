//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <utility>
#include <vector>

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/lfold.hpp>
#include <boost/static_views/raw_view.hpp>

#if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(BOOST_MSVC))
#define CONSTEXPR /* no constexpr for MSVC */
#define STATIC_ASSERT(expr, msg) BOOST_TEST(expr&& msg)
#else
#define CONSTEXPR constexpr
#define STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#endif

template <class OStream, class T>
auto operator<<(OStream& out, std::vector<T> const& xs) -> OStream&
{
    out << '[';
    if (xs.size() > 0) {
        out << xs[0];
        std::for_each(std::next(std::begin(xs)), std::end(xs),
            [&out](auto&& x) { out << ", " << x; });
    }
    out << ']';
    return out;
}

// Clang 3.8 wants operator<< to be defined before the BOOST_TEST_*
// macro
#include <boost/core/lightweight_test.hpp>

// Tests from stdlibc++-v3 test suite
auto test01()
{
    constexpr int  xs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    CONSTEXPR auto sum  = boost::static_views::lfold(
        boost::static_views::raw_view(xs), 11);
    STATIC_ASSERT(sum == 66, "lfold() is broken.");
}

auto test02()
{
    static constexpr bool xs[] = {true, false, true, true, false,
        true, false, true, true, false};
    CONSTEXPR auto        ys   = boost::static_views::raw_view(xs);
    CONSTEXPR auto        sum  = boost::static_views::lfold(ys, 100);
    STATIC_ASSERT(sum == 106, "lfold() is broken.");
}

auto test03()
{
    constexpr int xs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto const    ys =
        boost::static_views::lfold(boost::static_views::raw_view(xs),
            std::vector<int>{}, [](auto&& v, auto&& x) {
                v.push_back(x);
                return std::move(v);
            });
    BOOST_TEST_EQ(
        ys, (std::vector<int>{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}}));
}

int main(void)
{
    test01();
    test02();
    test03();

    return boost::report_errors();
}
