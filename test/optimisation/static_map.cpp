//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../example/static_map.hpp"
#include <cassert>
#include <iostream>
#include <utility>
#include <boost/config.hpp>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/raw_view.hpp>

struct equal_c {
    template <class _Char>
    constexpr auto operator()(
        _Char const* const a, _Char const* const b) const
        noexcept(
            noexcept(std::declval<_Char>() == std::declval<_Char>())
            && noexcept(
                   std::declval<_Char>() != std::declval<_Char>()))
    {
        std::size_t i = 0;
        while (a[i] != _Char{} && a[i] == b[i]) {
            ++i;
        }
        return a[i] == _Char{} && b[i] == _Char{};
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

using std::experimental::string_view;
#define STRING_VIEW(str)                                             \
    std::experimental::string_view { str, sizeof(str) - 1 }

struct equal_string_view {
    constexpr auto operator()(string_view const& a,
        string_view const& b) const noexcept -> bool
    {
        return equal_c{}(a.data(), b.data());
    }
};

void test1()
{
    {
        static constexpr std::pair<int const, const char*>
                       map_data[] = {{5, "apple"}, {8, "pear"}, {0, "banana"}};
        constexpr auto cmap = make_static_map<10, 1>(
            boost::static_views::raw_view(map_data));

        static_assert(equal_c{}(cmap[5], "apple"), "");
        static_assert(equal_c{}(cmap[8], "pear"), "");
        static_assert(equal_c{}(cmap[0], "banana"), "");
        static_assert(cmap.find(8) == &map_data[1], "");
        static_assert(cmap.find(10) == nullptr, "");

        if (cmap.find(10) != nullptr) std::terminate();
    }

    {
        // Working with string_view
        static constexpr std::pair<
            const std::experimental::string_view, weekday>
            string_to_weekday[]{
                {STRING_VIEW("sunday"), weekday::sunday},
                {STRING_VIEW("monday"), weekday::monday},
                {STRING_VIEW("tuesday"), weekday::tuesday},
                {STRING_VIEW("wednesday"), weekday::wednesday},
                {STRING_VIEW("thursday"), weekday::thursday},
                {STRING_VIEW("friday"), weekday::friday},
                {STRING_VIEW("saturday"), weekday::saturday}};
        constexpr auto to_weekday = make_static_map<10, 2>(
            boost::static_views::raw_view(string_to_weekday),
            equal_string_view{});

        static_assert(
            to_weekday[STRING_VIEW("sunday")] == weekday::sunday, "");
        static_assert(
            to_weekday[STRING_VIEW("monday")] == weekday::monday, "");
        static_assert(
            to_weekday[STRING_VIEW("tuesday")] == weekday::tuesday,
            "");
        static_assert(to_weekday[STRING_VIEW("wednesday")]
                          == weekday::wednesday,
            "");
        static_assert(
            to_weekday[STRING_VIEW("thursday")] == weekday::thursday,
            "");
        static_assert(
            to_weekday[STRING_VIEW("friday")] == weekday::friday, "");
        static_assert(
            to_weekday[STRING_VIEW("saturday")] == weekday::saturday,
            "");
    }
}

void test2()
{
    static constexpr std::pair<int const, const char*> map_data[] = {
        {5, "apple"}, {8, "pear"}, {0, "banana"}};
    auto cmap = make_static_map<10, 1>(
        boost::static_views::raw_view(map_data));

    if (!equal_c{}(cmap[5], "apple")) std::terminate();
    if (!equal_c{}(cmap[8], "pear")) std::terminate();
    if (!equal_c{}(cmap[0], "banana")) std::terminate();
    if (cmap.find(8) != &map_data[1]) std::terminate();
    if (cmap.find(10) != nullptr) std::terminate();
}

int main(void)
{
    test1();
    test2();
    return 0;
}
