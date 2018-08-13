//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <boost/static_views/static_map.hpp>

// This is the example used in GSoC 2017 project description.

// First, a small utility for comparing C-style (i.e. NUL-terminated) strings at
// compile-time.
struct equal_c {
    template <class Char>
    constexpr auto operator()(Char const* const a, Char const* const b) const
        noexcept -> bool
    {
        std::size_t i = 0;
        while (a[i] != Char{} && a[i] == b[i]) {
            ++i;
        }
        return a[i] == Char{} && b[i] == Char{};
    }
};

void test1()
{
    using value_type = std::pair<int const, char const*>;
    // Here's some data:
    // clang-format off
    static constexpr value_type map_data[] = {
        {5, "apple"},
        {8, "pear"},
        {0, "banana"}
    };
    // clang-format on

    constexpr auto cmap = boost::static_views::make_static_map_impl<10, 1>{}(
        // The view from which to create the map
        boost::static_views::raw_view(map_data),
        // An invocable to access the key part of an element
        &value_type::first,
        // An invocable to access the value part of an element
        &value_type::second);

    static_assert(equal_c{}(cmap[5], "apple"), "");
    static_assert(equal_c{}(cmap[8], "pear"), "");
    static_assert(equal_c{}(cmap[0], "banana"), "");
    static_assert(cmap.find(8) == &map_data[1], "");
    static_assert(cmap.find(10) == nullptr, "");

    // If compiled with optimisations turned on, the following doesn't even
    // appear in the generated assembly.
    if (cmap.find(10) != nullptr) std::terminate();
}

int main()
{
    test1();
    return 0;
}

