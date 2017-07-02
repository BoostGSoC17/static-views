//          copyright tom westerhout 2017.
// distributed under the boost software license, version 1.0.
//    (see accompanying file license_1_0.txt or copy at
//          http://www.boost.org/license_1_0.txt)

#include <cassert>
#include <utility>

#include <boost/static_views/raw_view.hpp>

template <class T>
constexpr decltype(auto) get_first(T&& x)
{
    auto const v = boost::static_views::raw_view(x);
    return v[1];
}


int main(void)
{
    constexpr int as[5] = {1, 2, 3, 4, 5};

    // constexpr auto v = boost::static_views::raw_view(as);
    // static_assert(v[1] == 2, "");
    static_assert(boost::static_views::raw_view(as)[1] == 2, "");
    static_assert(get_first(as) == 2, "");
    
    return 0;
}

