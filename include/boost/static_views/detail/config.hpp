//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_DETAIL_CONFIG_HPP
#define BOOST_STATIC_VIEWS_DETAIL_CONFIG_HPP

#include <exception>
#include <iostream>

// Define the following macro if you want to use StaticViews as part
// of Boost.
//#define BOOST_STATIC_VIEWS_USE_BOOST

// Define the following macro if when using Doxygen.
//#define DOXYGEN_IN_HOUSE

#if defined(BOOST_STATIC_VIEWS_USE_BOOST)
//////////////////////////////////////////////////////////////////////////////
// We have Boost at our disposal
//////////////////////////////////////////////////////////////////////////////
#include <boost/config.hpp>

// Check for compilers
#if defined(BOOST_CLANG)
#define BOOST_STATIC_VIEWS_CLANG                                     \
    (__clang_major__ * 10000 + __clang_minor__ * 100                 \
        + __clang_patchlevel__)

#elif defined(BOOST_GCC)
#define BOOST_STATIC_VIEWS_GCC BOOST_GCC

#elif defined(BOOST_MSVC)
#define BOOST_STATIC_VIEWS_MSVC BOOST_MSVC
#endif

// Force inlining
#define BOOST_STATIC_VIEWS_FORCEINLINE BOOST_FORCEINLINE

// Ask compiler not to inline
#define BOOST_STATIC_VIEWS_NOINLINE BOOST_NOINLINE

// Unused variable
#define BOOST_STATIC_VIEWS_UNUSED BOOST_ATTRIBUTE_UNUSED

// Noreturn
#define BOOST_STATIC_VIEWS_NORETURN BOOST_NORETURN

#else
//////////////////////////////////////////////////////////////////////////////
// No Boost --> do everything manually
//////////////////////////////////////////////////////////////////////////////

#if defined(__clang__)
// We're being compiled with Clang
#define BOOST_STATIC_VIEWS_CLANG                                     \
    (__clang_major__ * 10000 + __clang_minor__ * 100                 \
        + __clang_patchlevel__)

#define BOOST_STATIC_VIEWS_FORCEINLINE                               \
    inline __attribute__((__always_inline__))

#define BOOST_STATIC_VIEWS_NOINLINE __attribute__((__noinline__))

#define BOOST_STATIC_VIEWS_UNUSED __attribute__((__unused__))

#define BOOST_STATIC_VIEWS_NORETURN __attribute__((__noreturn__))

#elif defined(__GNUC__)
// We're being compiled with GCC
#define BOOST_STATIC_VIEWS_GCC                                       \
    (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#define BOOST_STATIC_VIEWS_FORCEINLINE                               \
    inline __attribute__((__always_inline__))

#define BOOST_STATIC_VIEWS_NOINLINE __attribute__((__noinline__))

#define BOOST_STATIC_VIEWS_UNUSED __attribute__((__unused__))

#define BOOST_STATIC_VIEWS_NORETURN __attribute__((__noreturn__))

#elif defined(_MSC_VER)
// We're being compiled with Microsoft Visual C++
#define BOOST_STATIC_VIEWS_MSVC _MSC_VER

#define BOOST_STATIC_VIEWS_FORCEINLINE inline __forceinline

#define BOOST_STATIC_VIEWS_NOINLINE

#define BOOST_STATIC_VIEWS_UNUSED

#define BOOST_STATIC_VIEWS_NORETURN __declspec(noreturn)

#else
// clang-format off
#   error "Unsupported compiler. Please, submit a request to https://github.com/boostgsoc17/static-views/issues."
// clang-format on
#endif

#endif

// Easily turn off constexpr-ness to add some debug output, for
// example.
#define BOOST_STATIC_VIEWS_CONSTEXPR constexpr

/// \brief Chooses whether to ignore the actual noexcept-ness of
/// std::tuple implementstion and make some reasonable assumptions
/// instead.
#define BOOST_STATIC_VIEWS_NEGLECT_STD_TUPLE

// Boost.StaticViews namespace
#define BOOST_STATIC_VIEWS_NAMESPACE boost::static_views

#define BOOST_STATIC_VIEWS_BEGIN_NAMESPACE                           \
    namespace boost {                                                \
    namespace static_views {

#define BOOST_STATIC_VIEWS_END_NAMESPACE                             \
    } /* static_views */                                             \
    } /* boost */

#if defined(DOXYGEN_IN_HOUSE)
// It's a bad idea to let Doxygen try deduce noexcept-ness.
#define BOOST_STATIC_VIEWS_NOEXCEPT_IF(...)                          \
    noexcept(whenever possible) /**/
#else
#define BOOST_STATIC_VIEWS_NOEXCEPT_IF(...) noexcept(__VA_ARGS__) /**/
#endif

#if defined(DOXYGEN_IN_HOUSE)
// Breathe does not handle decltype(auto) correctly, see
// https://github.com/michaeljones/breathe/issues/330. Hence, trick it
// into thinking that the function returns a plain auto insted.
#define BOOST_STATIC_VIEWS_DECLTYPE_AUTO auto
#else
#define BOOST_STATIC_VIEWS_DECLTYPE_AUTO decltype(auto)
#endif

#if 1
#if defined(DOXYGEN_IN_HOUSE)
#define BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT(...)                 \
    noexcept(whenever possible) { return (__VA_ARGS__); }            \
/**/
#else
// If a function body is just a return-statement, we can deduce the
// noexcept-ness automatically. This idea taken from range-v3.
#define BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT(...)                 \
    noexcept(noexcept(__VA_ARGS__)) { return (__VA_ARGS__); }        \
    /**/

#endif
#endif

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \cond
// See
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html
template <class T>
constexpr T _static_const{};
/// \endcond

BOOST_STATIC_VIEWS_END_NAMESPACE

#if defined(DOXYGEN_IN_HOUSE)
#define BOOST_STATIC_VIEWS_INLINE_VARIABLE(type, name)               \
    inline namespace {                                               \
        constexpr auto const& name =                                 \
            ::BOOST_STATIC_VIEWS_NAMESPACE::_static_const<type>;     \
    }                                                                \
/**/
#else
#define BOOST_STATIC_VIEWS_INLINE_VARIABLE(type, name)               \
    constexpr auto name = type{};                                    \
/**/
#endif

#endif // BOOST_STATIC_VIEWS_DETAIL_CONFIG_HPP
