//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_DETAIL_CONFIG_HPP
#define BOOST_STATIC_VIEWS_DETAIL_CONFIG_HPP

#include <cassert>

// Define the following macro if you want to use StaticViews as part
// of Boost.
//#define BOOST_STATIC_VIEWS_USE_BOOST

// Define the following macro if when using Doxygen.
//#define DOXYGEN_IN_HOUSE

/// \brief Chooses whether to ignore the actual noexcept-ness of
/// std::tuple implementstion and make some reasonable assumptions
/// instead.
#define BOOST_STATIC_VIEWS_NEGLECT_STD_TUPLE

#define BOOST_DISABLE_ASSERTS

// #define BOOST_STATIC_VIEWS_THROW_ON_FAILURES
// #define BOOST_STATIC_VIEWS_TERMINATE_ON_FAILURES

#if !defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)                   \
    && !defined(BOOST_STATIC_VIEWS_TERMINATE_ON_FAILURES)
#define BOOST_STATIC_VIEWS_THROW_ON_FAILURES
#endif

// Easily turn off constexpr-ness to add some debug output, for
// example.
#define BOOST_STATIC_VIEWS_CONSTEXPR constexpr

/// \brief Disables all checks to achieve better performance
// #define BOOST_STATIC_VIEWS_DISABLE_CHECKS

// Boost.StaticViews namespace
#define BOOST_STATIC_VIEWS_NAMESPACE boost::static_views

#define BOOST_STATIC_VIEWS_BEGIN_NAMESPACE                           \
    namespace boost {                                                \
    namespace static_views {

#define BOOST_STATIC_VIEWS_END_NAMESPACE                             \
    } /* static_views */                                             \
    } /* boost */

#define BOOST_STATIC_VIEWS_BUG_MESSAGE                               \
    "Congratulations, you've found a bug in the StaticViews "        \
    "library! Please, be so kind to submit here "                    \
    "https://github.com/BoostGSoC17/static-views/issues."

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
// ===========================================================================
// We're being compiled with Clang
#define BOOST_STATIC_VIEWS_CLANG                                     \
    (__clang_major__ * 10000 + __clang_minor__ * 100                 \
        + __clang_patchlevel__)

#define BOOST_STATIC_VIEWS_FORCEINLINE                               \
    inline __attribute__((__always_inline__))

#define BOOST_STATIC_VIEWS_NOINLINE __attribute__((__noinline__))

#define BOOST_STATIC_VIEWS_UNUSED __attribute__((__unused__))

#define BOOST_STATIC_VIEWS_NORETURN __attribute__((__noreturn__))

#define BOOST_STATIC_VIEWS_LIKELY(cond) __builtin_expect(!!(cond), 1)

#define BOOST_STATIC_VIEWS_UNLIKELY(cond)                            \
    __builtin_expect(!!(cond), 0)

#define BOOST_STATIC_VIEWS_ASSUME(cond) __builtin_assume(!!(cond))

#define BOOST_STATIC_VIEWS_CURRENT_FUNCTION __PRETTY_FUNCTION__

#define BOOST_STATIC_VIEWS_UNREACHABLE __builtin_unreachable()

