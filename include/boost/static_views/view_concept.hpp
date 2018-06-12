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

template <class V>
constexpr auto why_is_my_argument_not_a_view() -> void
{
    static_assert(!View<V>, BOOST_STATIC_VIEWS_BUG_MESSAGE);

    static_assert(View<V>,
        "Type `V` does not model the View concept, because: (read "
        "on for more)");
    static_assert(MoveConstructible<V>, "`V` is not move-constructible.");
    static_assert(HasExtent<V>,
        "`V` does not have a `constexpr` `static` non-throwing "
        "member function `extent()` returning a value >= 0 or "
        "`boost::static_views::dynamic_extent`.");
    static_assert(HasSizeWithType<V>,
        "`V` does not have a `const` non-throwing member function `size()` "
        "returning a `V::size_type`.");
    static_assert(HasIndexOperatorWithType<V>,
        "`V` does not have an `operator[](V::index_type)` returning a "
        "`V::reference`.");
    static_assert(HasUnsafeAtWithType<V>,
        "`V` does not have a member function `unsafe_at(V::index_type)` "
        "returning a `V::reference`.");
}

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_VIEW_CONCEPT_HPP
