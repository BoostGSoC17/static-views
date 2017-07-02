//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


// Try to create drop views of arrays of different type and size.



#include <utility>
#include <boost/config.hpp>
#include <boost/static_views/raw_view.hpp>
#include <boost/static_views/drop.hpp>



template <class T, std::size_t N, std::size_t... Is>
auto create_impl(std::index_sequence<Is...>)
{
    // Notice the `static`.
    static constexpr T    xs[] = { ((void)Is, T{})... };
    static constexpr auto raw1 = boost::static_views::raw_view(xs);
           constexpr auto raw2 = boost::static_views::raw_view(xs);

    BOOST_ATTRIBUTE_UNUSED constexpr
#if BOOST_WORKAROUND(BOOST_CLANG, BOOST_TESTED_AT(BOOST_CLANG))
    // Clangs implementation of std::initializer_list is strange...
    // And not constexpr at all.

    decltype(boost::static_views::drop(
        std::declval<std::size_t>())(raw1)) v1[] 
#else
    auto v1
#endif
        = { ((void)Is, boost::static_views::drop(Is)(raw1))... };


    BOOST_ATTRIBUTE_UNUSED constexpr 
#if BOOST_WORKAROUND(BOOST_CLANG, BOOST_TESTED_AT(BOOST_CLANG))
    decltype(boost::static_views::drop(std::declval<std::size_t>())(
        boost::static_views::raw_view(xs))) v2[]
#else
    auto v2 
#endif
        = { ((void)Is, boost::static_views::drop(Is)(
            boost::static_views::raw_view(xs)))... };

    BOOST_ATTRIBUTE_UNUSED constexpr
#if BOOST_WORKAROUND(BOOST_CLANG, BOOST_TESTED_AT(BOOST_CLANG))
    decltype(boost::static_views::drop(
        std::declval<std::size_t>())(std::declval<decltype(raw2)>())) v3[] 
#else
    auto v3
#endif
        = { ((void)Is, 
            boost::static_views::drop(Is)(decltype(raw2){raw2}))... };
}



template <class T, std::size_t N>
auto create()
{
    return create_impl<T, N>(std::make_index_sequence<N>{});
}


int main(void)
{
    create<int, 5>();
    create<double, 10>();
    create<std::pair<float, float>, 2>();

    return 0;
}
