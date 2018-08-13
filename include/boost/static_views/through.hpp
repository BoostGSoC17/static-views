//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_THROUGH_HPP
#define BOOST_STATIC_VIEWS_THROUGH_HPP

#include "view_base.hpp"
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
template <class Wrapper, class Proxy>
struct through_view_impl
    : public view_adaptor_base<through_view_impl<Wrapper, Proxy>, Wrapper>
    // TODO: I'm not sure if this is safe, i.e. could the user come up with a
    // really tricky combination of views such that view_adaptor_base<...> would
    // end up deriving from Proxy too?
    , private Proxy {

  private:
    using wrapper_type    = Wrapper;
    using proxy_type      = Proxy;
    using proxy_view_type = typename proxy_type::value_type;
    using base = view_adaptor_base<through_view_impl<Wrapper, Proxy>, Wrapper>;

    BOOST_STATIC_VIEWS_CONSTEXPR auto const& proxy() const& noexcept
    {
        return static_cast<Proxy const&>(*this).get();
    }

    BOOST_STATIC_VIEWS_CONSTEXPR auto& proxy() & noexcept
    {
        return static_cast<Proxy&>(*this).get();
    }

    BOOST_STATIC_VIEWS_CONSTEXPR auto proxy()
        && BOOST_STATIC_VIEWS_NOEXCEPT_IF(
               std::is_nothrow_move_constructible<proxy_view_type>::value)
    {
        return static_cast<Proxy&&>(*this).get();
    }

  public:
    using index_type = typename proxy_view_type::index_type;
    using typename base::reference;
    using typename base::size_type;
    using typename base::value_type;

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
    BOOST_STATIC_VIEWS_CONSTEXPR
    through_view_impl(Wrapper&& xs, Proxy&& proxy)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_constructible<base, Wrapper&&>::value&&
                std::is_nothrow_move_constructible<Proxy>::value)
        : base{std::move(xs)}, proxy_type{std::move(proxy)}
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
        return proxy_view_type::extent();
    }

    /// \brief Returns the number of elements viewed.

    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. It just calls ``size()`` on the proxy view. This
    /// function never fails unless a call to ``proxy.size()``
    /// fails.
    /// \endverbatim
    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const noexcept
    {
        return proxy().size();
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
    BOOST_STATIC_VIEWS_CONSTEXPR auto map(index_type const i) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            noexcept(std::declval<proxy_view_type const&>().unsafe_at(
                std::declval<index_type>())))
    {
        BOOST_STATIC_VIEWS_EXPECT(0 <= i && i < static_cast<index_type>(size()),
            "boost::static_views::through_impl::map: Precondition "
            "`0 <= i < size()` is not satisfied.");
        return proxy().unsafe_at(i);
    }
};

struct through_impl {
  private:
    template <class Wrapper, class Proxy>
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(Wrapper xs, Proxy ys) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            through_view_impl<Wrapper, Proxy>{std::move(xs), std::move(ys)});

  public:
    // TODO; Should through_view be checking that operator[] of P actually
    // returns something convertible to V::index_type?
    // clang-format off
    template <class V, class P
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>
         && View<std::remove_cv_t<std::remove_reference_t<P>>>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(V&& xs, P&& proxy) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            noexcept(std::declval<through_impl const&>().call_impl(
                make_wrapper(std::forward<V>(xs)),
                make_wrapper(std::forward<P>(proxy)))))
    {
        return call_impl(make_wrapper(std::forward<V>(xs)),
                make_wrapper(std::forward<P>(proxy)));
    }
};

#if 0
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
#endif
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
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::through_impl, through)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_THROUGH_HPP
