//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_COMPACT_INDEX_HPP
#define BOOST_STATIC_VIEWS_COMPACT_INDEX_HPP

#include "view_base.hpp"

#include <algorithm>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

template <class IndexType, std::ptrdiff_t Extent = dynamic_extent, class = void>
struct compact_index {
  private:
    IndexType _payload;

    static_assert(std::is_integral<IndexType>::value,
        "boost::static_views::detail::compact_index is meant to be used "
        "with integral types.");

  public:
    BOOST_STATIC_VIEWS_CONSTEXPR
    compact_index(IndexType const index) noexcept
        : _payload{index}
    {
        static_assert(std::is_nothrow_copy_constructible<IndexType>::value,
            "boost::static_views::detail::compact_index assumes that "
            "IndexType is nothrow copy-constructible.");
    }

    // clang-format off
    constexpr compact_index(compact_index const&) noexcept = default;
    constexpr compact_index(compact_index&&) noexcept      = default;
    constexpr compact_index& operator=(compact_index const&) noexcept = default;
    constexpr compact_index& operator=(compact_index&&) noexcept = default;
    // clang-format on

    static constexpr auto extent() noexcept -> std::ptrdiff_t { return Extent; }

    BOOST_STATIC_VIEWS_PURE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto index() const noexcept { return _payload; }

    BOOST_STATIC_VIEWS_PURE
    BOOST_STATIC_VIEWS_CONSTEXPR
    operator IndexType() const noexcept { return index(); }
};

template <class IndexType, std::ptrdiff_t Extent>
struct compact_index<IndexType, Extent, std::enable_if_t<Extent >= 0>> {
  public:
    // clang-format off
    constexpr compact_index() noexcept = default;
    constexpr compact_index(compact_index const&) noexcept = default;
    constexpr compact_index(compact_index&&) noexcept      = default;
    constexpr compact_index& operator=(compact_index const&) noexcept = default;
    constexpr compact_index& operator=(compact_index&&) noexcept = default;
    // clang-format on

    static_assert(std::is_convertible<std::ptrdiff_t, IndexType>::value,
        "boost::static_views::detail::compact_index assumes that "
        "std::ptrdiff_t is convertible to IndexType.");

    static constexpr auto extent() noexcept -> std::ptrdiff_t { return Extent; }

    BOOST_STATIC_VIEWS_PURE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto index() const noexcept { return static_cast<IndexType>(Extent); }

    /*
    BOOST_STATIC_VIEWS_PURE
    BOOST_STATIC_VIEWS_CONSTEXPR
    operator IndexType() const noexcept { return index(); }
    */
};

// clang-format off
template <class IndexType
    BOOST_STATIC_VIEWS_REQUIRES(std::is_integral<IndexType>::value)
BOOST_STATIC_VIEWS_CONSTEXPR auto index(IndexType const x) noexcept
// clang-format on
{
    return compact_index<IndexType>{x};
}

template <class IndexType, IndexType I>
BOOST_STATIC_VIEWS_CONSTEXPR auto index(
    std::integral_constant<IndexType, I> /*unused*/) noexcept
{
    return compact_index<IndexType, I>{};
}

#define BOOST_STATIC_VIEWS_DECLARE_BIN_OP(op)                                  \
    template <class IndexType, std::ptrdiff_t ExtentA, std::ptrdiff_t ExtentB> \
    BOOST_STATIC_VIEWS_PURE BOOST_STATIC_VIEWS_CONSTEXPR auto operator op(     \
        compact_index<IndexType, ExtentA> const a,                             \
        compact_index<IndexType, ExtentB> const b) noexcept->bool              \
    {                                                                          \
        return a.index() op b.index();                                         \
    }

BOOST_STATIC_VIEWS_DECLARE_BIN_OP(==)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(!=)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(<)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(<=)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(>)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(>=)

#undef BOOST_STATIC_VIEWS_DECLARE_BIN_OP

#define BOOST_STATIC_VIEWS_DECLARE_BIN_OP(op)                                  \
    template <class IndexType, std::ptrdiff_t Extent>                          \
    BOOST_STATIC_VIEWS_PURE BOOST_STATIC_VIEWS_CONSTEXPR auto operator op(     \
        compact_index<IndexType, Extent> const a,                              \
        IndexType const                        b) noexcept->bool                                      \
    {                                                                          \
        return a.index() op b;                                                 \
    }                                                                          \
                                                                               \
    template <class IndexType, std::ptrdiff_t Extent>                          \
    BOOST_STATIC_VIEWS_PURE BOOST_STATIC_VIEWS_CONSTEXPR auto operator op(     \
        IndexType const                        a,                              \
        compact_index<IndexType, Extent> const b) noexcept->bool               \
    {                                                                          \
        return a op b.index();                                                 \
    }

BOOST_STATIC_VIEWS_DECLARE_BIN_OP(==)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(!=)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(<)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(<=)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(>)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(>=)

#undef BOOST_STATIC_VIEWS_DECLARE_BIN_OP

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_COMPACT_INDEX_HPP
