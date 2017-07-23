//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Test whether drop view's of non-constexpr data are well optimised.

#include <cassert>
#include <utility>
#include <boost/config.hpp>
#include <boost/static_views/drop.hpp>
#include <boost/static_views/raw_view.hpp>

namespace {

template <class R>
BOOST_FORCEINLINE constexpr auto assert_all(R rng)
{
    auto const end = rng.end();
    for (auto i = rng.begin(); i != end; ++i) {
        assert(*i);
    }
}

template <std::size_t... Is, std::size_t... Js>
BOOST_FORCEINLINE constexpr auto create_impl(
    std::index_sequence<Is...>, std::index_sequence<Js...>)
{
    std::size_t xs[] = {Is...};
    auto const  raw  = boost::static_views::raw_view(xs);

    auto results = {(boost::static_views::drop(Js)(
                         boost::static_views::drop(Js)(raw))[0]
                     == 2 * Js)...};
    assert_all(results);

    static_assert(sizeof...(Is) > 1, "");
    boost::static_views::drop(sizeof...(Is) - 1)(raw)[0] = 0;
    assert(xs[sizeof...(Is) - 1] == 0);
}

template <std::size_t N, std::size_t M>
BOOST_FORCEINLINE constexpr auto create()
{
    return create_impl(
        std::make_index_sequence<N>{}, std::make_index_sequence<M>{});
}

} // unnamed namespace

BOOST_SYMBOL_EXPORT BOOST_NOINLINE void test1() { create<5, 3>(); }

int main(void)
{
    test1();

    return 0;
}
