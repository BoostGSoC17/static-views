//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/take.hpp
///
/// \brief Implementation of #take.

#ifndef BOOST_STATIC_VIEWS_TAKE_HPP
#define BOOST_STATIC_VIEWS_TAKE_HPP

#include "algorithm_base.hpp"
#include "compact_index.hpp"
#include "view_base.hpp"

#include <algorithm>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class Wrapper, std::ptrdiff_t Extent>
struct take_view_impl
    : view_adaptor_base<take_view_impl<Wrapper, Extent>, Wrapper>
    , private compact_index<
          typename view_adaptor_base<take_view_impl<Wrapper, Extent>,
              Wrapper>::index_type,
          Extent> {

  private:
    using wrapper_type = Wrapper;
    using base = view_adaptor_base<take_view_impl<Wrapper, Extent>, Wrapper>;
    using compact_index_type = compact_index<typename base::index_type, Extent>;
    using compact_index_type::index;

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
        wrapper_type&& xs, compact_index_type const n)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            std::is_nothrow_constructible<base, Wrapper&&>::value)
        : base{std::move(xs)}, compact_index_type{n}
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
    BOOST_STATIC_VIEWS_PURE
    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const noexcept -> size_type
    {
        return static_cast<size_type>(index());
    }

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        static_assert(base::extent() == dynamic_extent
                          || compact_index_type::extent() == dynamic_extent
                          || base::extent() >= compact_index_type::extent(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return compact_index_type::extent() == dynamic_extent
                   ? base::extent()
                   : compact_index_type::extent();
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
};

struct take_exactly_impl {
  private:
    template <class Wrapper, class IndexType>
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(
        Wrapper xs, IndexType const n) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            take_view_impl<Wrapper, IndexType::extent()>{std::move(xs), n});

    template <class T>
    using index_t =
        typename std::remove_cv_t<std::remove_reference_t<T>>::index_type;

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
            noexcept(std::declval<take_exactly_impl const&>().call_impl(
                make_wrapper(std::forward<V>(xs)), index(n))))
    {
        using index_type = index_t<V>;
        BOOST_STATIC_VIEWS_EXPECT(
            0 <= n && n <= static_cast<index_type>(xs.size()),
            "boost::static_views::take_exactly(xs, n): Precondition "
            "`0 <= n <= xs.size()` is not satisfied.");
        return call_impl(make_wrapper(std::forward<V>(xs)), index(n));
    }

    // clang-format off
    template <class IndexType
        BOOST_STATIC_VIEWS_REQUIRES(std::is_integral<IndexType>::value)
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(IndexType const n) const noexcept
    // clang-format on
    {
        return lazy_adaptor(take_exactly_impl{}, n);
    }

    // clang-format off
    template <class V, class Int, Int I
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(V&& xs, std::integral_constant<Int, I> /*unused*/) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            noexcept(std::declval<take_exactly_impl const&>().call_impl(
                make_wrapper(std::forward<V>(xs)),
                index(std::integral_constant<index_t<V>, I>{}))))
    {
        using index_type = index_t<V>;
        using view_type = std::remove_cv_t<std::remove_reference_t<V>>;
        constexpr auto n = static_cast<index_type>(I);
        static_assert(
            view_type::extent() == dynamic_extent || view_type::extent() >= n,
            "boost::static_views::take_exactly(xs, n): Can't take more "
            "elements than there are in the view.");
        BOOST_STATIC_VIEWS_EXPECT(
            0 <= n && n <= static_cast<index_type>(xs.size()),
            "boost::static_views::take_exactly(xs, b): Precondition "
            "`0 <= n <= xs.size()` is not satisfied.");
        return call_impl(make_wrapper(std::forward<V>(xs)),
            index(std::integral_constant<index_type, I>{}));
    }

    // clang-format off
    template <class IndexType, IndexType I>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::integral_constant<IndexType, I> const n) const noexcept
    // clang-format on
    {
        return lazy_adaptor(take_exactly_impl{}, n);
    }

#if !defined(BOOST_STATIC_VIEWS_SFINAE)
    // clang-format off
    template <class V, class IndexType
        BOOST_STATIC_VIEWS_REQUIRES(
            !View<std::remove_cv_t<std::remove_reference_t<V>>>)
    constexpr auto operator()(V&& /*unused*/, IndexType /*unused*/) const noexcept
    // clang-format on
    {
        static_assert(View<std::remove_cv_t<std::remove_reference_t<V>>>,
            "`boost::static_views::take_exactly(xs, n)` requires xs to model "
            "the View concept.");
        return why_is_my_argument_not_a_view<
            std::remove_cv_t<std::remove_reference_t<V>>>();
    }
#endif
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
// BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_take_algo_impl, take)

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::take_exactly_impl, take_exactly)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_TAKE_HPP
