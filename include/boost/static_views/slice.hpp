//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_SLICE_HPP
#define BOOST_STATIC_VIEWS_SLICE_HPP

#include <type_traits>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/drop.hpp>
#include <boost/static_views/take.hpp>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class DropTake>
struct slice_impl : DropTake {

    static_assert(
        std::is_same<DropTake, std::decay_t<DropTake>>::value,
        BOOST_STATIC_VIEWS_BUG_MESSAGE);
    static_assert(detail::concepts::is_View<DropTake>(),
        "[INTERNAL] invalid use of drop_impl.");

  private:

    template <class T>
    using call_parent_t = decltype( std::declval<T const&>().parent() );

    template <class T>
    static constexpr auto check_has_parent() noexcept -> bool
    {
        return is_detected<call_parent_t, T>::value;
    }

    template <class T,
        class = std::enable_if_t<check_has_parent<T>()>, class = void>
    static constexpr auto check_parent_returns_view() noexcept -> bool
    {
        return concepts::is_View<
            std::decay_t<detected_t<call_parent_t, T>>>();
    }

    template <class T,
        class = std::enable_if_t<!check_has_parent<T>()>>
    static constexpr auto check_parent_returns_view() noexcept -> bool
    {
        return false;
    }

    template <class T>
    static constexpr auto check_parent() noexcept -> bool
    {
        return utils::all(
            check_has_parent<T>(),
            check_parent_returns_view<T>());
    }

    template <class T>
    static constexpr auto assert_parent() noexcept -> bool
    {
        constexpr bool x = check_parent<T>();
        static_assert(x,
            "[INTERNAL] `T` is required to have a `parent()` member "
            "function with return type modeling the View concept.");
        return x;
    }

  public:
    /// \brief Converts \p xs to "slice view".

    /// \tparam View
    /// \verbatim embed:rst:leading-slashes
    /// Wrapper around a view, i.e. ``typename View::type`` is a view
    /// and
    /// must model the :ref:`view <view-concept>` concept. It is
    /// furthermore
    /// assumed that ``View`` is of the form
    /// ``wrapper<drop_impl<wrapper<take_impl<wrapper<V&(&)>&&>>>&&>``
    /// for
    /// some view ``V``.
    /// \endverbatim
    /// \param xs Rvalue reference to a wrapper around a take view of
    /// a drop view of a view.
    ///
    /// \verbatim embed:rst:leading-slashes
    /// .. note::
    ///   It's pretty impossible to specify ``DropTake`` and ``View``
    ///   template parameters correctly. For this reason a
    ///   :cpp:var:`slice`
    ///   factory function is provided. Use it instead to construct
    ///   slice
    ///   views.
    /// \endverbatim
    template <class View,
        // avoid clashes with copy/move constructors
        class = std::enable_if_t<!std::is_same<std::decay_t<View>,
            slice_impl<DropTake>>::value>>
    explicit BOOST_STATIC_VIEWS_CONSTEXPR slice_impl(View&& xs)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_constructible<DropTake, View&&>::value)
        : DropTake{std::forward<View>(xs)}
    {
        assert_parent<DropTake>();
    }

    /// \brief "Maps" index \p i to the corresponding index in the
    /// parent
    /// view.

    /// Let `xs` be of type #slice_impl. The following relation then
    /// holds
    ///
    /// `xs[i] = xs.` #parent `()[xs.` #map `(i)]`
    /// \f$ \forall i \in \{0, 1, \dots, \text{xs.size}()-1\}\f$.
    ///
    /// This function and the #parent() member function are what
    /// distinguish
    /// slice view from the "drop view of take view of view". Although
    /// slice
    /// is implemented in terms of the latter, this function goes two
    /// steps
    /// up and thus makes the rest of the library think that slice is
    /// a single view.
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto map(std::size_t const i) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<DropTake const&>().parent().map(
                std::declval<DropTake const&>().map(
                    std::declval<std::size_t>()))))
    {
        static_assert(
            noexcept(std::declval<DropTake const&>().parent()),
            "[INTERNAL] Why is parent() not noexcept?");
        auto const* p = static_cast<DropTake const*>(this);
        return p->parent().map(p->map(i));
    }

    /// \brief Returns the "parent" view.

    /// Let `xs_ptr` be of a pointer to some view. The following
    /// relation
    /// then holds
    ///
    /// `xs_ptr = &` #slice `(b, e)(*xs_ptr).` #parent `(),`
    ///     \f$\forall \text{b},\text{e} \in \mathbb{N}\f$.
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO parent() const noexcept
    {
        static_assert(
            noexcept(std::declval<DropTake const&>().parent()),
            "[INTERNAL] Why is parent() not noexcept?");
        using parent_type = std::decay_t<decltype(
            std::declval<DropTake const&>().parent())>;
        assert_parent<parent_type>();
        static_assert(noexcept(std::declval<parent_type>().parent()),
            "[INTERNAL] Why is parent() not noexcept?");
        return static_cast<DropTake const*>(this)->parent().parent();
    }
};

struct make_slice_impl {

    // clang-format off
    template <class View>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs, std::size_t const b, std::size_t const e) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(slice_impl<std::decay_t<decltype(
                    drop(b)(take(e)(std::forward<View>(xs).get())))>>{
                drop(b)(take(e)(std::forward<View>(xs).get()))}))
    {
        return slice_impl<std::decay_t<decltype(
            drop(b)(take(e)(std::forward<View>(xs).get())))>>{
            drop(b)(take(e)(std::forward<View>(xs).get()))};
    }
    // clang-format on
};
} // end namespace detail

/// \brief A functor for creating "slice views"

/// \f[
/// \text{slice} : \mathbb{N} \to \mathbb{N} \to \text{View} \to
/// \text{View} \f]
///
/// \verbatim embed:rst:leading-slashes
/// Given a lower bound ``b``, an upper bound ``e`` and a view ``xs``,
/// creates
/// a view of the ``[b, e)`` part of ``xs``. Type of ``xs`` may be
/// anything as long as it models the :ref:`view <view-concept>`
/// concept. The exact type of the returned view is an implementation
/// detail. What's important is that it also models the :ref:`view
/// <view-concept>` concept.
///
/// Calling ``slice(b, e)(xs)`` is almost equivalent to calling
/// ``drop(b)(take(e)(xs))``. The difference is that slice is a single
/// view, and, for example, calling :cpp:func:`parent()
/// <detail::slice_impl::parent()>` on it will return xs. Calling
/// ``parent()``
/// on the drop view of take view of view will return the take view.
///
/// .. note::
///   Haskell notation is used here, i.e. the function is curried and
///   :math:`\text{slice}(b, e) : \text{View} \to \text{View}` models
///   the :ref:`algorithm <algorithm-concept>` concept.
///
/// \endverbatim
BOOST_STATIC_VIEWS_INLINE_ALGO_VARIABLE(
    detail::make_slice_impl, slice)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_SLICE_HPP
