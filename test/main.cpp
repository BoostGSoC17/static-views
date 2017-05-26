#include <iostream>
#include <stdexcept>
#include <utility>
#include <functional>
#include <tuple>
#include <array>

#include "../include/boost/static_map/static_views.hpp"


int main()
{
    constexpr int xs[] = {1, 2, 3, 4, 5};
    static_assert((identity(xs) | identity).size() == 5, "");
    static_assert((xs | identity | identity).at(2) == 3, "");
    static_assert((xs | subset(2, 5) | identity).at(1) == 4, "");
    static_assert((xs | subset(2, 5) | subset(2, 5)).at(0) == 5, "");
    static_assert((xs | filter([](auto&& x) { return x % 2 == 0; })).size() == 2, "");

    constexpr int as[] = {3, -4, 2, -8, -10, 2};
    static_assert((as | sort(std::less<void>{})).at(0) == -10, "");
    static_assert(sort(std::less<void>{}, identity(as)).at(2) == -4, "");
}
