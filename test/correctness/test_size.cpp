//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


// Check whether drop view's size() member function works OK.


#include <utility>
#include <boost/config.hpp>
#include <boost/static_views/raw_view.hpp>
#include <boost/static_views/drop.hpp>


template <std::size_t N>
constexpr auto all(bool const (&rng)[N])
{
    for (std::size_t i = 0; i < N; ++i) {
        if (!rng[i]) return false;
    }
    return true;
}


template <class T, std::size_t... Is, std::size_t... Js>
auto create_impl(std::index_sequence<Is...>, std::index_sequence<Js...>)
{
    static constexpr T    xs[] = { ((void)Is, T{})... };
    static constexpr auto raw = boost::static_views::raw_view(xs);
           constexpr auto N   = sizeof...(Is);

    constexpr bool results[] = { (boost::static_views::drop(Js)(raw).size()
        == ((N >= Js) ? (N - Js) : 0))... };

    static_assert(all(results), "drop::size() does not work correctly."); 
}



template <class T, std::size_t N, std::size_t M>
auto create()
{
    return create_impl<T>(std::make_index_sequence<N>{},
        std::make_index_sequence<M>{});
}


int main(void)
{
    create<int, 5, 8>();
    create<double, 10, 20>();
    create<std::pair<float, float>, 2, 3>();

    return 0;
}
