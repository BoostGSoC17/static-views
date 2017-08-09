//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/drop.hpp
///
/// \brief Implementation of #drop.

#ifndef BOOST_STATIC_VIEWS_DROP_HPP
#define BOOST_STATIC_VIEWS_DROP_HPP

#include "algorithm_base.hpp"
#include "detail/config.hpp"
#include "detail/utils.hpp"
#include "sequence_traits.hpp"
#include "view_base.hpp"
#include <algorithm>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class Wrapper, std::ptrdiff_t Extent>
struct drop_impl
    : view_adaptor_base<drop_impl<Wrapper, Extent>, Wrapper> {

    /// \brief Constructs a view of \p xs with the first \p b elements
    /// dropped.

    /// \param xs **Rvalue** reference to a wrapper around a view.
    /// \param b  Number of elements to drop. The resulting view will
    ///           contain `max(xs.get().size() - b, 0)` elements.
    ///
    /// \exception Never throws, unless the initialisation of
    /// #view_adaptor_base with `Wrapper&&` throws.
    explicit BOOST_STATIC_VIEWS_CONSTEXPR drop_impl(
        Wrapper&& xs, std::size_t const b)
#if !defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)
        // This is formally wrong, but come on,
        // std::min(size_t, size_t) _should_ be noexcept.
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(std::is_nothrow_constructible<
            typename drop_impl::view_adaptor_base_type,
            Wrapper&&>::value)
#endif
        : drop_impl::view_adaptor_base_type{std::move(xs)}, _b{b}
    {
        BOOST_STATIC_VIEWS_EXPECT(b <= this->parent().size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        BOOST_STATIC_VIEWS_EXPECT(
            extent() == dynamic_extent || extent() >= size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
    }

    /// \brief Returns the number of elements viewed.
    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const noexcept
        -> std::size_t
    {
        static_assert(noexcept(this->parent().size()),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        // Constructor makes sure this never causes underflow
        BOOST_STATIC_VIEWS_EXPECT(_b <= this->parent().size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return this->parent().size() - _b;
    }

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return Extent;
    }

    /// \brief "Maps" index \p i to the corresponding index in the
    /// parent view.

    /// For \f$ i \geq \text{xs.size}()\f$ behavior of this function
    /// is undefined
    BOOST_STATIC_VIEWS_CONSTEXPR auto map(std::size_t const i) const
        noexcept
    {
        BOOST_STATIC_VIEWS_EXPECT(i < size(),
            "Which part of `map: [0, size()) --> N` you didn't "
            "understand?");
        return _b + i;
    }

  private:
    std::size_t _b;
};

struct make_drop_exactly_impl {
    // clang-format off
    template <class Wrapper>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Wrapper&& xs, std::size_t const b) const
    // clang-format on
#if !defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            drop_impl<std::decay_t<Wrapper>, Wrapper::type::extent()>{
                std::forward<Wrapper>(xs), b}))
#endif
    {
        BOOST_STATIC_VIEWS_EXPECT(b <= xs.get().size(),
            "drop_exactly(n)(xs) expects `n <= xs.size()`.");
        BOOST_STATIC_VIEWS_EXPECT(
            Wrapper::type::extent() == dynamic_extent
                || Wrapper::type::extent() >= xs.get().size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return drop_impl<std::decay_t<Wrapper>,
            Wrapper::type::extent()>{std::forward<Wrapper>(xs), b};
    }

    // clang-format off
    template <class Wrapper, std::size_t N>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Wrapper&& xs,
        std::integral_constant<std::size_t, N> /*unused*/) const
    // clang-format on
#if !defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            drop_impl<std::decay_t<Wrapper>, Wrapper::type::extent()>{
                std::forward<Wrapper>(xs), N}))
#endif
    {
        BOOST_STATIC_VIEWS_EXPECT(N <= xs.get().size(),
            "drop_exactly(n)(xs) expects `n <= xs.size()`.");
        BOOST_STATIC_VIEWS_EXPECT(
            Wrapper::type::extent() == dynamic_extent
                || Wrapper::type::extent() >= xs.get().size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return drop_impl<std::decay_t<Wrapper>,
            ((Wrapper::type::extent() > N)
                    ? (Wrapper::type::extent() - N)
                    : 0)>{std::forward<Wrapper>(xs), N};
    }
};

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

} // end namespace detail

/// \verbatim embed:rst:leading-slashes
/// :math:`\mathtt{drop} : \mathbb{N} \to \text{View} \to
/// \text{View}` is a functor that let's you create dropped views.
/// Given a count :math:`n \in \mathbb{N}` and a :ref:`view
/// <view-concept>` :math:`xs`, creates a view of :math:`xs` with the
/// first :math:`n` elements dropped. If the size of :math:`xs` is
/// less than :math:`n`, an empty view is returned.
/// \endverbatim
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_drop_algo_impl, drop)

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
BOOST_STATIC_VIEWS_INLINE_VARIABLE(
    detail::make_drop_exactly_algo_impl, drop_exactly)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DROP_HPP
