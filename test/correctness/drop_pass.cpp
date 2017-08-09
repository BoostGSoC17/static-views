//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../include/boost/static_views/detail/config.hpp"
#include "../../include/boost/static_views/detail/utils.hpp"
#include "../../include/boost/static_views/drop.hpp"
#include "../../include/boost/static_views/raw_view.hpp"
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

class non_copyable_int {
  public:
    constexpr non_copyable_int(int const x) noexcept : _payload{x} {}

    non_copyable_int(non_copyable_int const&) = delete;

    constexpr non_copyable_int(non_copyable_int&& other) noexcept
        : _payload{other._payload}
    {
    }

    non_copyable_int& operator         =(
        non_copyable_int const& other) = delete;
    non_copyable_int& operator=(non_copyable_int&& other) = delete;

    explicit constexpr operator int() noexcept { return _payload; }

  private:
    int _payload;
};

#define MAKE_DROP(x)                                                 \
    boost::static_views::drop(std::declval<std::size_t>())(          \
        std::declval<x>())

template <class T>
using compile_make_drop_t = decltype(MAKE_DROP(T));


#if defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)
# define NOEXCEPT(...) true
#else
# define NOEXCEPT(...) noexcept(__VA_ARGS__)
#endif


#define TEST_MAKE_IMPL(view_type, q)                                 \
    static_assert(boost::static_views::detail::is_detected<          \
                      compile_make_drop_t, view_type q>::value,      \
        "passing `" #view_type " " #q "` to `drop` doesn't work.");  \
    static_assert(NOEXCEPT(MAKE_DROP(view_type q)),                  \
        "passing `" #view_type " " #q                                \
        "` to `drop` is not noexcept.");                             \
    boost::static_views::concepts::View::check<boost::static_views:: \
            detail::detected_t<compile_make_drop_t, view_type q>>();

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
    using drop_view_1_t =
        detected_t<compile_make_drop_t, raw_view_1_t>;
    TEST_MAKE(drop_view_1_t);

    using data_2_t =
        std::add_lvalue_reference_t<non_copyable_int[20]>;
    using raw_view_2_t =
        std::decay_t<decltype(raw_view(std::declval<data_2_t>()))>;
    TEST_MAKE(raw_view_2_t);
    using drop_view_2_t =
        detected_t<compile_make_drop_t, raw_view_2_t>;
    TEST_MAKE(drop_view_2_t);
}

template <class T, std::size_t n, std::size_t expected,
    std::size_t... Is>
CONSTEXPR auto test_size_impl(std::index_sequence<Is...>)
{
    T          xs[] = {T{Is}...};
    auto const raw  = boost::static_views::raw_view(xs);
    return boost::static_views::drop(n)(raw).size() == expected;
}

template <class T, std::size_t N, std::size_t n, std::size_t expected>
CONSTEXPR auto test_size_impl()
{
    STATIC_ASSERT((test_size_impl<T, n, expected>(
                      std::make_index_sequence<N>{})),
        "drop::size() does not work correctly.");
}

auto test_size()
{
    // clang-format off
    //             type      data      how many     expected
    //                       size      to drop      size
    test_size_impl<int,        10,            5,           5>();
    test_size_impl<int,        13,            0,           13>();
    test_size_impl<int,         1,            5,           0>();
    test_size_impl<int,         7,            6,           1>();
    // clang-format on
}

auto test_access()
{
    static constexpr int data_1[] = {
        237, 833, 715, 164, /**/ 323, 528, 403, 586, 745, 657};
    static CONSTEXPR auto xs = boost::static_views::raw_view(data_1);

    // Through an lvalue rerefence
    CONSTEXPR auto v1 = boost::static_views::drop(4)(xs);

    STATIC_ASSERT(v1[0] == 323, "operator[] is broken.");
    STATIC_ASSERT(v1[2] == 403, "operator[] is broken.");
    STATIC_ASSERT(v1[5] == 657, "operator[] is broken.");
    STATIC_ASSERT(v1.unsafe_at(2) == v1[2], "unsafe_at() is broken.");
    STATIC_ASSERT(v1.unsafe_at(4) == v1[4], "unsafe_at() is broken.");
    BOOST_TEST_THROWS(v1[6], boost::static_views::out_of_bound);
    BOOST_TEST_THROWS(v1[7], boost::static_views::out_of_bound);
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(v1[0]), int const&>));
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(v1.unsafe_at(3)), int const&>));

    int  data_2[] = {10, 100, 1000, 1, 2, 3, 4, 5};
    auto v2       = boost::static_views::drop(3)(
        boost::static_views::raw_view(data_2));
    BOOST_TEST_TRAIT_TRUE((std::is_same<decltype(v2[0]), int&>));
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(v2.unsafe_at(0)), int&>));
    v2.unsafe_at(0) = 6;
    BOOST_TEST_EQ(v2[0], 6);
    v2[3] = 78;
    BOOST_TEST_EQ(v2.unsafe_at(3), 78);

    // BOOST_TEST_TRAIT_TRUE(
    //     (std::is_same<decltype(std::move(data_2)[0]), int&&>));
    // But should the following hold too?
    // BOOST_TEST_TRAIT_TRUE(
    //    (std::is_same<decltype(std::move(v2)[0]), int&&>));
}

auto test_copy_move()
{
    static constexpr int data_1[] = {1, 2, 3, 4};

    auto const v1 = boost::static_views::drop(1)(
        boost::static_views::raw_view(data_1));
    BOOST_STATIC_VIEWS_UNUSED decltype(v1) v2{v1};
    auto         v3 = v1;
    auto         v4 = std::move(v3);
    BOOST_STATIC_VIEWS_UNUSED decltype(v1) v5{std::move(v4)};
}

int main()
{
    test_make();
    test_size();
    test_access();
    test_copy_move();
    return boost::report_errors();
}
