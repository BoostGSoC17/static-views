//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../include/boost/static_views/detail/config.hpp"

#if defined(BOOST_STATIC_VIEWS_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif

#include <boost/config.hpp>
#include <boost/config/workaround.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>

#if defined(BOOST_STATIC_VIEWS_CLANG)
#pragma clang diagnostic pop
#endif

#if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(BOOST_MSVC))
    // || BOOST_WORKAROUND(BOOST_GCC, BOOST_TESTED_AT(BOOST_GCC))
// C++-14 constexpr support not good enough

#define CONSTEXPR const /* no constexpr */
#define STATIC_ASSERT(expr, msg) BOOST_TEST(expr&& msg)

#else
// Supports C++-14

#define CONSTEXPR BOOST_STATIC_VIEWS_CONSTEXPR
#define STATIC_ASSERT(expr, msg) static_assert(expr, msg)

#endif


#if defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)
#define NOEXCEPT(...) true
#else
#define NOEXCEPT(...) noexcept(__VA_ARGS__)
#endif


struct non_copyable {
    constexpr non_copyable() noexcept {}

    non_copyable(non_copyable const&) = delete;
    non_copyable& operator=(non_copyable const& /*other*/) = delete;

    constexpr non_copyable(
        non_copyable&& /*other*/) noexcept = default;
    // clang-format off
    constexpr non_copyable& operator=(non_copyable&&  /*other*/)
        noexcept = default;
    // clang-format on
};
