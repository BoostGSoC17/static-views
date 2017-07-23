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
#include <boost/static_views/detail/utils.hpp>
#include <boost/static_views/raw_view.hpp>
#include <boost/static_views/take.hpp>

#if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(BOOST_MSVC))
#define CONSTEXPR /* no constexpr for MSVC */
#define STATIC_ASSERT(expr, msg) BOOST_TEST(expr&& msg)
#else
#define CONSTEXPR constexpr
#define STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#endif

template <class T, std::size_t N, std::size_t... Is>
auto create_impl(std::index_sequence<Is...>)
{
    // Notice the `static`.
    static constexpr T    xs[] = {((void)Is, T{})...};
    static CONSTEXPR auto raw1 = boost::static_views::raw_view(xs);
    CONSTEXPR auto        raw2 = boost::static_views::raw_view(xs);

    // View of a reference
    BOOST_ATTRIBUTE_UNUSED CONSTEXPR decltype(
        boost::static_views::take(std::declval<std::size_t>())(
            raw1)) v1[] = {
        ((void)Is, boost::static_views::take(Is)(raw1))...};
    STATIC_ASSERT(boost::static_views::detail::utils::all(noexcept(
                      boost::static_views::take(Is)(raw1))...),
        "take's noexcept specifiers are broken.");

    // View of an rvalue
    BOOST_ATTRIBUTE_UNUSED                      CONSTEXPR decltype(
        boost::static_views::take(std::declval<std::size_t>())(
            boost::static_views::raw_view(xs))) v2[] = {
        ((void)Is, boost::static_views::take(Is)(
                       boost::static_views::raw_view(xs)))...};
    STATIC_ASSERT(boost::static_views::detail::utils::all(
                      noexcept(boost::static_views::take(Is)(
                          boost::static_views::raw_view(xs)))...),
        "take's noexcept specifiers are broken.");

    BOOST_ATTRIBUTE_UNUSED                   CONSTEXPR decltype(
        boost::static_views::take(std::declval<std::size_t>())(
            std::declval<decltype(raw2)>())) v3[] = {((void)Is,
        boost::static_views::take(Is)(decltype(raw2){raw2}))...};
}

template <class T, std::size_t N>
auto test_create()
{
    return create_impl<T, N>(std::make_index_sequence<N>{});
}

template <class T, std::size_t... Is, std::size_t... Js>
auto size_impl(std::index_sequence<Is...>, std::index_sequence<Js...>)
{
    static constexpr T    xs[] = {((void)Is, T{})...};
    static CONSTEXPR auto raw  = boost::static_views::raw_view(xs);
    constexpr auto        N    = sizeof...(Is);

    STATIC_ASSERT(boost::static_views::detail::utils::all(
                      boost::static_views::take(Js)(raw).size()
                      == ((Js < N) ? Js : N)...),
        "take::size() does not work correctly.");
    STATIC_ASSERT(boost::static_views::detail::utils::all(noexcept(
                      boost::static_views::take(Js)(raw).size())...),
        "take's size() noexcept qualifiers are broken.");

    STATIC_ASSERT(
        boost::static_views::detail::utils::all(
            boost::static_views::take(Js)(raw).capacity() == N...),
        "take::capacity() does not work correctly.");
    STATIC_ASSERT(
        boost::static_views::detail::utils::all(noexcept(
            boost::static_views::take(Js)(raw).capacity())...),
        "take's capacity() noexcept qualifiers are broken.");
}

template <class T, std::size_t N, std::size_t M>
auto test_size()
{
    return size_impl<T>(
        std::make_index_sequence<N>{}, std::make_index_sequence<M>{});
}

