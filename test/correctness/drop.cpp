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
#include <boost/static_views/drop.hpp>
#include <boost/static_views/raw_view.hpp>

#if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(BOOST_MSVC))
#define CONSTEXPR /* no constexpr for MSVC */
#define STATIC_ASSERT(expr, msg) BOOST_TEST(expr&& msg)
#else
#define CONSTEXPR BOOST_STATIC_VIEWS_CONSTEXPR
#define STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#endif


class non_copyable_int {
  public:
    constexpr non_copyable_int(int const x) noexcept
        : _payload{x}
    {
    }

    non_copyable_int(non_copyable_int const&) = delete;

    constexpr non_copyable_int(non_copyable_int&& other) noexcept
        : _payload{other._payload}
    {
    }

    non_copyable_int& operator=(non_copyable_int const& other) = delete;
    non_copyable_int& operator=(non_copyable_int&& other) = delete;

    explicit constexpr operator int() noexcept
    {
        return _payload;
    }

  private:
    int _payload;
};


#define MAKE_DROP(x)                                                 \
    boost::static_views::drop(std::declval<std::size_t>())(          \
        std::declval<x>())

template <class T>
using compile_make_drop_t = decltype( MAKE_DROP(T) );

#define TEST_MAKE_IMPL(view_type, q)                                 \
    static_assert(boost::static_views::detail::is_detected<          \
                      compile_make_drop_t, view_type q>::value,      \
        "passing `" #view_type " " #q "` to `drop` doesn't work.");  \
    static_assert(noexcept(MAKE_DROP(view_type q)),                  \
        "passing `" #view_type " " #q                                \
        "` to `drop` is not noexcept.");                             \
    boost::static_views::detail::concepts::View::check<              \
        boost::static_views::detail::detected_t<compile_make_drop_t, \
            view_type q>>();

#define TEST_MAKE(view_type)                                         \
    do {                                                             \
        TEST_MAKE_IMPL(view_type, &)                                 \
        TEST_MAKE_IMPL(view_type, const&)                            \
        TEST_MAKE_IMPL(view_type, &&)                                \
        TEST_MAKE_IMPL(view_type, const&&)                           \
    } while (false)

auto test_make()
{
    using namespace boost::static_views;
    using detail::detected_t;

    using data_1_t = std::add_lvalue_reference_t<int[20]>;
    using raw_view_1_t =
        std::decay_t<decltype(raw_view(std::declval<data_1_t>()))>;
    TEST_MAKE(raw_view_1_t);
    using drop_view_1_t = detected_t<compile_make_drop_t, raw_view_1_t>;
    TEST_MAKE(drop_view_1_t);

    using data_2_t = std::add_lvalue_reference_t<non_copyable_int[20]>;
    using raw_view_2_t =
        std::decay_t<decltype(raw_view(std::declval<data_2_t>()))>;
    TEST_MAKE(raw_view_2_t);
    using drop_view_2_t = detected_t<compile_make_drop_t, raw_view_2_t>;
    TEST_MAKE(drop_view_2_t);
}

template <class T, std::size_t n, std::size_t expected,
    std::size_t... Is>
CONSTEXPR auto test_size_impl(std::index_sequence<Is...>)
{
    T xs[] = {T{Is}...};
    auto const raw = boost::static_views::raw_view(xs);
    return boost::static_views::drop(n)(raw).size() == expected;
}

template <class T, std::size_t N, std::size_t n, std::size_t expected>
CONSTEXPR auto test_size_impl()
{
    STATIC_ASSERT(
        (test_size_impl<T, n, expected>(std::make_index_sequence<N>{})),
        "drop::size() does not work correctly.");
}

auto test_size()
{
    //             type      data      how many     expected
    //                       size      to drop      size
    test_size_impl<int,        10,            5,           5>();
    test_size_impl<int,        13,            0,           13>();
    test_size_impl<int,         1,            5,           0>();
    test_size_impl<int,         7,            6,           1>();
}



template <std::size_t K, std::size_t... Is, std::size_t... Js>
auto access_impl(
    std::index_sequence<Is...>, std::index_sequence<Js...>)
{
    static constexpr std::size_t xs[] = {Is...};
    constexpr auto               N    = sizeof...(Is);
    static CONSTEXPR auto raw1 = boost::static_views::raw_view(xs);

    // Through an lvalue rerefence
    CONSTEXPR auto v1 = boost::static_views::drop(K)(raw1);
    // Compile-time access
    STATIC_ASSERT(
        boost::static_views::detail::utils::all(
            (Js < v1.size()) ? (v1[Js] == K + Js) : true...),
        "drop::operator[] const&  does not work correctly.");
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(v1[0]), std::size_t const&>));
    // Run-time access
    for (std::size_t j = 0; j < sizeof...(Js); ++j) {
        if (K + j < N) {
            BOOST_TEST_EQ(v1[j], K + j);
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
            (Js < boost::static_views::drop(K)(raw1).size())
                ? (boost::static_views::drop(K)(raw1)[Js] == K + Js)
                : (true)...),
        "drop::operator[] &&  does not work correctly.");
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(boost::static_views::drop(K)(raw1)[0]),
            std::size_t const&>));
    // Run-time access
    for (std::size_t j = 0; j < sizeof...(Js); ++j) {
        if (K + j < N) {
            BOOST_TEST_EQ(
                boost::static_views::drop(K)(raw1)[j], K + j);
        }
        else {
            BOOST_TEST_THROWS(boost::static_views::drop(K)(raw1)[j],
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
    std::size_t    xs[] = {Is...};
    constexpr auto N    = sizeof...(Is);
    auto const     raw1 = boost::static_views::raw_view(xs);

    // Through an lvalue rerefence
    auto const v1 = boost::static_views::drop(K)(raw1);
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(v1[0]), std::size_t&>));
    for (std::size_t j = 0; j < sizeof...(Js); ++j) {
        if (K + j < N) {
            BOOST_TEST_EQ(v1[j], K + j);
            v1[j] = 0xABCD;
            BOOST_TEST_EQ(v1[j], 0xABCD);
            v1[j] = K + j;
            BOOST_TEST_EQ(v1[j], K + j);
        }
        else {
            BOOST_TEST_THROWS(
                v1[j], boost::static_views::out_of_bound);
        }
    }

    // Through an rvalue reference
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(boost::static_views::drop(K)(raw1)[0]),
            std::size_t&>));
    for (std::size_t j = 0; j < sizeof...(Js); ++j) {
        if (K + j < N) {
            BOOST_TEST_EQ(
                boost::static_views::drop(K)(raw1)[j], K + j);
            boost::static_views::drop(K)(raw1)[j] = 0xABCD;
            BOOST_TEST_EQ(
                boost::static_views::drop(K)(raw1)[j], 0xABCD);
            boost::static_views::drop(K)(raw1)[j] = K + j;
            BOOST_TEST_EQ(
                boost::static_views::drop(K)(raw1)[j], K + j);
        }
        else {
            BOOST_TEST_THROWS(boost::static_views::drop(K)(raw1)[j],
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
    test_make();
    test_size();

    test_size();

    test_access<5, 10>();
    test_access<20, 26>();
    test_access<1, 2>();

    test_modify<5, 10>();
    test_modify<3, 8>();

    return boost::report_errors();
}
