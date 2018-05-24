//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_VIEW_CONCEPT_HPP
#define BOOST_STATIC_VIEWS_VIEW_CONCEPT_HPP

#include "concepts.hpp"

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

#if defined(BOOST_STATIC_VIEWS_CONCEPTS)
template <class T>
concept bool View = MoveConstructible<T>&& HasExtent<T>&& HasSizeWithType<T>&&
    HasIndexOperatorWithType<T>&& HasUnsafeAtWithType<T>;
#else
template <class T>
constexpr bool             View =
    MoveConstructible<T>&& HasExtent<T>&& HasSizeWithType<T>&&
        HasIndexOperatorWithType<T>&& HasUnsafeAtWithType<T>;
#endif

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_VIEW_CONCEPT_HPP
