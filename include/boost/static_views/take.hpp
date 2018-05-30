//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/take.hpp
///
/// \brief Implementation of #take.

#ifndef BOOST_STATIC_VIEWS_TAKE_HPP
#define BOOST_STATIC_VIEWS_TAKE_HPP

#include "view_base.hpp"

#include <algorithm>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class Wrapper, std::ptrdiff_t Extent>
struct take_view_impl
    : view_adaptor_base<take_view_impl<Wrapper, Extent>, Wrapper> {

  private:
    using wrapper_type = Wrapper;
    using base = view_adaptor_base<take_view_impl<Wrapper, Extent>, Wrapper>;

  public:
    using typename base::index_type;
    using typename base::reference;
    using typename base::size_type;
    using typename base::value_type;

    using base::parent;

    /// \brief Constructs a view of \p xs consisting of at most \p n
    /// elements of \p xs.

    /// \param xs **Rvalue** reference to a wrapper around a view.
    /// \param n  Number of elements to take. `n` must not exceed the size of
    ///           `xs`.
    BOOST_STATIC_VIEWS_CONSTEXPR take_view_impl(
        wrapper_type&& xs, index_type const n)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            std::is_nothrow_constructible<base, View&&>::value)
        : base{std::move(xs)}, _n{n}
    {
        BOOST_STATIC_VIEWS_EXPECT(
            0 <= n && n <= static_cast<index_type>(parent().size()),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
    }

    take_view_impl(take_view_impl const&) = default;
    take_view_impl(take_view_impl&&)      = default;
    take_view_impl& operator=(take_view_impl const&) = default;
    take_view_impl& operator=(take_view_impl&&) = default;

    /// \brief Returns the number of elements viewed.
    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const noexcept -> size_type
    {
        return static_cast<size_type>(_n);
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
            "boost::static_views::take_view_impl::map: Precondition "
            "`0 <= i < size()` is not satisfied.");
        return i;
    }

  private:
    index_type _n;
};

struct take_exactly_impl {
  private:
    template <class Wrapper, class IndexType>
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(Wrapper xs, IndexType n) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            take_view_impl<Wrapper, dynamic_extent>{std::move(xs), n});

  public:
    // clang-format off
    template <class V
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(V&& xs,
        typename std::remove_cv_t<
                 std::remove_reference_t<V>>::index_type const n) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            noexcept(take_impl<std::decay_t<View>, View::type::extent()>{
                std::forward<View>(xs), n}))
    {
        using index_type =
            typename std::remove_cv_t<std::remove_reference_t<V>>::index_type;
        BOOST_STATIC_VIEWS_EXPECT(
            0 <= n && n <= static_cast<index_type>(xs.size()),
            "boost::static_views::take_exactly(xs, n): Precondition "
            "`0 <= n <= xs.size()` is not satisfied.");
        return call_impl(make_wrapper(std::forward<V>(xs)), n);
    }
};

#if 0
struct make_take_exactly_algo_impl {
    // clang-format off
    template <std::size_t N>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(
        std::integral_constant<std::size_t, N> /*unused*/) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_take_exactly_impl{},
            std::integral_constant<std::size_t, N>{})
    );
    // clang-format on

    // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::size_t const n) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_take_exactly_impl{}, std::size_t{n})
    );
    // clang-format on
};

struct make_take_impl {
    // clang-format off
    template <class View, std::size_t N>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs,
        std::integral_constant<std::size_t, N> /*unused*/) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        take_impl<std::decay_t<View>,
            ((View::type::extent() == dynamic_extent
                 || static_cast<std::size_t>(View::type::extent()) > N)
                    ? static_cast<std::ptrdiff_t>(N)
                    : View::type::extent())>{
            std::forward<View>(xs), std::min(xs.get().size(), N)}
    );
    // clang-format on

    // clang-format off
    template <class View>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs, std::size_t const n) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        take_impl<std::decay_t<View>,
            View::type::extent()>{std::forward<View>(xs),
                std::min(n, xs.get().size())}
    );
    // clang-format on
};

struct make_take_algo_impl {
    // clang-format off
    template <std::size_t N>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(
        std::integral_constant<std::size_t, N> /*unused*/) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_take_impl{},
            std::integral_constant<std::size_t, N>{})
    );
    // clang-format on

    // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::size_t const n) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_take_impl{}, std::size_t{n})
    );
    // clang-format on
};
#endif
} // end namespace detail

/// \verbatim embed:rst:leading-slashes
/// :math:`\mathtt{take} : \mathbb{N} \to \text{View} \to
/// \text{View}` is a functor that let's you create take views. Given
/// a count :math:`n \in \mathbb{N}` and a :ref:`view <view-concept>`
/// :math:`xs`, creates a view of :math:`xs` consisting only of the
/// first :math:`n` elements of :math:`xs`. If the size of :math:`xs`
/// is less than :math:`n`, view of the whole :math:`xs` is returned.
/// \endverbatim
// BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_take_algo_impl, take)

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::take_exactly_impl, take_exactly)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_TAKE_HPP
