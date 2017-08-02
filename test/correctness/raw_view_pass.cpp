//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../include/boost/static_views/detail/config.hpp"
#include "../../include/boost/static_views/raw_view.hpp"
#include <utility>
#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>

#if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(BOOST_MSVC))
#define CONSTEXPR /* no constexpr for MSVC */
#define STATIC_ASSERT(expr, msg) BOOST_TEST(expr&& msg)
#else
#define CONSTEXPR constexpr
#define STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#endif

struct ObscureSequence {
};

namespace boost {
namespace static_views {

    template <>
    struct sequence_traits<ObscureSequence>
        : sequence_traits_default<ObscureSequence> {

        static constexpr auto at(ObscureSequence const& /*unused*/,
            std::size_t /*unused*/) noexcept -> int
        {
            return 12345;
        }

        static constexpr auto extent() noexcept
            // something convertible to ptrdiff_t, but not really
            // ptrdiff_t
            -> int
        {
            return 20;
        }

        static constexpr auto size(ObscureSequence const&) noexcept
        {
            return extent();
        }
    };

} // namespace static_views
} // namespace boost

int main(void)
{
    int                              as[]  = {1, 2, 3, 4, 5};
    int const                        bs[]  = {1, 2, 3};
    static constexpr int             cs[]  = {1, 2, 3, 4};
    constexpr int                    cs2[] = {1, 2, 3, 4};
    static constexpr ObscureSequence ds    = {};

    auto const     as_view = boost::static_views::raw_view(as);
    auto const     bs_view = boost::static_views::raw_view(bs);
    CONSTEXPR auto cs_view = boost::static_views::raw_view(cs);
    CONSTEXPR auto ds_view = boost::static_views::raw_view(ds);

    BOOST_TEST_EQ(as_view.size(), 5);
    BOOST_TEST_EQ(bs_view.size(), 3);
    STATIC_ASSERT(cs_view.size() == 4, "size() is broken.");
    STATIC_ASSERT(boost::static_views::raw_view(cs2).size() == 4,
        "size() is broken.");
    // Even though ObscureSequence's size() returns int, raw_view's
    // size() and extent() should still return size_t.
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(ds_view.size()), std::size_t>));
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(ds_view.extent()), std::ptrdiff_t>));

    BOOST_TEST_EQ(as_view.size(), decltype(as_view)::extent());
    BOOST_TEST_EQ(bs_view.size(), decltype(bs_view)::extent());
    STATIC_ASSERT(cs_view.size() == decltype(cs_view)::extent(),
        "size() is broken.");

    for (std::size_t i = 0; i < as_view.size(); ++i) {
        BOOST_TEST_EQ(as_view[i], i + 1);
    }
    BOOST_TEST_THROWS(
        as_view[as_view.size()], boost::static_views::out_of_bound);
    BOOST_TEST_THROWS(as_view[as_view.size() + 1],
        boost::static_views::out_of_bound);
    // unsafe_at() still provides _unsafe_ noexcept access, no bounds
    // checking
    STATIC_ASSERT(noexcept(as_view.unsafe_at(as_view.size())),
        "unsafe_at() not noexcept");
    STATIC_ASSERT(cs_view[0] == 1, "operator[] is broken.");
    STATIC_ASSERT(
        cs_view.unsafe_at(2) == cs_view[2], "operator[] is broken.");
    STATIC_ASSERT(cs_view[3] == 4, "operator[] is broken.");
    STATIC_ASSERT(boost::static_views::raw_view(cs2)[0] == 1,
        "operator[] is broken.");
    STATIC_ASSERT(boost::static_views::raw_view(cs2)[3] == 4,
        "operator[] is broken.");

    BOOST_TEST_TRAIT_TRUE((std::is_same<decltype(as_view[1]), int&>));
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(bs_view[2]), int const&>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<decltype(ds_view[2]), int>));

    return boost::report_errors();
}
