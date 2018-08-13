//          Copyright Tom Westerhout 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/chunk.hpp
///
/// \brief Implementation of #chunk.

#ifndef BOOST_STATIC_VIEWS_CHUNK_HPP
#define BOOST_STATIC_VIEWS_CHUNK_HPP

#include "algorithm_base.hpp"
#include "detail/invoke.hpp"
#include "drop.hpp"
#include "take.hpp"

#include <algorithm>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

struct chunk_size_tag {
};

template <class Wrapper, std::ptrdiff_t ChunkSize>
struct chunk_view_impl
    : public view_adaptor_base<chunk_view_impl<Wrapper, ChunkSize>, Wrapper>
    , private compact_index<chunk_size_tag,
          typename view_adaptor_base<chunk_view_impl<Wrapper, ChunkSize>,
              Wrapper>::index_type,
          ChunkSize> {

  private:
    using wrapper_type = Wrapper;
    using base =
        view_adaptor_base<chunk_view_impl<Wrapper, ChunkSize>, Wrapper>;

  public:
    using base::parent;
    using typename base::index_type;
    using typename base::size_type;

  private:
    typename base::index_type _chunk_count;

    using chunk_size_type =
        compact_index<chunk_size_tag, index_type, ChunkSize>;
    using chunk_size_type::index;

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto chunk_count() const noexcept -> index_type { return _chunk_count; }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto chunk_size() const noexcept -> index_type { return index(); }

  public:
    BOOST_STATIC_VIEWS_CONSTEXPR chunk_view_impl(Wrapper&& xs,
        index_type const chunk_count, chunk_size_type const chunk_size)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            std::is_nothrow_constructible<base, Wrapper&&>::value)
        : base{std::move(xs)}
        , chunk_size_type{chunk_size}
        , _chunk_count{chunk_count}
    {
        BOOST_STATIC_VIEWS_EXPECT(
            parent().size() == static_cast<size_type>(chunk_count * chunk_size),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
    }

    chunk_view_impl(chunk_view_impl const&) = default;
    chunk_view_impl(chunk_view_impl&&)      = default;
    chunk_view_impl& operator=(chunk_view_impl const&) = default;
    chunk_view_impl& operator=(chunk_view_impl&&) = default;

    /// \brief Returns the number of elements viewed.
    BOOST_STATIC_VIEWS_PURE
    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const noexcept -> size_type
    {
        return static_cast<size_type>(chunk_size());
    }

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return dynamic_extent;
    }

  private:
    // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto unsafe_at_impl(index_type const i, std::true_type /*is static*/) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    // clang-format on
    {
        return take_exactly(drop_exactly(parent(), i),
            std::integral_constant<index_type, ChunkSize>{});
    }

    // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto unsafe_at_impl(index_type const i, std::false_type /*is static*/) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    // clang-format on
    {
        return take_exactly(drop_exactly(parent(), i), chunk_size());
    }

  public:
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto unsafe_at(index_type const i) const&
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    {
        BOOST_STATIC_VIEWS_EXPECT(0 <= i && i < static_cast<index_type>(size()),
            "boost::static_views::chunk_view_impl::unsafe_at: Precondition "
            "`0 <= i < size()` is not satisfied.");
        auto const start = chunk_size() * i;
        return unsafe_at_impl(
            start, std::integral_constant<bool,
                       chunk_size_type::extent() != dynamic_extent>{});
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator[](index_type const i) const&
    {
        auto const start = chunk_size() * i;
        if (BOOST_STATIC_VIEWS_UNLIKELY(i >= static_cast<index_type>(size()))) {
            make_out_of_bound_error(
                "Precondition `i < size()` not satisfied in "
                "boost::static_views::chunk_view_impl::operator[].");
            BOOST_STATIC_VIEWS_UNREACHABLE;
        }
        // TODO: using ChunkSize explicitly here is ugly, but Clang barfs
        // on chunk_size_type::extent().
        return unsafe_at_impl(
            start, std::integral_constant<bool, ChunkSize != dynamic_extent>{});
    }

    using reference = decltype(std::declval<chunk_view_impl const&>()[std::declval<index_type>()]);
    using value_type = std::remove_reference_t<reference>;
};

