//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../example/static_map.hpp"
#include <cassert>
#include <iostream>
#include <utility>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/raw_view.hpp>
#include <boost/static_views/hashed.hpp>

struct equal_c {

    template <class T>
    using equal_t = decltype( std::declval<T>() == std::declval<T>() );

    template <class T>
    using not_equal_t = decltype( std::declval<T>() != std::declval<T>() );

    template <class Char>
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(
        Char const* const a, Char const* const b) const noexcept
    {
        using boost::static_views::detail::is_detected;
        static_assert(
            std::is_nothrow_default_constructible<Char>::value,
            "`equal_c` requires `Char` to be nothrow "
            "default-constructible.");
        static_assert(is_detected<equal_t, Char>::value
                          && is_detected<not_equal_t, Char>::value,
            "`equal_c` requires `Char` to define `operator==` and "
            "`operator!=`.");
        static_assert(
            noexcept(std::declval<Char>() == std::declval<Char>())
                && noexcept(
                       std::declval<Char>() != std::declval<Char>()),
            "`equal_c` requires `Char`'s `operator==` and "
            "`operator!=` to be noexcept.");

        std::size_t i = 0;
        while (a[i] != Char{} && a[i] == b[i]) {
            ++i;
        }
        return a[i] == Char{} && b[i] == Char{};
    }
};

enum class weekday {
    sunday,
    monday,
    tuesday,
    wednesday,
    thursday,
    friday,
    saturday
};

#if defined(BOOST_STATIC_VIEWS_HAVE_STRING_VIEW)
using boost::static_views::string_view;
#define STRING_VIEW(str)                                             \
    string_view { str, sizeof(str) - 1 }

struct equal_string_view {
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(string_view const a,
        string_view const b) const noexcept -> bool
    {
        return equal_c{}(a.data(), b.data());
    }
};
#endif

void test1()
{
    {
        using value_type = std::pair<int const, char const*>;
        static constexpr value_type map_data[] = {
            {5, "apple"}, {8, "pear"}, {0, "banana"}};

#if defined(__cpp_constexpr) && __cpp_constexpr >= 201603
// Use C++17 lambdas
        constexpr auto cmap =
            boost::static_views::static_map::make_static_map<10, 1>(
                boost::static_views::raw_view(map_data),
                [](auto&& x) -> decltype(auto) { return x.first; },
                [](auto&& x) -> decltype(auto) { return x.second; });
#else
        struct get_key {
            constexpr decltype(auto) operator()(
                std::pair<int const, char const*> const& x) const noexcept
            {
                return x.first;
            }
        };
        struct get_mapped {
            constexpr decltype(auto) operator()(
                std::pair<int const, char const*> const& x) const noexcept
            {
                return x.second;
            }
        };
        constexpr auto cmap =
            boost::static_views::static_map::make_static_map<10, 1>(
                boost::static_views::raw_view(map_data), get_key{},
                get_mapped{});
#endif

        static_assert(equal_c{}(cmap[5], "apple"), "");
        static_assert(equal_c{}(cmap[8], "pear"), "");
        static_assert(equal_c{}(cmap[0], "banana"), "");
        static_assert(cmap.find(8) == &map_data[1], "");
        static_assert(cmap.find(10) == nullptr, "");

        if (cmap.find(10) != nullptr) std::terminate();
    }

    {
#if defined(BOOST_STATIC_VIEWS_HAVE_STRING_VIEW)
        // Working with string_view
        static constexpr std::tuple<string_view, weekday>
            string_to_weekday[]{
                {STRING_VIEW("sunday"), weekday::sunday},
                {STRING_VIEW("monday"), weekday::monday},
                {STRING_VIEW("tuesday"), weekday::tuesday},
                {STRING_VIEW("wednesday"), weekday::wednesday},
                {STRING_VIEW("thursday"), weekday::thursday},
                {STRING_VIEW("friday"), weekday::friday},
                {STRING_VIEW("saturday"), weekday::saturday}};

#if defined(__cpp_constexpr) && __cpp_constexpr >= 201603
        // clang-format off
        constexpr auto to_weekday =
            boost::static_views::static_map::make_static_map<10, 2>(
                boost::static_views::raw_view(string_to_weekday),
                [](auto&& x) -> decltype(auto) { return std::get<0>(x); },
                [](auto&& x) -> decltype(auto) { return std::get<1>(x); },
                equal_string_view{});
        // clang-format on
#else
#error "Why do you have C++17 string_view, but no constexpr lambdas?"
#endif
        // clang-format off
        static_assert(to_weekday[STRING_VIEW("sunday")] == weekday::sunday, "");
        static_assert(to_weekday[STRING_VIEW("monday")] == weekday::monday, "");
        static_assert(to_weekday[STRING_VIEW("tuesday")] == weekday::tuesday, "");
        static_assert(to_weekday[STRING_VIEW("wednesday")] == weekday::wednesday, "");
        static_assert(to_weekday[STRING_VIEW("thursday")] == weekday::thursday, "");
        static_assert(to_weekday[STRING_VIEW("friday")] == weekday::friday, "");
        static_assert(to_weekday[STRING_VIEW("saturday")] == weekday::saturday, "");
        // clang-format on
#endif
    }
}

void test2()
{
    // from Niall's ntkernel-error-category
    struct field {
        int ntstatus;
        int win32;
        int posix;
        char const* message;
    };

    static constexpr field error_codes[] = {
        {static_cast<int>(0x80000001), static_cast<int>(0x0), 0,
            "{EXCEPTION}\nGuard Page Exception\nA page of memory "
            "that marks the end of a data structure, such as a stack "
            "or an array, has been accessed."},
        {static_cast<int>(0x80000002), static_cast<int>(0x3e6),
            EACCES,
            "{EXCEPTION}\nAlignment Fault\nA datatype misalignment "
            "was detected in a load or store instruction."},
        {static_cast<int>(0x80000003), static_cast<int>(0x0), 0,
            "{EXCEPTION}\nBreakpoint\nA breakpoint has been "
            "reached."}};

    // Notice the use of pointers to member data!
    constexpr auto cmap = boost::static_views::static_map::make_static_map<5, 3>(
        /* error_codes,*/ // produces a static_assert-failure
        boost::static_views::raw_view(error_codes),
        &field::posix,
        &field::ntstatus);

    if (cmap[0] != static_cast<int>(0x80000001)) std::terminate();
    static_assert(cmap[EACCES] == static_cast<int>(0x80000002), "");

}

void test3()
{
    struct field {
        char const* code;
        char const* area;
        double      latitude;
        double      longitude;
    };

#if defined(__cpp_constexpr) && __cpp_constexpr >= 201603
// Use C++17 lambdas

    static constexpr field table[] = {
#include "../../example/phone_codes.txt"
    };


    constexpr std::size_t table_size = sizeof(table) / sizeof(table[0]);
    constexpr auto        phone_codes =
        boost::static_views::static_map::make_static_map<table_size,
            5>(boost::static_views::raw_view(table), //
            &field::code, &field::area, equal_c{});

    if (!equal_c{}(phone_codes["01564"], "Lapworth")) std::terminate();
    if (!equal_c{}(phone_codes["01706"], "Rochdale")) std::terminate();
    if (!equal_c{}(phone_codes["028 82"], "Omagh")) std::terminate();

#else
#warning "C++17 only"
#endif
}

int main(void)
{
    test1();
    test2();
    test3();
    return 0;
}
