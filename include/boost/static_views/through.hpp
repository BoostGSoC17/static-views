//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_THROUGH_HPP
#define BOOST_STATIC_VIEWS_THROUGH_HPP

#include "algorithm_base.hpp"
#include "detail/config.hpp"
#include "view_base.hpp"
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
template <class View, class Proxy>
struct through_impl
    : view_adaptor_base<through_impl<View, Proxy>, View> {

    /// \brief Constructs a view of \p xs through \p proxy.

    /// \tparam View
    /// \verbatim embed:rst:leading-slashes
    /// Wrapper around a view, i.e. ``typename View::type`` is a view
    /// and
    /// must model the :ref:`view <view-concept>` concept.
    /// \endverbatim
    /// \tparam Proxy
    /// \verbatim embed:rst:leading-slashes
    /// Just like ``View``, a wrapper around a view, i.e.
    /// ``typename Proxy::type`` must be a view. Furthermore, viewed
    /// elements must be of type ``std::size_t``.
    /// \endverbatim
    /// \param xs    Rvalue reference to a wrapper around a view.
    /// \param proxy Rvalue reference to a wrapper around a view. It
    /// is
    ///              used as a proxy to access elements of \p xs, i.e.
    ///              if `ys = through_impl{xs, proxy}`, then
    ///              \f[
    ///                 \text{ys}[i] = \text{xs.get}()\!\left[
    ///                     \text{proxy.get}()[i]\right]\;,
    ///                 \forall i \in \{0,1,\dots,\text{xs.size}() -
    ///                 1\}.
    ///              \f]
    ///
    /// \verbatim embed:rst:leading-slashes
    /// .. note::
    ///   It's annoying to have to specify the ``View`` and ``Proxy``
    ///   template parameters all the time. For this reason a
    ///   :cpp:var:`through` factory function is provided. Use it
    ///   instead
    ///   to construct views through other views.
    /// \endverbatim
    explicit BOOST_STATIC_VIEWS_CONSTEXPR through_impl(
        View&& xs, Proxy&& proxy)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_constructible<
                typename through_impl::view_adaptor_base_type,
                View&&>::value,
            std::is_nothrow_move_constructible<Proxy>::value))
        : through_impl::view_adaptor_base_type{std::move(xs)}
        , _proxy{std::move(proxy)}
    {
    }

    /// \brief Returns the capacity of this view.

    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. It just calls ``capacity()`` on the proxy view. This
    /// function never fails.
    /// \endverbatim
    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return Proxy::type::extent();
    }

    /// \brief Returns the number of elements viewed.

    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. It just calls ``size()`` on the proxy view. This
    /// function never fails unless a call to ``proxy.size()``
    /// fails.
    /// \endverbatim
    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<Proxy const&>().get().size()))
    {
        return _proxy.get().size();
    }

    /// \brief "Maps" index \p i to the corresponding index in the
    /// parent
    /// view. Actually, it just calls ``operator[]`` on the proxy
    /// view.

    /// Let `ys` be of type #through_impl, i.e. `ys = ` #through
    /// `(proxy)(xs)`. The following relation then holds
    ///
    /// `ys.` #map `(i) = proxy[i],`
    ///     \f$\forall i \in \{0, 1, \dots,
    ///     \text{proxy.size}()-1\}.\f$
    BOOST_STATIC_VIEWS_CONSTEXPR auto map(std::size_t const i) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(concepts::View::unsafe_at(
                std::declval<Proxy const&>().get(),
                std::declval<std::size_t>()))) -> std::size_t
    {
        return concepts::View::unsafe_at(_proxy.get(), i);
    }

  private:
    // friend struct
    // BOOST_STATIC_VIEWS_NAMESPACE::view_adaptor_core_access;
    Proxy _proxy;
};

struct make_through_impl {
    // clang-format off
    template <class View, class Proxy>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs, Proxy&& proxy) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        through_impl<std::decay_t<View>,
            decltype(make_wrapper(std::forward<Proxy>(proxy).get()))>{
                std::forward<View>(xs),
                make_wrapper(std::forward<Proxy>(proxy).get())}
    );
    // clang-format on
};

struct make_through_algo_impl {
    // clang-format off
    template <class Proxy>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Proxy&& proxy) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_through_impl{},
            make_wrapper(std::forward<Proxy>(proxy)))
        /*
        algorithm_impl<make_through_impl,
            decltype(make_wrapper(std::forward<Proxy>(proxy)))>{
            make_through_impl{}, make_wrapper(std::forward<Proxy>(proxy))}*/
    );
    // clang-format on
};

} // end namespace detail

/// \brief A functor for creating "through views"

/// \f[
/// \text{through} : \text{View} \to \text{View} \to \text{View}
/// \f]
///
/// \verbatim embed:rst:leading-slashes
/// Given a view ``proxy`` and a view ``xs``, creates a view of ``xs``
/// through
/// ``proxy``. Type of ``xs`` may be anything as long as it models the
/// :ref:`view <view-concept>` concept. ``proxy`` must be a view of
/// elements of type ``std::size_t``. The exact type of the returned
/// view is an implementation detail. What's important is that it also
/// models the :ref:`view <view-concept>` concept.
///
/// .. note::
///   Haskell notation is used here, i.e. the function is curried and
///   :math:`\text{through}(\text{proxy}) : \text{View} \to
///   \text{View}` models the :ref:`algorithm <algorithm-concept>`
///   concept.
///
/// \endverbatim
BOOST_STATIC_VIEWS_INLINE_VARIABLE(
    detail::make_through_algo_impl, through)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_THROUGH_HPP
