//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/static_views/raw_view.hpp>

int main()
{
    // Some data
    constexpr int xs[] = {1, 2, 3, 4, 5};

    // Works!
    auto const v1 = boost::static_views::raw_view(xs);

    // Error!
    // GCC: '... {(& xs)}' is not a constant expression
    // Clang: pointer to 'xs' is not a constant expression
    //
    // Indeed, 'xs' is a non-static constexpr variable that's stored
    // nowhere (except for the mind of the compiler).
    /* constexpr auto const v2 = boost::static_views::raw_view(xs); */

    // Solution? declare 'xs' as static
    static constexpr int ys[] = {1, 2, 3, 4, 5};

    auto const           v3 = boost::static_views::raw_view(ys); // OK
    constexpr auto const v4 = boost::static_views::raw_view(ys); // OK

    // Check that the returned type models the View concept
    boost::static_views::concepts::View::check<decltype(v1)>();
}
