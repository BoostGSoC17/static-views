//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../include/boost/static_views/detail/config.hpp"
#include "../../include/boost/static_views/raw_view.hpp"
#include "../../include/boost/static_views/slice.hpp"
#include <utility>
#include <boost/config.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>
#include <boost/detail/workaround.hpp>

#if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(BOOST_MSVC))
#define CONSTEXPR /* no constexpr for MSVC */
#define STATIC_ASSERT(expr, msg) BOOST_TEST(expr&& msg)
// #elif BOOST_WORKAROUND(BOOST_GCC, BOOST_TESTED_AT(BOOST_GCC))
// #define CONSTEXPR /* no constexpr for GCC due to this bug \
//                      https://gcc.gnu.org/bugzilla/show_bug.cgi?id=71504
//                      \
//                    */
// #define STATIC_ASSERT(expr, msg) BOOST_TEST(expr&& msg)
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

#define MAKE_SLICE(x)                                                \
    boost::static_views::slice(std::declval<std::size_t>(),          \
        std::declval<std::size_t>())(std::declval<x>())

template <class T>
using compile_make_slice_t = decltype(MAKE_SLICE(T));

#if defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)
#define NOEXCEPT(...) true
#else
#define NOEXCEPT(...) noexcept(__VA_ARGS__)
#endif

#define TEST_MAKE_IMPL(view_type, q)                                 \
    static_assert(boost::static_views::detail::is_detected<          \
                      compile_make_slice_t, view_type q>::value,     \
        "passing `" #view_type " " #q "` to `slice` doesn't work."); \
    static_assert(NOEXCEPT(MAKE_SLICE(view_type q)),                 \
        "passing `" #view_type " " #q                                \
        "` to `slice` is not noexcept.");                            \
    boost::static_views::concepts::View::check<                      \
        boost::static_views::detail::detected_t<                     \
            compile_make_slice_t, view_type q>>();

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
    using slice_view_1_t =
        detected_t<compile_make_slice_t, raw_view_1_t>;
    TEST_MAKE(slice_view_1_t);

    using data_2_t =
        std::add_lvalue_reference_t<non_copyable_int[20]>;
    using raw_view_2_t =
        std::decay_t<decltype(raw_view(std::declval<data_2_t>()))>;
    TEST_MAKE(raw_view_2_t);
    using slice_view_2_t =
        detected_t<compile_make_slice_t, raw_view_2_t>;
    TEST_MAKE(slice_view_2_t);
}

template <class T, std::size_t b, std::size_t e, std::size_t expected,
    std::size_t... Is>
CONSTEXPR auto test_size_impl(std::index_sequence<Is...>)
{
    T          xs[] = {T{Is}...};
    auto const raw  = boost::static_views::raw_view(xs);
    return boost::static_views::slice(b, e)(raw).size() == expected;
}

template <class T, std::size_t N, std::size_t b, std::size_t e,
    std::size_t expected>
CONSTEXPR auto test_size_impl()
{
    STATIC_ASSERT((test_size_impl<T, b, e, expected>(
                      std::make_index_sequence<N>{})),
        "slice::size() does not work correctly.");
}

auto test_size()
{
    // clang-format off
    //             type      data     begin      end      expected
    //                       size                             size
    test_size_impl<int,        10,        5,       5,            0>();
    test_size_impl<int,        13,        0,      13,           13>();
    test_size_impl<int,         1,        1,       1,            0>();
    test_size_impl<int,         7,        5,       7,            2>();
    // clang-format on
}

auto test_access()
{
    static constexpr int data_1[] = {
        237, 833, /**/ 715, 164, 323, 528, 403, 586, 745, /**/ 657};
    static CONSTEXPR auto xs = boost::static_views::raw_view(data_1);

    // Through an lvalue rerefence
    CONSTEXPR auto v1 = boost::static_views::slice(2, 9)(xs);

    STATIC_ASSERT(v1[0] == 715, "operator[] is broken.");
    STATIC_ASSERT(v1[2] == 323, "operator[] is broken.");
    STATIC_ASSERT(v1[6] == 745, "operator[] is broken.");
    STATIC_ASSERT(v1.unsafe_at(2) == v1[2], "unsafe_at() is broken.");
    STATIC_ASSERT(v1.unsafe_at(4) == v1[4], "unsafe_at() is broken.");
    BOOST_TEST_THROWS(v1[7], boost::static_views::out_of_bound);
    BOOST_TEST_THROWS(v1[8], boost::static_views::out_of_bound);
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(v1[0]), int const&>));
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<decltype(v1.unsafe_at(3)), int const&>));

    int  data_2[] = {10, 1, 2, 3, 4, 5, 100, 1000};
    auto v2       = boost::static_views::slice(1, 6)(
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

    auto const v1 = boost::static_views::slice(1, 4)(
        boost::static_views::raw_view(data_1));
    decltype(v1) v2{v1};
    auto         v3 = v1;
    auto         v4 = std::move(v3);
    decltype(v1) v5{std::move(v4)};
}

int main()
{
    test_make();
    test_size();
    test_access();
    test_copy_move();
    return boost::report_errors();
}
