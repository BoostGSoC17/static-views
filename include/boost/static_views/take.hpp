//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/take.hpp
///
/// \brief Implementation of #boost::static_views::take.

#ifndef BOOST_STATIC_VIEWS_TAKE_HPP
#define BOOST_STATIC_VIEWS_TAKE_HPP

#include "algorithm_base.hpp"
#include "detail/config.hpp"
#include "detail/utils.hpp"
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
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(std::is_nothrow_constructible<
            typename take_impl::view_adaptor_base_type, View&&>::value
            // This is formally wrong, but come on,
            // std::min(size_t, size_t) _should_ be noexcept.
            )
        : take_impl::view_adaptor_base_type{std::move(xs)}
        , _n{std::min(this->parent().size(), n)}
    {
    }

    /// \brief Returns the number of elements viewed.
    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const noexcept
    {
        return _n;
    }

    /// \brief "Maps" index \p i to the corresponding index in the
    /// parent view.

    /// For \f$ i \geq \text{xs.size}()\f$ behavior of this function
    /// is undefined
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto map(std::size_t const i) const
        noexcept
    {
        return i;
    }

  private:
    std::size_t _n;
};

struct make_take_impl {
    // clang-format off
    template <class View>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs, std::size_t const n) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        take_impl<std::decay_t<View>,
            View::type::extent()>{std::forward<View>(xs), n}
    );
    // clang-format on
};

struct make_take_algo_impl {
    // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::size_t const n) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_take_impl{}, std::size_t{n})
        /*
        algorithm_impl<make_take_impl, std::size_t>(
            make_take_impl{}, n)*/
    );
    // clang-format on
};

} // end namespace detail

/// \verbatim embed:rst:leading-slashes
/// :math:`\textit{take} : \mathbb{N} \to (\text{View} \to
/// \text{View})` is a functor that let's you create take views. Given
/// a count :math:`n \in \mathbb{N}` and a :ref:`view <view-concept>`
/// :math:`xs`, creates a view of :math:`xs` consisting only of the
/// first :math:`n` elements of :math:`xs`. If the size of :math:`xs`
/// is less than :math:`n`, view of the whole :math:`xs` is returned.
/// \endverbatim
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_take_algo_impl, take)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_TAKE_HPP
