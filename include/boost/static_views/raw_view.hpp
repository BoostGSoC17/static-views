//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/raw_view.hpp
///
/// \brief Implementation of #boost::static_views::raw_view.

#ifndef BOOST_STATIC_VIEWS_RAW_VIEW_HPP
#define BOOST_STATIC_VIEWS_RAW_VIEW_HPP

#include "detail/config.hpp"
#include "detail/utils.hpp"
#include "errors.hpp"
#include "sequence_traits.hpp"
#include "view_base.hpp"
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

template <std::size_t N>
constexpr auto capacity = std::integral_constant<std::size_t, N>{};

namespace detail {

template <class Sequence, std::ptrdiff_t Extent>
//    class = std::enable_if_t<concepts::Sequence::test<
//        std::remove_cv_t<std::remove_reference_t<Sequence>>>()>>
struct raw_view_impl : view_base {

  private:
    using sequence_type = std::remove_cv_t<Sequence>;
    Sequence* _xs;

  public:
    BOOST_STATIC_VIEWS_CONSTEXPR
    explicit raw_view_impl(Sequence& xs) noexcept : _xs{&xs} {}

    BOOST_STATIC_VIEWS_CONSTEXPR
    raw_view_impl(raw_view_impl const&) noexcept = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    raw_view_impl(raw_view_impl&&) noexcept = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    raw_view_impl& operator=(raw_view_impl const&) noexcept = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    raw_view_impl& operator=(raw_view_impl&&) noexcept = default;

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return sequence_traits<sequence_type>::extent();
    }

    constexpr auto size() const noexcept -> std::size_t
    {
        return sequence_traits<sequence_type>::size(*_xs);
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto unsafe_at(std::size_t const i) const
        // clang-format off
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        sequence_traits<sequence_type>::at(*_xs, i)
    );
    // clang-format on

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](
        std::size_t const i) const
    {
        return (i < size()) ? unsafe_at(i)
                            : (make_out_of_bound_error(
                                   "`i < size()` not satisfied."),
                                  unsafe_at(i));
    }
};

/// \cond
struct make_raw_view {
  public:
    template <class Seq>
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(Seq& sequence) const
        noexcept
    {
        using sequence_type = std::remove_cv_t<Seq>;
        concepts::Sequence::check<sequence_type>();
        constexpr auto extent =
            sequence_traits<sequence_type>::extent();
        return raw_view_impl<Seq, extent>{sequence};
    }
};
/// \endcond

} // namespace detail

/// \verbatim embed:rst:leading-slashes
/// :math:`\textit{raw_view} : \text{Sequence} \to \text{View}` (i.e.
/// takes a :ref:`sequence <sequence-concept>` and returns a
/// :ref:`view <view-concept>`). It will trigger ``static_assert``
/// failures if passed a type not modeling the
/// :ref:`sequence <sequence-concept>` concept.
/// \endverbatim
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_raw_view, raw_view)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_RAW_VIEW_HPP
