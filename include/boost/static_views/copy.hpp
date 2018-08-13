//          Copyright Tom Westerhout 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/copy.hpp
///
/// \brief Implementation of #copy.

#ifndef BOOST_STATIC_VIEWS_COPY_HPP
#define BOOST_STATIC_VIEWS_COPY_HPP

#include "concepts.hpp"

#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

struct copy_impl {

  public:
    // clang-format off
    template <class T
        BOOST_STATIC_VIEWS_REQUIRES(std::is_copy_constructible<T>::value)
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(T const& x) const
              // clang-format on
              BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                  std::is_nothrow_copy_constructible<T>::value)
                  ->T
    {
        return x;
    }

#if !defined(BOOST_STATIC_VIEWS_SFINAE)
    // clang-format off
    template <class T
        BOOST_STATIC_VIEWS_REQUIRES(!std::is_copy_constructible<T>::value)
    constexpr auto operator()(T const& /* unused */) const noexcept -> void
    // clang-format on
    {
        static_assert(std::is_copy_constructible<T>::value,
            "`boost::static_views::copy(x)` requires x to be copy-constructible.");
    }
#endif
};

} // end namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::copy_impl, copy)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_COPY_HPP

