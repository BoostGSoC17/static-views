//          Copyright Tom Westerhout 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_SEQUENCE_CONCEPT_HPP
#define BOOST_STATIC_VIEWS_SEQUENCE_CONCEPT_HPP

#include "concepts.hpp"

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

template <class T>
struct sequence_traits;

namespace detail {

template <class T>
using has_sequence_traits_specialisation_t =
    decltype(sequence_traits<T>{});

} // namespace detail

#if defined(BOOST_STATIC_VIEWS_CONCEPTS)
template <class T>
concept bool Sequence =
    is_detected<detail::has_sequence_traits_specialisation_t,
        T>::value;
#else
template <class T>
constexpr bool Sequence =
    is_detected<detail::has_sequence_traits_specialisation_t,
        T>::value;
#endif

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_SEQUENCE_CONCEPT_HPP
