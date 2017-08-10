//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/static_views/raw_view.hpp> // to get boost::static_views::raw_view
#include <boost/static_views/take.hpp> // to get boost::static_views::drop

int main()
{
    // First of all, we need a view of the data
    static constexpr int  data[] = {1, 2, 3, 4, 5};
    static constexpr auto raw = boost::static_views::raw_view(data);

    // Now, let's create a view of the first four elements of raw.
    constexpr auto v1 = boost::static_views::take(4)(raw);

    // If you forget the step of creating the raw_view and write
    // constexpr auto v2 = boost::static_views::take(4)(data);
    // ===> Error!
    // "static assertion failed: I'm sorry, but `View` must model the
    // View concept!"

    // Size is changed
    static_assert(v1.size() == 4, "");

    // Though not the extent
    static_assert(raw.extent() == v1.extent(), "");

    // But if you _do_ want the extent to change as well, just pass
    // integral_constant rather than size_t to take:
    constexpr auto v3 = boost::static_views::take(
        std::integral_constant<std::size_t, 4>{})(raw);
    static_assert(v3.extent() == 4, "");

    // You can do all the common stuff like
    //
    // accessing elements
    static_assert(v1[1] == 2, "");
    // copying the view
    constexpr auto v4 = v1;
    // accessing the parent view
    static_assert(&raw == &v4.parent(), "");
    // etc.
}
