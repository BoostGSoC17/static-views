//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_STATIC_VIEWS_SLICE_HPP
#define BOOST_STATIC_VIEWS_SLICE_HPP


#include <boost/config.hpp>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/drop.hpp>
#include <boost/static_views/take.hpp>


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

    template <class DropTake>
    struct slice_impl : DropTake {
 
        /// \brief Converts \p xs to "slice view".

        /// \tparam View
        /// \verbatim embed:rst:leading-slashes
        /// Wrapper around a view, i.e. ``typename View::type`` is a view and
        /// must model the :ref:`view <view-concept>` concept. It is furthermore
        /// assumed that ``View`` is of the form
        /// ``wrapper<drop_impl<wrapper<take_impl<wrapper<V&(&)>&&>>>&&>`` for
        /// some view ``V``.
        /// \endverbatim
        /// \param xs Rvalue reference to a wrapper around a take view of a drop
        /// view of a view.
        /// 
        /// \verbatim embed:rst:leading-slashes
        /// .. note::
        ///   It's pretty impossible to specify ``DropTake`` and ``View``
        ///   template parameters correctly. For this reason a :cpp:var:`slice`
        ///   factory function is provided. Use it instead to construct slice
        ///   views.
        /// \endverbatim
        template <class View>
        BOOST_STATIC_VIEWS_CONSTEXPR slice_impl(View&& xs)
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                std::is_nothrow_constructible<DropTake, View&&>::value)
            : DropTake{ std::forward<View>(xs) }
        {
        }

        /// \brief "Maps" index \p i to the corresponding index in the parent
        /// view.

        /// Let `xs` be of type #slice_impl. The following relation then holds
        ///
        /// `xs[i] = xs.` #parent `()[xs.` #map `(i)]`
        /// \f$ \forall i \in \{0, 1, \dots, \text{xs.size}()-1\}\f$.
        /// 
        /// This function and the #parent() member function are what distinguish
        /// slice view from the "drop view of take view of view". Although slice
        /// is implemented in terms of the latter, this function goes two steps
        /// up and thus makes the rest of the library think that slice is a
        /// single view.
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR auto map(std::size_t const i) const 
        {
            auto const* p = static_cast<DropTake const*>(this);
            return p->parent().map(p->map(i));
        }

        
        /// \brief Returns the "parent" view.

        /// Let `xs_ptr` be of a pointer to some view. The following relation
        /// then holds
        ///
        /// `xs_ptr = &` #slice `(b, e)(*xs_ptr).` #parent `(),`
        ///     \f$\forall \text{b},\text{e} \in \mathbb{N}\f$.
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        BOOST_STATIC_VIEWS_DECLTYPE_AUTO parent() const
        {
            return static_cast<DropTake const*>(this)->parent().parent();
        }
    };

    struct make_slice_impl {
        template <class View>
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        auto operator()(View&& xs, std::size_t const b,
            std::size_t const e) const
        BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
        (
            slice_impl<std::decay_t<decltype(
                drop(b)(take(e)(std::forward<View>(xs).get()))
            )>>{ drop(b)(take(e)(std::forward<View>(xs).get())) }
        )
    };
} // end namespace detail


/// \brief A functor for creating "slice views"

/// \f[
/// \text{slice} : \mathbb{N} \to \mathbb{N} \to \text{View} \to \text{View}
/// \f]
///
/// \verbatim embed:rst:leading-slashes
/// Given a lower bound ``b``, an upper bound ``e`` and a view ``xs``, creates
/// a view of the ``[b, e)`` part of ``xs``. Type of ``xs`` may be anything as
/// long as it models the :ref:`view <view-concept>` concept. The exact type
/// of the returned view is an implementation detail. What's important is that
/// it also models the :ref:`view <view-concept>` concept.
///
/// Calling ``slice(b, e)(xs)`` is almost equivalent to calling
/// ``drop(b)(take(e)(xs))``. The difference is that slice is a single view,
/// and, for example, calling :cpp:func:`parent() 
/// <detail::slice_impl::parent()>` on it will return xs. Calling ``parent()``
/// on the drop view of take view of view will return the take view.
/// 
/// .. note::
///   Haskell notation is used here, i.e. the function is curried and
///   :math:`\text{slice}(b, e) : \text{View} \to \text{View}` models the
///   :ref:`algorithm <algorithm-concept>` concept.
///
/// \endverbatim
BOOST_STATIC_VIEWS_INLINE_ALGO_VARIABLE(detail::make_slice_impl, slice)


BOOST_STATIC_VIEWS_END_NAMESPACE


#endif // BOOST_STATIC_VIEWS_SLICE_HPP
