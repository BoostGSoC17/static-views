//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <utility>
#include <boost/config.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>
#include <boost/detail/workaround.hpp>

#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/hashed.hpp>
#include <boost/static_views/raw_view.hpp>

#if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(BOOST_MSVC))
#define CONSTEXPR /* no constexpr for MSVC */
#define STATIC_ASSERT(expr, msg) BOOST_TEST(expr&& msg)
#else
#define CONSTEXPR constexpr
#define STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#endif

/*
template <class T, std::size_t N, std::size_t... Is, std::size_t...
Js> auto create_impl(std::index_sequence<Is...>,
std::index_sequence<Js...>)
{
    // Notice the `static`.
    static constexpr T    xs[] = { ((void)Is, T{})... };
    static CONSTEXPR auto raw1 = boost::static_views::raw_view(xs);
           CONSTEXPR auto raw2 = boost::static_views::raw_view(xs);

    // View of a reference
    BOOST_ATTRIBUTE_UNUSED CONSTEXPR
decltype(boost::static_views::slice( std::declval<std::size_t>(),
std::declval<std::size_t>())(raw1)) v1[] = { ( (Js / N <= Js % N) ?
boost::static_views::slice(Js / N, Js % N)(raw1) :
boost::static_views::slice(Js / N, Js / N)(raw1) )... };

    // View of an rvalue
    BOOST_ATTRIBUTE_UNUSED CONSTEXPR
decltype(boost::static_views::slice( std::declval<std::size_t>(),
std::declval<std::size_t>())( boost::static_views::raw_view(xs))) v2[]
= { ( (Js / N <= Js % N) ? boost::static_views::slice(Js / N, Js % N)(
                    boost::static_views::raw_view(xs))
                : boost::static_views::slice(Js / N, Js / N)(
                    boost::static_views::raw_view(xs)) )... };

    BOOST_ATTRIBUTE_UNUSED CONSTEXPR
decltype(boost::static_views::slice( std::declval<std::size_t>(),
std::declval<std::size_t>())( std::declval<decltype(raw2)>())) v3[] =
            { ( (Js / N <= Js % N)
                ? boost::static_views::slice(Js / N, Js % N)(
                    decltype(raw2){raw2})
                : boost::static_views::slice(Js / N, Js / N)(
                    decltype(raw2){raw2}) )... };
}

template <class T, std::size_t N, std::size_t M>
auto test_create()
{
    return create_impl<T, M>(std::make_index_sequence<N>{},
        std::make_index_sequence<M * M>{});
}


template <std::size_t N>
constexpr auto all(bool const (&rng)[N])
{
    for (std::size_t i = 0; i < N; ++i) {
        if (!rng[i]) return false;
    }
    return true;
}


template <class T, std::size_t N, std::size_t... Is, std::size_t...
Js> auto size_impl(std::index_sequence<Is...>,
std::index_sequence<Js...>)
{
    static constexpr T    xs[] = { ((void)Is, T{})... };
    static CONSTEXPR auto raw1 = boost::static_views::raw_view(xs);

    BOOST_ATTRIBUTE_UNUSED CONSTEXPR std::size_t sizes[] =
        { ( (Js / N <= Js % N)
            ? boost::static_views::slice(Js / N, Js % N)(raw1)
            : boost::static_views::slice(Js / N, Js / N)(raw1)
).size()... };

    BOOST_ATTRIBUTE_UNUSED CONSTEXPR std::size_t capacities[] =
        { ( (Js / N <= Js % N)
            ? boost::static_views::slice(Js / N, Js % N)(raw1)
            : boost::static_views::slice(Js / N, Js /
N)(raw1)).capacity()... };

    for (std::size_t i = 0; i < N; ++i) {
        for (std::size_t j = 0; j < N; ++j) {
            if (i <= j) {
                BOOST_TEST_EQ((boost::static_views::slice(i,
j)(raw1).size()), (static_cast<std::size_t>(std::min(j, raw1.size()) -
                        std::min(i, raw1.size()))));
                BOOST_TEST_EQ(boost::static_views::slice(i,
j)(raw1).capacity(), sizeof...(Is));
            }
            else {
                BOOST_TEST_THROWS((boost::static_views::slice(i,
j)(raw1).size()),
                    boost::static_views::invalid_range);
            }
        }
    }
}

template <class T, std::size_t N, std::size_t M>
auto test_size()
{
    return size_impl<T, M>(std::make_index_sequence<N>{},
        std::make_index_sequence<M * M>{});
}


template <std::size_t B, std::size_t E, std::size_t... Is,
std::size_t... Js> auto access_impl(std::index_sequence<Is...>,
std::index_sequence<Js...>)
{
    static constexpr std::size_t xs[] = { Is... };
    static CONSTEXPR auto        raw1 =
boost::static_views::raw_view(xs);

    // Through an lvalue rerefence
    CONSTEXPR auto v1 = boost::static_views::slice(B, E)(raw1);
    // Compile-time access
    CONSTEXPR bool lvalue_results_compile [] =
        { ((Js < v1.size()) ? (v1[Js] == Js + B) : (true))... };
    BOOST_TEST_TRAIT_TRUE(( std::is_same<decltype(v1[0]),
        std::size_t const&> ));
    STATIC_ASSERT(all(lvalue_results_compile),
        "take::operator[] const&  does not work correctly.");
    // Run-time access
    for (std::size_t j = 0; j < sizeof...(Js); ++j) {
        if (j < v1.size()) {
            BOOST_TEST_EQ(v1[j], j + B);
        }
        else {
            BOOST_TEST_THROWS(v1[j],
boost::static_views::out_of_bound);
        }
    }

    // Through an rvalue reference
    // Compile-time access
    CONSTEXPR bool rvalue_results_compile[] =
        { ( (Js < boost::static_views::slice(B, E)(raw1).size())
            ? (boost::static_views::slice(B, E)(raw1)[Js] == Js + B)
            : (true) )... };
    BOOST_TEST_TRAIT_TRUE(( std::is_same<decltype(
        boost::static_views::slice(B, E)(raw1)[0]), std::size_t
const&> )); STATIC_ASSERT(all(rvalue_results_compile),
        "take::operator[] &&  does not work correctly.");
    // Run-time access
    for (std::size_t j = 0; j < sizeof...(Js); ++j) {
        if (j < boost::static_views::slice(B, E)(raw1).size()) {
            BOOST_TEST_EQ(boost::static_views::slice(B, E)(raw1)[j],
j);
        }
        else {
            BOOST_TEST_THROWS(boost::static_views::slice(B,
E)(raw1)[j], boost::static_views::out_of_bound);
        }
    }
}

template <std::size_t N, std::size_t M, std::size_t K>
auto test_access_impl_2(std::index_sequence<>)
{
}

template <std::size_t N, std::size_t M, std::size_t K, std::size_t...
Is> auto test_access_impl_2(std::index_sequence<Is...>)
{
    BOOST_ATTRIBUTE_UNUSED int dummy[] =
        { ((void)access_impl<Is, K>(std::make_index_sequence<
                N>{}, std::make_index_sequence<M>{}), 0)... };
}

template <std::size_t N, std::size_t M>
auto test_access_impl_1(std::index_sequence<>)
{
}

template <std::size_t N, std::size_t M, std::size_t... Is>
auto test_access_impl_1(std::index_sequence<Is...>)
{
    BOOST_ATTRIBUTE_UNUSED int dummy[] =
        { ((void)test_access_impl_2<N, M, Is>(
               std::make_index_sequence<Is>{}), 0) ... };
}

template <std::size_t N, std::size_t M, std::size_t K>
auto test_access()
{
    return test_access_impl_1<N, M>(std::make_index_sequence<K>{});
}


template <std::size_t K, std::size_t... Is, std::size_t... Js>
auto modify_impl(std::index_sequence<Is...>,
std::index_sequence<Js...>)
{
    std::size_t xs[] = { Is... };
    auto const raw1 = boost::static_views::raw_view(xs);

    // Through an lvalue rerefence
    auto const v1 = boost::static_views::take(K)(raw1);
    BOOST_TEST_TRAIT_TRUE(( std::is_same<decltype(v1[0]),
std::size_t&> )); for (std::size_t j = 0; j < sizeof...(Js); ++j) { if
(j < v1.size()) { BOOST_TEST_EQ(v1[j], j); v1[j] = 0xABCD;
            BOOST_TEST_EQ(v1[j], 0xABCD);
            v1[j] = j;
            BOOST_TEST_EQ(v1[j], j);
        }
        else {
            BOOST_TEST_THROWS(v1[j],
boost::static_views::out_of_bound);
        }
    }

    // Through an rvalue reference
    BOOST_TEST_TRAIT_TRUE(( std::is_same<decltype(
        boost::static_views::take(K)(raw1)[0]), std::size_t&> ));
    for (std::size_t j = 0; j < sizeof...(Js); ++j) {
        if (j < boost::static_views::take(K)(raw1).size()) {
            BOOST_TEST_EQ(boost::static_views::take(K)(raw1)[j], j);
            boost::static_views::take(K)(raw1)[j] = 0xABCD;
            BOOST_TEST_EQ(boost::static_views::take(K)(raw1)[j],
0xABCD); boost::static_views::take(K)(raw1)[j] = j;
            BOOST_TEST_EQ(boost::static_views::take(K)(raw1)[j], j);
        }
        else {
            BOOST_TEST_THROWS(boost::static_views::take(K)(raw1)[j],
                boost::static_views::out_of_bound);
        }
    }
}

template <std::size_t M, std::size_t... Is>
auto test_modify_impl(std::index_sequence<Is...>)
{
    BOOST_ATTRIBUTE_UNUSED int dummy[] =
        {
((void)modify_impl<Is>(std::make_index_sequence<sizeof...(Is)>{},
            std::make_index_sequence<M>{}), 0)... };
}

template <std::size_t N, std::size_t M>
auto test_modify()
{
    return test_modify_impl<M>(std::make_index_sequence<N>{});
}
*/

int main(void)
{
    struct hasher {
        constexpr auto operator()(int const x) const noexcept
        {
            return static_cast<std::size_t>(x);
        }
    };

    static constexpr int xs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    BOOST_STATIC_VIEWS_CONSTEXPR auto vs =
        boost::static_views::hashed<5, 3>(hasher{})(
            boost::static_views::raw_view(xs));

    for (std::size_t i = 0; i < vs.bucket_count(); ++i) {
        std::cout << "i = " << i << ": [";
        for (std::size_t j = 0; j < vs[i].size(); ++j) {
            std::cout << vs[i][j] << ", ";
        }
        std::cout << "]\n";
    }
    /*
    test_create<int, 5, 6>();
    test_create<double, 10, 15>();
    test_create<std::pair<float, float>, 2, 4>();

    test_size<int, 5, 8>();
    test_size<double, 10, 20>();
    test_size<std::pair<float, float>, 2, 3>();

    // test_access<5, 10, 6>();
    // test_access<20, 26, 22>();
    test_access<1, 2, 3>();

    test_modify<5, 10>();
    test_modify<3, 8>();
    */

    return boost::report_errors();
}
