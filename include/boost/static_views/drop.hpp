//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/drop.hpp
///
/// \brief Implementation of #drop.

#ifndef BOOST_STATIC_VIEWS_DROP_HPP
#define BOOST_STATIC_VIEWS_DROP_HPP

#include "view_base.hpp"

#include <algorithm>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class Wrapper, std::ptrdiff_t Extent>
struct drop_view_impl
    : view_adaptor_base<drop_view_impl<Wrapper, Extent>, Wrapper> {

  private:
    using wrapper_type = Wrapper;
    using base = view_adaptor_base<drop_view_impl<Wrapper, Extent>, Wrapper>;

  public:
    using typename base::index_type;
    using typename base::reference;
    using typename base::size_type;
    using typename base::value_type;

    using base::parent;

    /// \brief Constructs a view of \p xs with the first \p b elements
    /// dropped.

    /// \param xs **Rvalue** reference to a wrapper around a view.
    /// \param b  Number of elements to drop. \p b must not exceed the size of
    ///           \p xs.
    BOOST_STATIC_VIEWS_CONSTEXPR drop_view_impl(
        Wrapper&& xs, index_type const b)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            std::is_nothrow_constructible<typename drop_impl::base,
                Wrapper&&>::value)
        : base{std::move(xs)}, _b{b}
    {
        BOOST_STATIC_VIEWS_EXPECT(
            0 <= b && b <= static_cast<index_type>(parent().size()),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
    }

    drop_view_impl(drop_view_impl const&) = default;
    drop_view_impl(drop_view_impl&&)      = default;
    drop_view_impl& operator=(drop_view_impl const&) = default;
    drop_view_impl& operator=(drop_view_impl&&) = default;

    /// \brief Returns the number of elements viewed.
    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const noexcept -> size_type
    {
        static_assert(
            noexcept(parent().size()), BOOST_STATIC_VIEWS_BUG_MESSAGE);
        // Per construction, 0 <= _b <= parent().size(), so this is safe
        return parent().size() - static_cast<size_type>(_b);
    }

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return dynamic_extent;
    }

    /// \brief "Maps" index \p i to the corresponding index in the
    /// parent view.

    /// For \f$ i \geq \text{xs.size}()\f$ behavior of this function
    /// is undefined
    BOOST_STATIC_VIEWS_CONSTEXPR auto map(index_type const i) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    {
        BOOST_STATIC_VIEWS_EXPECT(0 <= i && i < static_cast<index_type>(size()),
            "boost::static_views::drop_impl::map: Precondition "
            "`0 <= i < size()` is not satisfied.");
        return _b + i;
    }

  private:
    index_type _b;
};

struct drop_exactly_impl {
  private:
    template <class Wrapper, class IndexType>
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(Wrapper xs, IndexType b) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            drop_view_impl<Wrapper, dynamic_extent>{std::move(xs), b});

  public:
    // clang-format off
    template <class V
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(V&& xs,
        typename std::remove_cv_t<
                 std::remove_reference_t<V>>::index_type const b) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            noexcept(std::declval<drop_exactly_impl const&>().call_impl(
                make_wrapper(std::forward<V>(xs), b))))
    {
        using index_type =
            typename std::remove_cv_t<std::remove_reference_t<V>>::index_type;
        BOOST_STATIC_VIEWS_EXPECT(
            0 <= b && b <= static_cast<index_type>(xs.size()),
            "boost::static_views::drop_exactly(xs, b): Precondition "
            "`0 <= b <= xs.size()` is not satisfied.");
        return call_impl(make_wrapper(std::forward<V>(xs)), b);
    }
};

#if 0
struct make_drop_exactly_algo_impl {
    // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::size_t const n) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_drop_exactly_impl{}, std::size_t{n})
    );
    // clang-format on

    // clang-format off
    template <std::size_t N>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::integral_constant<std::size_t, N> /*unused*/) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_drop_exactly_impl{},
            std::integral_constant<std::size_t, N>{})
    );
    // clang-format on
};

struct make_drop_impl {
    // clang-format off
    template <class Wrapper>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Wrapper&& xs, std::size_t const b) const
    BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN
    (
        drop_impl<std::decay_t<Wrapper>,
            Wrapper::type::extent()>{std::forward<Wrapper>(xs), 
            std::min(xs.get().size(), b)}
    );
    // clang-format on

    // clang-format off
    template <class Wrapper, std::size_t N>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Wrapper&& xs,
        std::integral_constant<std::size_t, N> /*unused*/) const
    BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN
    (
        drop_impl<std::decay_t<Wrapper>,
            ((Wrapper::type::extent() > N)
                    ? (Wrapper::type::extent() - N)
                    : 0)>{std::forward<Wrapper>(xs), std::min(xs.get().size(), N)}
    );
    // clang-format on
};

struct make_drop_algo_impl {
    // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::size_t const n) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_drop_impl{}, std::size_t{n})
    );
    // clang-format on

    // clang-format off
    template <std::size_t N>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::integral_constant<std::size_t, N> /*unused*/) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_drop_impl{},
            std::integral_constant<std::size_t, N>{})
    );
    // clang-format on
};
#endif

} // end namespace detail

/// \verbatim embed:rst:leading-slashes
/// :math:`\mathtt{drop} : \mathbb{N} \to \text{View} \to
/// \text{View}` is a functor that let's you create dropped views.
/// Given a count :math:`n \in \mathbb{N}` and a :ref:`view
/// <view-concept>` :math:`xs`, creates a view of :math:`xs` with the
/// first :math:`n` elements dropped. If the size of :math:`xs` is
/// less than :math:`n`, an empty view is returned.
/// \endverbatim
// BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_drop_algo_impl, drop)

/// \verbatim embed:rst:leading-slashes
/// :math:`\mathtt{drop} : \mathbb{N} \to \text{View} \to \text{View}`
/// is a functor that let's you create dropped views. Very similar to
/// :cpp:var:`drop`, but has an extra precondition. When given a count
/// :math:`n` and a :ref:`view <view-concept>` :math:`xs`, it assumes
/// that the size of :math:`xs` is at least :math:`n`. If that's not
/// the case, an assertion failure is triggered.
///
/// .. warning::
///
///   If you disable the checks (i.e. define
///   ``BOOST_STATIC_VIEWS_DISABLE_CHECKS``) and pass an :math:`n >
///   xs\mathtt{.size()}`, **the behavior is undefined**.
/// \endverbatim
// BOOST_STATIC_VIEWS_INLINE_VARIABLE(
//     detail::make_drop_exactly_algo_impl, drop_exactly)
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::drop_exactly_impl, drop_exactly)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DROP_HPP
