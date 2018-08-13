//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <tuple>

#include <boost/static_views/raw_view.hpp>
#include <boost/static_views/view_concept.hpp>

namespace sv = boost::static_views;

int main()
{
    // Some data which models the sequence concept.
    constexpr int xs[] = {1, 2, 3, 4, 5};
    static_assert(sv::Sequence<decltype(xs)>,
        "C-style arrays model the Sequence concept.");

    // First step is to create a view of it:
    auto const v1 = sv::raw_view(xs);
    static_assert(
        sv::View<decltype(v1)>, "raw_view must model the View concept.");
    // raw_view is the simplest view there is. Is just wraps a reference to a
    // Sequence and provides the member functions required by the View concept.
    // raw_views are extremely lightweight objects which are trivially copyable,
    // movable, and destructible.

    // Trying to make v1 `constexpr` will result in a compile-time error,
    // because non-static constexpr variables are basically "non-existent", i.e.
    // they live only in the mind of the compiler. Thus you can't take its
    // address.

    // An obvious solution is to add static keyword:
    static constexpr int ys[] = {1, 2, 3, 4, 5};
    auto const           v3 = sv::raw_view(ys);
    constexpr auto const v4 = sv::raw_view(ys);

    // You can now use basic view functions like
    // safely accessing elements:
    static_assert(v4[2] == 3, "");
    try {
        v4[8];
    }
    catch (sv::out_of_bound& e) {
        std::cout << "Caught an out_of_bound error: " << e.what() << '\n';
    }
    // determining the number of elements
    static_assert(v4.size() == 5, "");
    // or the extent
    static_assert(decltype(v4)::extent() == 5, "");

    // C-style array is not the only data type that models the Sequence concept.
    // For example, one can also use homogeneous tuples (I know it sounds funny,
    // but still):
    static constexpr auto zs = std::make_tuple(43.5, 89.0, -1.0);
    constexpr auto        v5 = sv::raw_view(zs);
}