struct chunk_impl {
  private:
    template <class Wrapper, class ChunkCount, class ChunkSize>
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(
        Wrapper xs, ChunkCount chunk_count, ChunkSize chunk_size) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            chunk_view_impl<Wrapper, ChunkSize::extent()>{
                std::move(xs), chunk_count, chunk_size});

    template <class T>
    using index_t =
        typename std::remove_cv_t<std::remove_reference_t<T>>::index_type;

    template <class T>
    using size_type_t = typename remove_cvref_t<T>::size_type;

    // clang-format off
    template <class V, class ChunkCount, class ChunkSize
        BOOST_STATIC_VIEWS_REQUIRES(View<remove_cvref_t<V>>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto call(V&& xs, ChunkCount chunk_count,
            ChunkSize chunk_size, int /*unused*/) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<chunk_impl const&>().call_impl(
                make_wrapper(std::forward<V>(xs)), chunk_count, chunk_size)))
    {
        return call_impl(
            make_wrapper(std::forward<V>(xs)), chunk_count, chunk_size);
    }

#if !defined(BOOST_STATIC_VIEWS_SFINAE)
    template <class V, class ChunkCount, class ChunkSize>
    constexpr auto call(V&& /*unused*/, ChunkCount /*unused*/,
        ChunkSize /*unused*/, ...) const noexcept
    {
        static_assert(View<remove_cvref_t<V>>,
            "`boost::static_views::chunk(xs, n)` requires xs to model "
            "the View concept.");
        return why_is_my_argument_not_a_view<remove_cvref_t<V>>();
    }
#endif

  public:
    template <class V>
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(
        V&& xs, typename remove_cvref_t<V>::index_type const chunk_size) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            noexcept(std::declval<chunk_impl const&>().call(std::forward<V>(xs),
                static_cast<index_t<V>>(xs.size()) / chunk_size,
                index<chunk_size_tag>(chunk_size), int{})))
    {
        BOOST_STATIC_VIEWS_EXPECT(
            0 < chunk_size
                && static_cast<typename remove_cvref_t<V>::size_type>(
                       chunk_size)
                       <= xs.size()
                && xs.size() % chunk_size == 0,
            "boost::static_views::chunk(xs, n): Preconditions not satisfied.");
        return call(std::forward<V>(xs),
            static_cast<index_t<V>>(xs.size()) / chunk_size,
            index<chunk_size_tag>(chunk_size), int{});
    }

    template <class V, class IndexType, IndexType N>
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(
        V&& xs, std::integral_constant<IndexType, N> const chunk_size) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            noexcept(std::declval<chunk_impl const&>().call(std::forward<V>(xs),
                static_cast<index_t<V>>(xs.size()) / chunk_size,
                index<chunk_size_tag>(chunk_size), int{})))
    {
        BOOST_STATIC_VIEWS_EXPECT(0 < chunk_size && chunk_size <= xs.size()
                                      && xs.size() % chunk_size == 0,
            "boost::static_views::chunk(xs, n): Preconditions not satisfied.");
        return call(std::forward<V>(xs),
            static_cast<index_t<V>>(xs.size()) / chunk_size,
            index<chunk_size_tag>(chunk_size), int{});
    }

    // clang-format off
    template <class Int
        BOOST_STATIC_VIEWS_REQUIRES(std::is_integral<Int>::value)
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(Int const chunk_size) const noexcept
    // clang-format on
    {
        return lazy_adaptor(chunk_impl{}, copy(chunk_size));
    }

    // clang-format off
    template <class IndexType, IndexType N>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::integral_constant<IndexType, N> const chunk_size) const noexcept
    // clang-format on
    {
        return lazy_adaptor(chunk_impl{}, copy(chunk_size));
    }
};

} // end namespace detail


BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::chunk_impl, chunk)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_CHUNK_HPP

