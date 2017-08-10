//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_PIPE_HPP
#define BOOST_STATIC_VIEWS_PIPE_HPP

#include "detail/config.hpp"
#include "detail/invoke.hpp"
#include "detail/utils.hpp"
#include "view_base.hpp"
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

// clang-format off
template <class View, class Function,
    class = std::enable_if_t<concepts::View::template test<View>()>>
BOOST_STATIC_VIEWS_FORCEINLINE
BOOST_STATIC_VIEWS_CONSTEXPR
auto operator|(View&& xs, Function&& pipe)
    BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN
(
    invoke(std::forward<Function>(pipe), std::forward<View>(xs))
);
// clang-format on

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_PIPE_HPP
