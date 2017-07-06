//          copyright tom westerhout 2017.
// distributed under the boost software license, version 1.0.
//    (see accompanying file license_1_0.txt or copy at
//          http://www.boost.org/license_1_0.txt)

#include <cassert>
#include <utility>

#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/raw_view.hpp>


int main(void)
{
    int as[]                               = {1, 2, 3, 4, 5};
    int const bs[]                         = {1, 2, 3};
    static constexpr int cs[]              = {1, 2, 3, 4};
    constexpr int cs2[]                    = {1, 2, 3, 4};
    static constexpr std::array<int, 5> ds {{1, 2, 3, 4, 5}};

    auto const as_view = boost::static_views::raw_view(as);
    auto const bs_view = boost::static_views::raw_view(bs);
    constexpr auto cs_view = boost::static_views::raw_view(cs);
#if BOOST_WORKAROUND(BOOST_GCC, < 70000)
    // nothing
#else
    BOOST_CONSTEXPR_AFTER_CXX14
#endif
        auto ds_view       = boost::static_views::raw_view(ds);

    BOOST_TEST_EQ(as_view.size(), 5);
    BOOST_TEST_EQ(bs_view.size(), 3);
    static_assert(cs_view.size() == 4, "size() is broken.");
    static_assert(boost::static_views::raw_view(cs2).size() == 4,
        "size() is broken.");
    BOOST_TEST_EQ(ds_view.size(), 5);

    BOOST_TEST_EQ(as_view.size(), decltype(as_view)::capacity());
    BOOST_TEST_EQ(bs_view.size(), decltype(bs_view)::capacity());
    static_assert(cs_view.size() == decltype(cs_view)::capacity(),
        "size() is broken.");
    BOOST_TEST_EQ(ds_view.size(), decltype(ds_view)::capacity());

    for (std::size_t i = 0; i < as_view.size(); ++i) {
        BOOST_TEST_EQ(as_view[i], i + 1);
    }
    BOOST_TEST_THROWS(as_view[as_view.size()],
        boost::static_views::out_of_bound);
    BOOST_TEST_THROWS(as_view[as_view.size() + 1],
        boost::static_views::out_of_bound);
    static_assert(cs_view[0] == 1, "operator[] is broken.");
    static_assert(cs_view[3] == 4, "operator[] is broken.");
    static_assert(boost::static_views::raw_view(cs2)[0] == 1, 
        "operator[] is broken.");
    static_assert(boost::static_views::raw_view(cs2)[3] == 4, 
        "operator[] is broken.");

#if BOOST_WORKAROUND(BOOST_GCC, < 70000)
    // nothing
#else
    BOOST_CONSTEXPR_AFTER_CXX14
#endif
        auto ds0 = ds_view[0];

#if BOOST_WORKAROUND(BOOST_GCC, < 70000)
    // nothing
#else
    BOOST_CONSTEXPR_AFTER_CXX14
#endif
        auto ds4 = ds_view[4];

    BOOST_TEST_EQ(ds0, 1);
    BOOST_TEST_EQ(ds4, 5);

    BOOST_TEST_TRAIT_TRUE(( std::is_same<int&, decltype(as_view[1])> ));
    BOOST_TEST_TRAIT_TRUE(( std::is_same<int const&, decltype(bs_view[2])> ));
    BOOST_TEST_TRAIT_TRUE(( std::is_same<int const&, decltype(cs_view[2])> ));
    BOOST_TEST_TRAIT_TRUE(( std::is_same<int const&, decltype(ds_view[0])> ));

    return boost::report_errors();
}


