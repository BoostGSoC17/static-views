#include <iostream>
#include <stdexcept>
#include <utility>
#include <functional>
#include <tuple>
#include <array>

#include "../include/boost/static_map/static_views.hpp"


int main()
{
    constexpr static int xs[] = {1, 2, 3, 4, 5};
    constexpr static std::array<int, 6> as = {3, -4, 2, -8, -10, 2};

    // adopting simple sequences
    constexpr static auto ys  = raw_view(xs);
    constexpr static auto bs  = raw_view(as);

    // testing for concepts
    static_assert(is_view<decltype(ys)>::value, "");
    static_assert(!is_view<decltype(xs)>::value, "");
    static_assert(is_pipe<decltype(slice(0, 123))>::value, "");
    static_assert(is_view<decltype(slice(0, 2)(ys))>::value, "");

    // querying for size and elements
    static_assert(ys.capacity() == 5, "");
    static_assert(ys.size() == 5, "");
    static_assert(ys.at(2) == 3, "");
    // compile-time error:
    //     static_assert(ys.at(6) == 3, "");

    // some views
    static_assert((ys | drop(3)).at(0) == 4, "");
    // notice how we can also pass xs (which is __not__ a view)
    // to a pipe
    static_assert((xs | slice(2, 5)).at(1) == 4, "");
    static_assert((xs | slice(2, 5) | slice(1, 3)).at(0) == 4, "");
    static_assert((xs | drop_while([](auto&& x) { return x < 4; })).at(0) 
        == 4, "");
    static_assert((ys | reverse).at(0) == 5, "");
    static_assert((ys | take(2)).size() == 2, "");
    static_assert((ys | take(20)).size() == ys.size(), "");
    // std::experimental::not_fn :D
    static_assert(!not_fn([](auto&& x) { return x > 3; })(4), "");
    static_assert((xs | filter([](auto&& x) { return x % 2 == 0; })).size() 
        == 2, "");

    // alternative syntax, for people who hate pipes
    static_assert(sort(std::less<void>{})(bs).at(0) == -10, "");
    static_assert(sort(std::less<void>{})(slice(1, 6)(raw_view(as))).at(2) == -4, "");

    // also hashed view is supported
    // it's not very useful without any algorithms though
    // 
    // choose some stupid hash function:
    static_assert((as | hashed<20>([](auto&& x) { return 1 << (x > 0 ? x : (-x)); }))(8) == 0);

    // I still have no idea as to why this doesn't work:
    // constexpr auto hashed_as = as | hashed<20>([](auto&& x) { return 1 << (x > 0 ? x : (-x)); });
    // static_assert(hashed_as(8) == 0);
    // while this does:
    constexpr auto reversed_ys = ys | reverse;
}