#elif defined(__GNUC__)
// ===========================================================================
// We're being compiled with GCC
#define BOOST_STATIC_VIEWS_GCC                                       \
    (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#define BOOST_STATIC_VIEWS_FORCEINLINE                               \
    inline __attribute__((__always_inline__))

#define BOOST_STATIC_VIEWS_NOINLINE __attribute__((__noinline__))

#define BOOST_STATIC_VIEWS_UNUSED __attribute__((__unused__))

#define BOOST_STATIC_VIEWS_NORETURN __attribute__((__noreturn__))

#define BOOST_STATIC_VIEWS_LIKELY(cond) __builtin_expect(!!(cond), 1)

#define BOOST_STATIC_VIEWS_UNLIKELY(cond)                            \
    __builtin_expect(!!(cond), 0)

#define BOOST_STATIC_VIEWS_ASSUME(cond)                              \
    ((!!(cond)) ? static_cast<void>(0) : __builin_unreachable())

#define BOOST_STATIC_VIEWS_CURRENT_FUNCTION __PRETTY_FUNCTION__

#define BOOST_STATIC_VIEWS_UNREACHABLE __builtin_unreachable()

#elif defined(_MSC_VER)
// ===========================================================================
// We're being compiled with Microsoft Visual C++
#define BOOST_STATIC_VIEWS_MSVC _MSC_VER

#define BOOST_STATIC_VIEWS_FORCEINLINE inline __forceinline

#define BOOST_STATIC_VIEWS_NOINLINE

#define BOOST_STATIC_VIEWS_UNUSED

#define BOOST_STATIC_VIEWS_NORETURN __declspec(noreturn)

#define BOOST_STATIC_VIEWS_LIKELY(cond) (cond)

#define BOOST_STATIC_VIEWS_UNLIKELY(cond) (cond)

#define BOOST_STATIC_VIEWS_ASSUME(cond) __assume(!!(cond))

#define BOOST_STATIC_VIEWS_CURRENT_FUNCTION __FUNCTION__

#define BOOST_STATIC_VIEWS_UNREACHABLE __assume(0)

#else
// clang-format off
#   error "Unsupported compiler. Please, submit a request to https://github.com/boostgsoc17/static-views/issues."
// clang-format on
// ===========================================================================
#endif

#endif // use Boost.Config

#define BOOST_STATIC_VIEWS_DO_JOIN2(X, Y) X##Y
#define BOOST_STATIC_VIEWS_DO_JOIN1(X, Y)                            \
    BOOST_STATIC_VIEWS_DO_JOIN2(X, Y)
#define BOOST_STATIC_VIEWS_JOIN(X, Y)                                \
    BOOST_STATIC_VIEWS_DO_JOIN1(X, Y)

#define BOOST_STATIC_VIEWS_STRINGIFY_DO_STRINGIFY(X) #X
#define BOOST_STATIC_VIEWS_STRINGIFY(X)                              \
    BOOST_STATIC_VIEWS_DO_STRINGIFY(X)

#if defined(DOXYGEN_IN_HOUSE)
// It's a bad idea to let Doxygen try deduce noexcept-ness.
#define BOOST_STATIC_VIEWS_NOEXCEPT_IF(...)                          \
    noexcept(whenever possible) /**/
#else

#if defined(BOOST_STATIC_VIEWS_DISABLE_CHECKS)                       \
    || !defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)
#define BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(...)                   \
    BOOST_STATIC_VIEWS_NOEXCEPT_IF(__VA_ARGS__)
#else
#define BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(...)
#endif

#define BOOST_STATIC_VIEWS_NOEXCEPT_IF(...) noexcept(__VA_ARGS__)
#endif

#if defined(DOXYGEN_IN_HOUSE)
// Breathe does not handle decltype(auto) correctly, see
// https://github.com/michaeljones/breathe/issues/330. Hence, trick it
// into thinking that the function returns a plain auto insted.
#define BOOST_STATIC_VIEWS_DECLTYPE_AUTO auto
#else
#define BOOST_STATIC_VIEWS_DECLTYPE_AUTO decltype(auto)
#endif

// Automatic noexcept deduction + trailing return type + body creation
#define BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN(...)             \
    BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(__VA_ARGS__))            \
        ->decltype(__VA_ARGS__)                                      \
    {                                                                \
        return (__VA_ARGS__);                                        \
    }                                                                \
    /* This is needed to make clang-format think we've just */       \
    /* declared a function. It then doesn't that annoying   */       \
    /* extra indent. */                                              \
    static_assert(true, "")

// Automatic noexcept deduction + body creation
#define BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(...)                 \
    BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(__VA_ARGS__))            \
    {                                                                \
        return (__VA_ARGS__);                                        \
    }                                                                \
    static_assert(true, "")

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \cond
// See
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html
template <class T>
constexpr T _static_const{};
/// \endcond

BOOST_STATIC_VIEWS_END_NAMESPACE

#if defined(BOOST_DISABLE_ASSERTS)
#define BOOST_STATIC_VIEWS_EXPECT(cond, msg)                         \
    BOOST_STATIC_VIEWS_ASSUME(cond)
#elif defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)

#include <exception>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \brief Exception that is thrown when an assert failure is
/// encountered.
class assert_failure : std::exception {
    char const* _msg;

  public:
    explicit assert_failure(char const* msg) noexcept : _msg{msg} {}

    auto what() const noexcept -> char const* override
    {
        return _msg;
    }
};

BOOST_STATIC_VIEWS_END_NAMESPACE

#define BOOST_STATIC_VIEWS_EXPECT(cond, msg)                         \
    ((BOOST_STATIC_VIEWS_LIKELY(!!(cond)))                           \
            ? static_cast<void>(0)                                   \
            : (throw assert_failure{                                 \
                  "Assertion failure in '" __FILE__}))

#elif defined(BOOST_STATIC_VIEWS_TERMINATE_ON_FAILURES)

#define BOOST_STATIC_VIEWS_EXPECT(cond, msg)                         \
    ((BOOST_STATIC_VIEWS_LIKELY(!!(cond))) ? static_cast<void>(0)    \
                                           : std::terminate())

#else

#error "No error handling pocily chosen."

#endif

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
