//          Copyright Tom Westerhout 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <boost/static_views/pipe.hpp>     // for | syntax
#include <boost/static_views/raw_view.hpp> // to get boost::static_views::raw_view
#include <boost/static_views/transform.hpp> // to get boost::static_views::transform

namespace sv = boost::static_views;

int main()
{
    // First of all, we need some view of the data
    static constexpr int  data[] = {1, 2, 3, 4, 5};
    static constexpr auto raw    = sv::raw_view(data);

    // Now, let's create a view of squares of elements of data:
    struct sqr_fn {
        constexpr auto operator()(int const x) const noexcept { return x * x; }
    };
    static constexpr auto sqr = sqr_fn{};
    constexpr auto        v1  = sv::transform(raw, sqr);

    // The docs promised to return a View
    using v1_type = std::remove_const_t<decltype(v1)>;
    static_assert(
        sv::View<v1_type>, "Since when is transform view not a View?");

    // Check that it works:
    for (auto x : v1)
        std::cout << x << '\n';

    // Alternative syntax (currying)
    BOOST_STATIC_VIEWS_UNUSED constexpr auto v2 = sv::transform(sqr)(raw);
    // Or even (piping)
    BOOST_STATIC_VIEWS_UNUSED constexpr auto v3 = raw | sv::transform(sqr);

    // Empty base optimisation:
    static_assert(sizeof(sv::raw_view(data))
                      == sizeof(sv::raw_view(data) | sv::transform(sqr_fn{})),
        "Transform view is wasting memory!");

    // Preserves size and extent
    static_assert(raw.size() == v1.size(), "Size has changed!?");
    static_assert(raw.extent() == v1.extent(), "Extent has changed!?");
}