template <std::size_t K, std::size_t... Is, std::size_t... Js>
auto access_impl(
    std::index_sequence<Is...>, std::index_sequence<Js...>)
{
    static constexpr std::size_t xs[] = {Is...};
    static CONSTEXPR auto raw1 = boost::static_views::raw_view(xs);

    // Through an lvalue rerefence
    CONSTEXPR auto v1 = boost::static_views::take(K)(raw1);
    // Compile-time access
    STATIC_ASSERT(boost::static_views::detail::utils::all(
                      (Js < v1.size()) ? (v1[Js] == Js) : true...),
        "take::operator[] const&  does not work correctly.");
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(v1[0]), std::size_t const&>));
    // Run-time access
    for (std::size_t j = 0; j < sizeof...(Js); ++j) {
        if (j < v1.size()) {
            BOOST_TEST_EQ(v1[j], j);
        }
        else {
            BOOST_TEST_THROWS(
                v1[j], boost::static_views::out_of_bound);
        }
    }

    // Through an rvalue reference
    // Compile-time access
    STATIC_ASSERT(
        boost::static_views::detail::utils::all(
            (Js < boost::static_views::take(K)(raw1).size())
                ? (boost::static_views::take(K)(raw1)[Js] == Js)
                : true...),
        "take::operator[] &&  does not work correctly.");
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(boost::static_views::take(K)(raw1)[0]),
            std::size_t const&>));
    // Run-time access
    for (std::size_t j = 0; j < sizeof...(Js); ++j) {
        if (j < v1.size()) {
            BOOST_TEST_EQ(boost::static_views::take(K)(raw1)[j], j);
        }
        else {
            BOOST_TEST_THROWS(boost::static_views::take(K)(raw1)[j],
                boost::static_views::out_of_bound);
        }
    }
}

template <std::size_t M, std::size_t... Is>
auto test_access_impl(std::index_sequence<Is...>)
{
    BOOST_ATTRIBUTE_UNUSED int dummy[] = {
        ((void)access_impl<Is>(
             std::make_index_sequence<sizeof...(Is)>{},
             std::make_index_sequence<M>{}),
            0)...};
}

template <std::size_t N, std::size_t M>
auto test_access()
{
    return test_access_impl<M>(std::make_index_sequence<N>{});
}

template <std::size_t K, std::size_t... Is, std::size_t... Js>
auto modify_impl(
    std::index_sequence<Is...>, std::index_sequence<Js...>)
{
    std::size_t xs[] = {Is...};
    auto const  raw1 = boost::static_views::raw_view(xs);

    // Through an lvalue rerefence
    auto const v1 = boost::static_views::take(K)(raw1);
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(v1[0]), std::size_t&>));
    for (std::size_t j = 0; j < sizeof...(Js); ++j) {
        if (j < v1.size()) {
            BOOST_TEST_EQ(v1[j], j);
            v1[j] = 0xABCD;
            BOOST_TEST_EQ(v1[j], 0xABCD);
            v1[j] = j;
            BOOST_TEST_EQ(v1[j], j);
        }
        else {
            BOOST_TEST_THROWS(
                v1[j], boost::static_views::out_of_bound);
        }
    }

    // Through an rvalue reference
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(boost::static_views::take(K)(raw1)[0]),
            std::size_t&>));
    for (std::size_t j = 0; j < sizeof...(Js); ++j) {
        if (j < boost::static_views::take(K)(raw1).size()) {
            BOOST_TEST_EQ(boost::static_views::take(K)(raw1)[j], j);
            boost::static_views::take(K)(raw1)[j] = 0xABCD;
            BOOST_TEST_EQ(
                boost::static_views::take(K)(raw1)[j], 0xABCD);
            boost::static_views::take(K)(raw1)[j] = j;
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
    BOOST_ATTRIBUTE_UNUSED int dummy[] = {
        ((void)modify_impl<Is>(
             std::make_index_sequence<sizeof...(Is)>{},
             std::make_index_sequence<M>{}),
            0)...};
}

template <std::size_t N, std::size_t M>
auto test_modify()
{
    return test_modify_impl<M>(std::make_index_sequence<N>{});
}

int main(void)
{
    test_create<int, 5>();
    test_create<double, 10>();
    test_create<std::pair<float, float>, 2>();

    test_size<int, 5, 8>();
    test_size<double, 10, 20>();
    test_size<std::pair<float, float>, 2, 3>();

    test_size<int, 5, 8>();
    test_size<double, 10, 20>();
    test_size<std::pair<float, float>, 2, 3>();

    test_access<5, 10>();
    test_access<20, 26>();
    test_access<1, 2>();

    test_modify<5, 10>();
    test_modify<3, 8>();

    return boost::report_errors();
}
