//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/take.hpp
///
/// \brief Implementation of #take.

#ifndef BOOST_STATIC_VIEWS_TAKE_HPP
#define BOOST_STATIC_VIEWS_TAKE_HPP

#include "algorithm_base.hpp"
#include "detail/config.hpp"
#include "detail/utils.hpp"
#include "sequence_traits.hpp"
#include "view_base.hpp"
#include <algorithm>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
template <class View, std::ptrdiff_t Extent>
struct take_impl : view_adaptor_base<take_impl<View, Extent>, View> {

    /// \brief Constructs a view of \p xs consisting of at most \p n
    /// elements of \p xs.

    /// \param xs **Rvalue** reference to a wrapper around a view.
    /// \param n  Number of elements to take. The resulting view will
    ///           contain `min(xs.get().size(), n)` elements.
    ///
    /// \exception Never throws, unless the initialisation of
    /// #view_adaptor_base with `Wrapper&&` throws.
    explicit BOOST_STATIC_VIEWS_CONSTEXPR take_impl(
        View&& xs, std::size_t const n)
#if defined(BOOST_STATIC_VIEWS_DISABLE_CHECKS)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(std::is_nothrow_constructible<
            typename take_impl::view_adaptor_base_type, View&&>::value
            // This is formally wrong, but come on,
            // std::min(size_t, size_t) _should_ be noexcept.
            )
#endif
        : take_impl::view_adaptor_base_type{std::move(xs)}, _n{n}
    {
        BOOST_STATIC_VIEWS_EXPECT(n <= this->parent().size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        BOOST_STATIC_VIEWS_EXPECT(
            extent() == dynamic_extent || extent() >= size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
    }

    /// \brief Returns the number of elements viewed.
    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const noexcept
        -> std::size_t
    {
        return _n;
    }

    BOOST_STATIC_VIEWS_CONSTEXPR static auto extent() noexcept
        -> std::ptrdiff_t
    {
        return Extent;
    }

    /// \brief "Maps" index \p i to the corresponding index in the
    /// parent view.

    /// For \f$ i \geq \text{xs.size}()\f$ behavior of this function
    /// is undefined
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto map(std::size_t const i) const
        noexcept
    {
        BOOST_STATIC_VIEWS_EXPECT(i < size(),
            "Which part of `map: [0, size()) --> N` you didn't "
            "understand?");
        return i;
    }

  private:
    std::size_t _n;
};

struct make_take_exactly_impl {
    // clang-format off
    template <class View, std::size_t N>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs,
        std::integral_constant<std::size_t, N> /*unused*/) const
    // clang-format on
#if defined(BOOST_STATIC_VIEWS_DISABLE_CHECKS)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            take_impl<std::decay_t<View>, View::type::extent()>{
                std::forward<View>(xs), N}))
#endif
    {
        BOOST_STATIC_VIEWS_EXPECT(N <= xs.get().size(),
            "take_exactly(n)(xs) expects `n <= xs.size()`.");
        BOOST_STATIC_VIEWS_EXPECT(
            View::type::extent() == dynamic_extent
                || View::type::extent() >= xs.get().size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return take_impl<std::decay_t<View>,
            ((View::type::extent() > static_cast<std::ptrdiff_t>(N))
                    ? static_cast<std::ptrdiff_t>(N)
                    : View::type::extent())>{
            std::forward<View>(xs), N};
    }

    // clang-format off
    template <class View>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs, std::size_t const n) const
#if defined(BOOST_STATIC_VIEWS_DISABLE_CHECKS)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            take_impl<std::decay_t<View>, View::type::extent()>{
                std::forward<View>(xs), n}))
#endif
    {
        BOOST_STATIC_VIEWS_EXPECT(n <= xs.get().size(),
            "take_exactly(n)(xs) expects `n <= xs.size()`.");
        BOOST_STATIC_VIEWS_EXPECT(
            View::type::extent() == dynamic_extent
                || View::type::extent() >= xs.get().size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return take_impl<std::decay_t<View>,
            View::type::extent()>{std::forward<View>(xs), n};
    }
    // clang-format on
};

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

} // end namespace detail

/// \verbatim embed:rst:leading-slashes
/// :math:`\mathtt{take} : \mathbb{N} \to \text{View} \to
/// \text{View}` is a functor that let's you create take views. Given
/// a count :math:`n \in \mathbb{N}` and a :ref:`view <view-concept>`
/// :math:`xs`, creates a view of :math:`xs` consisting only of the
/// first :math:`n` elements of :math:`xs`. If the size of :math:`xs`
/// is less than :math:`n`, view of the whole :math:`xs` is returned.
/// \endverbatim
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_take_algo_impl, take)

BOOST_STATIC_VIEWS_INLINE_VARIABLE(
    detail::make_take_exactly_algo_impl, take_exactly)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_TAKE_HPP
