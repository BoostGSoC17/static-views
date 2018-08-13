//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_PIPE_HPP
#define BOOST_STATIC_VIEWS_PIPE_HPP

#include "detail/config.hpp"
#include "detail/invoke.hpp"
#include "view_concept.hpp"

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

// namespace detail here is needed to make ADL work. Functions like raw_view
// return structs from boost::static_views::detail namespace rather than
// boost::static_views.
namespace detail {

// clang-format off
template <class V, class F
    BOOST_STATIC_VIEWS_REQUIRES(
        View<std::remove_cv_t<std::remove_reference_t<V>>>)
BOOST_STATIC_VIEWS_FORCEINLINE
BOOST_STATIC_VIEWS_CONSTEXPR
auto operator|(V&& xs, F&& pipe)
    BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN
(
    invoke(std::forward<F>(pipe), std::forward<V>(xs))
);
// clang-format on

} // namespace detail

using detail::operator|;

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_PIPE_HPP
