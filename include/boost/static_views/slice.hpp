//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_SLICE_HPP
#define BOOST_STATIC_VIEWS_SLICE_HPP

#include "drop.hpp"
#include "take.hpp"
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class DropTake
    BOOST_STATIC_VIEWS_REQUIRES(View<DropTake>)
struct slice_view_impl : DropTake {

  private:
    using self_type = slice_view_impl;
    using base = DropTake;

  public:
    using typename base::index_type;
    using typename base::reference;
    using typename base::size_type;
    using typename base::value_type;

    /// \brief Converts \p xs to "slice view".

    /// \tparam View
    /// \verbatim embed:rst:leading-slashes
    /// Wrapper around a *view* (i.e. ``typename View::type`` must
    /// model the :ref:`view <view-concept>` concept). It is
    /// furthermore assumed that ``View`` is of the form
    /// ``wrapper<drop_impl<wrapper<take_impl<wrapper<V>&&>>>&&>``,
    /// where ``V`` is either lvalue or rvalue reference to some view.
    /// \endverbatim
    /// \param xs **Rvalue** reference to a wrapper around a take view
    /// of a drop view of a view.
    ///
    /// \verbatim embed:rst:leading-slashes
    /// .. note::
    ///   It's pretty much impossible to specify ``DropTake`` and
    ///   ``View`` template parameters correctly. For this reason
    ///   :cpp:var:`slice` factory function is provided. Use it
    ///   instead to construct slice views.
    /// \endverbatim
    BOOST_STATIC_VIEWS_CONSTEXPR slice_view_impl(DropTake&& xs)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<DropTake>::value)
        : DropTake{std::move(xs)}
    {
    }

    slice_view_impl(slice_view_impl const&) = default;
    slice_view_impl(slice_view_impl&&)      = default;
    slice_view_impl& operator=(slice_view_impl const&) = default;
    slice_view_impl& operator=(slice_view_impl&&) = default;

#if 0
    /// \brief "Maps" index \p i to the corresponding index in the
    /// parent view.

    /// Let `xs` be of type #slice_impl. The following relation then
    /// holds
    ///
    /// `xs[i] = xs.` #parent `()[xs.` #map `(i)]`
    /// \f$ \forall i \in \{0, 1, \dots, \text{xs.size}()-1\}\f$.
    ///
    /// This function and the #parent() member function are what
    /// distinguish slice view from the "drop view of take view of
    /// view". Although slice is implemented in terms of the latter,
    /// this function goes two steps up and thus makes the rest of the
    /// library think that slice is a single view.
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto map(index_type const i) const
        noexcept
    {
        static_assert(
            noexcept(std::declval<DropTake const&>().parent()),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        static_assert(noexcept(std::declval<DropTake const&>().map(
                          std::declval<std::size_t>())),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        static_assert(
            noexcept(std::declval<DropTake const&>().parent().map(
                std::declval<std::size_t>())),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        auto const* p = static_cast<DropTake const*>(this);
        return p->parent().map(p->map(i));
    }

    /// \brief Returns the "parent" view.

    /// Let `xs_ptr` be a pointer to some view. The following
    /// relation then holds
    ///
    /// `xs_ptr = &` #slice `(b, e)(*xs_ptr).` #parent `(),`
    ///     \f$\forall \text{b},\text{e} \in \mathbb{N}\f$.
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO parent() const noexcept
    {
        static_assert(
            noexcept(
                std::declval<DropTake const&>().parent().parent()),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return static_cast<DropTake const*>(this)->parent().parent();
    }
#endif
};

struct slice_impl {

  private:
    template <class T>
    using index_type_t =
        typename std::remove_cv_t<std::remove_reference_t<T>>::index_type;

    template <class DropTake>
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(DropTake xs) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            slice_view_impl<DropTake>{std::move(xs)});

  public:
    // clang-format off
    template <class V
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(V&& xs, index_type_t<V> const b, index_type_t<V> const e)
        // clang-format on
        const BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            noexcept(std::declval<slice_impl const&>().call_impl(
                drop_exactly(take_exactly(std::forward<V>(xs), e), b))))
    {
        return call_impl(drop_exactly(take_exactly(std::forward<V>(xs), e), b));
    }
};

#if 0
struct make_slice_algo_impl {
    // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::size_t const b, std::size_t const e) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_slice_impl{}, std::size_t{b}, std::size_t{e})
        /*algorithm_impl<make_slice_impl, std::size_t, std::size_t>(
            make_slice_impl{}, b, e)*/
    );
    // clang-format on
};
#endif

} // end namespace detail

/// \verbatim embed:rst:leading-slashes
/// :math:`\mathtt{slice} : \mathbb{N} \times \mathbb{N} \to
/// \text{View} \to \text{View}` is a functor that let's you create
/// slices of views. Given a index :math:`b \in \mathbb{N}`, an index
/// :math:`e \in \mathbb{N}` and a :ref:`view <view-concept>`
/// :math:`xs`, creates a view of elements of :math:`xs` with indices
/// :math:`i \in \mathbb{N} \cap [b, e)`. \endverbatim
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::slice_impl, slice)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_SLICE_HPP
