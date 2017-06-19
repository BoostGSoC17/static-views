//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Implementation of boost::static_views::sequence_traits.

#ifndef BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP
#define BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP

#include <array>
#include <boost/static_views/detail/config.hpp>


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE


template <class Sequence> struct sequence_traits;


/// \cond
template <class T, std::size_t N>
struct sequence_traits<T[N]> {
    using type = T[N];

    static constexpr auto size() noexcept 
    { return N; }

    template <class S>
    static constexpr decltype(auto) at(S&& xs, std::size_t i) noexcept 
    { return std::forward<S>(xs)[i]; }
};
/// \endcond


/// \cond
template <class T, std::size_t N>
struct sequence_traits<std::array<T, N>> {
    using type = std::array<T, N>;

    static constexpr auto size() noexcept 
    { return N; }

    template <class S>
    static constexpr decltype(auto) at(S&& xs, std::size_t i) noexcept 
    { return std::forward<S>(xs)[i]; }
};
/// \endcond


BOOST_STATIC_VIEWS_END_NAMESPACE


#endif // BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP
