//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "testing.hpp"
#include <utility>
#include <boost/static_views/sequence_traits.hpp>

auto test_carray_traits()
{
    using S               = int[10];
    static constexpr S xs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    // const here is due to the constexpr above
    using Traits = boost::static_views::sequence_traits<S const>;

    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<Traits::size_type, unsigned>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<Traits::index_type, int>));
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<Traits::difference_type, int>));
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<Traits::value_type, int const>));
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<Traits::reference, int const&>));
    S ys = {};
    STATIC_ASSERT(
        (std::is_same<decltype(boost::static_views::sequence_traits<
                          S>::at(ys, 2)),
            int&>::value),
        "");
    // STATIC_ASSERT(boost::static_views::detail::SequenceTraitsHaveAt<S>, "");
    STATIC_ASSERT(boost::static_views::StaticSequence<S>,
        "foo[10] does not model the StaticSequence concept.");
    // This one is important!
    STATIC_ASSERT(boost::static_views::Sequence<int const[3]>,
        "const int[3] does not model the Sequence concept.");
    STATIC_ASSERT(boost::static_views::Sequence<int const volatile[3]>,
        "const int[3] does not model the Sequence concept.");
    STATIC_ASSERT(Traits::extent() == 10, "extent() is broken.");
    STATIC_ASSERT(Traits::size(xs) == 10, "size() is broken.");
    STATIC_ASSERT(Traits::at(xs, 2) == 3, "at() is broken.");
    STATIC_ASSERT(Traits::at(xs, 9) == 10, "at() is broken.");
}

auto test_stdarray_traits()
{
    struct foo {
        int unpack;
    };
    using S      = std::array<foo, 10>;
    using Traits = boost::static_views::sequence_traits<S>;
    S xs{{{1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9}, {10}}};

    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<Traits::size_type, unsigned>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<Traits::index_type, int>));
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<Traits::difference_type, int>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<Traits::value_type, foo>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<Traits::reference, foo&>));
    STATIC_ASSERT(boost::static_views::StaticSequence<S>,
        "std::array<foo, 10> does not model the StaticSequence "
        "concept.");
    STATIC_ASSERT(Traits::extent() == 10, "extent() is broken.");
    BOOST_TEST_EQ(Traits::size(xs), 10);
    BOOST_TEST_EQ(Traits::at(xs, 2).unpack, 3);
    BOOST_TEST_EQ(Traits::at(xs, 9).unpack, 10);
}

auto test_stdtuple_traits()
{
    using S      = std::tuple<char, char, char, char, char>;
    using Traits = boost::static_views::sequence_traits<S>;
    S xs = std::make_tuple('h', 'e', 'l', 'l', 'o');

    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<Traits::size_type, unsigned>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<Traits::index_type, int>));
    BOOST_TEST_TRAIT_TRUE(
        (std::is_same<Traits::difference_type, int>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<Traits::value_type, char>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<Traits::reference, char&>));
    STATIC_ASSERT(boost::static_views::StaticSequence<S>,
        "std::tuple<char, char, char, char, char> does not model the "
        "StaticSequence concept.");
    STATIC_ASSERT(Traits::extent() == 5, "extent() is broken.");
    BOOST_TEST_EQ(Traits::size(xs), 5);
    BOOST_TEST_EQ(Traits::at(xs, 2), 'l');
    BOOST_TEST_EQ(Traits::at(xs, 4), 'o');
}

struct A1 {
    auto size() const noexcept -> std::size_t;
    auto operator[](std::ptrdiff_t) noexcept -> char&;
    auto operator[](std::ptrdiff_t) const noexcept
        -> std::pair<int, float> const&;
};

struct A2 {
    // size() is missing a const --> A2 is not a Sequence
    auto size() noexcept -> std::size_t;
    auto operator[](std::ptrdiff_t) noexcept -> char&;
    auto operator[](std::ptrdiff_t) const noexcept
        -> std::pair<int, float> const&;
};

struct A3 {
    auto operator[](std::ptrdiff_t) noexcept -> char&;
    auto operator[](std::ptrdiff_t) const noexcept
        -> std::pair<int, float> const&;
    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return 123;
    }
};

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

template <>
struct sequence_traits<A1>
    : sequence_traits_default<sequence_traits<A1>, A1> {
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using index_type = difference_type;
    using value_type = int;
    using reference = char&;
};

template <>
struct sequence_traits<A2>
    : sequence_traits_default<sequence_traits<A2>, A2> {
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using index_type = difference_type;
    using value_type = int;
    using reference = char&;
};

template <>
struct sequence_traits<A3>
    : sequence_traits_default<sequence_traits<A3>, A3> {
    using size_type = unsigned; // This is unfortunate
    using difference_type = std::ptrdiff_t;
    using index_type = difference_type;
    using value_type = int;
    using reference = char&;
};

BOOST_STATIC_VIEWS_END_NAMESPACE

auto test_traits_detaults()
{
    STATIC_ASSERT(boost::static_views::Sequence<A1>,
        "A1 does not model the Sequence concept.");
    STATIC_ASSERT(!boost::static_views::Sequence<A2>,
        "A2 models the Sequence concept but its size() is "
        "non-const.");
    STATIC_ASSERT(boost::static_views::Sequence<A3>,
        "A3 does not model the Sequence concept.");
}

int main()
{
    test_carray_traits();
    test_stdarray_traits();
    test_stdtuple_traits();
    test_traits_detaults();
    return boost::report_errors();
}

