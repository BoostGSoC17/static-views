//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Include _all_ the headers

#ifndef BOOST_STATIC_VIEWS_STATIC_VIEWS_HPP
#define BOOST_STATIC_VIEWS_STATIC_VIEWS_HPP
#line 1 "static_views/algorithm_base.hpp"
//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP
#define BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP
#line 1 "detail/config.hpp"
//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_DETAIL_CONFIG_HPP
#define BOOST_STATIC_VIEWS_DETAIL_CONFIG_HPP

#include <cassert>
#include <cstddef>

// Define the following macro if you want to use StaticViews as part
// of Boost.
//#define BOOST_STATIC_VIEWS_USE_BOOST

// Define the following macro if when using Doxygen.
//#define DOXYGEN_IN_HOUSE

/// \brief Chooses whether to ignore the actual noexcept-ness of
/// std::tuple implementstion and make some reasonable assumptions
/// instead.
#define BOOST_STATIC_VIEWS_NEGLECT_STD_TUPLE

// #define BOOST_DISABLE_CHECKS
// #define BOOST_STATIC_VIEWS_THROW_ON_FAILURES
// #define BOOST_STATIC_VIEWS_TERMINATE_ON_FAILURES

#if !defined(BOOST_STATIC_VIEWS_DISABLE_CHECKS)                                \
    && !defined(BOOST_STATIC_VIEWS_TERMINATE_ON_FAILURES)                      \
    && !defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)

#define BOOST_STATIC_VIEWS_THROW_ON_FAILURES
#endif

// Easily turn off constexpr-ness to add some debug output, for
// example.
#define BOOST_STATIC_VIEWS_CONSTEXPR constexpr

/// \brief Disables all checks to achieve better performance
// #define BOOST_STATIC_VIEWS_DISABLE_CHECKS

// Boost.StaticViews namespace
#define BOOST_STATIC_VIEWS_NAMESPACE boost::static_views

#define BOOST_STATIC_VIEWS_BEGIN_NAMESPACE                                     \
    namespace boost {                                                          \
    namespace static_views {

#define BOOST_STATIC_VIEWS_END_NAMESPACE                                       \
    } /* static_views */                                                       \
    } /* boost */

#define BOOST_STATIC_VIEWS_BUG_MESSAGE                                         \
    "Congratulations, you've found a bug in the StaticViews "                  \
    "library! Please, be so kind to submit here "                              \
    "https://github.com/BoostGSoC17/static-views/issues."

#if defined(BOOST_STATIC_VIEWS_USE_BOOST)
//////////////////////////////////////////////////////////////////////////////
// We have Boost at our disposal
//////////////////////////////////////////////////////////////////////////////
#include <boost/config.hpp>

// Check for compilers
#if defined(BOOST_CLANG)
#define BOOST_STATIC_VIEWS_CLANG                                               \
    (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)

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
#define BOOST_STATIC_VIEWS_CLANG                                               \
    (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)

#define BOOST_STATIC_VIEWS_FORCEINLINE inline __attribute__((__always_inline__))

#define BOOST_STATIC_VIEWS_NOINLINE __attribute__((__noinline__))

#define BOOST_STATIC_VIEWS_UNUSED __attribute__((__unused__))

#define BOOST_STATIC_VIEWS_NORETURN __attribute__((__noreturn__))

#define BOOST_STATIC_VIEWS_LIKELY(cond) __builtin_expect(!!(cond), 1)

#define BOOST_STATIC_VIEWS_UNLIKELY(cond) __builtin_expect(!!(cond), 0)

#define BOOST_STATIC_VIEWS_ASSUME(cond) __builtin_assume(!!(cond))

#define BOOST_STATIC_VIEWS_CURRENT_FUNCTION __PRETTY_FUNCTION__

#define BOOST_STATIC_VIEWS_UNREACHABLE __builtin_unreachable()

#define BOOST_STATIC_VIEWS_PURE __attribute__((__pure__))

#elif defined(__GNUC__)
// ===========================================================================
// We're being compiled with GCC
#define BOOST_STATIC_VIEWS_GCC                                                 \
    (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#define BOOST_STATIC_VIEWS_FORCEINLINE inline __attribute__((__always_inline__))

#define BOOST_STATIC_VIEWS_NOINLINE __attribute__((__noinline__))

#define BOOST_STATIC_VIEWS_UNUSED __attribute__((__unused__))

#define BOOST_STATIC_VIEWS_NORETURN __attribute__((__noreturn__))

#define BOOST_STATIC_VIEWS_LIKELY(cond) __builtin_expect(!!(cond), 1)

#define BOOST_STATIC_VIEWS_UNLIKELY(cond) __builtin_expect(!!(cond), 0)

#define BOOST_STATIC_VIEWS_ASSUME(cond)                                        \
    ((!!(cond)) ? static_cast<void>(0) : __builtin_unreachable())

#define BOOST_STATIC_VIEWS_CURRENT_FUNCTION __PRETTY_FUNCTION__

#define BOOST_STATIC_VIEWS_UNREACHABLE __builtin_unreachable()

#define BOOST_STATIC_VIEWS_PURE __attribute__((__pure__))

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

#define BOOST_STATIC_VIEWS_PURE
#else
// clang-format off
#   error "Unsupported compiler. Please, submit a request to https://github.com/boostgsoc17/static-views/issues."
// clang-format on
// ===========================================================================
#endif

#endif // use Boost.Config

#define BOOST_STATIC_VIEWS_ISSUES_LINK                                         \
    "https://github.com/BoostGSoC17/static-views/issues"

#define BOOST_STATIC_VIEWS_DO_JOIN2(X, Y) X##Y
#define BOOST_STATIC_VIEWS_DO_JOIN1(X, Y) BOOST_STATIC_VIEWS_DO_JOIN2(X, Y)

#define BOOST_STATIC_VIEWS_JOIN(X, Y) BOOST_STATIC_VIEWS_DO_JOIN1(X, Y)

#define BOOST_STATIC_VIEWS_DO_STRINGIFY(X) #X
#define BOOST_STATIC_VIEWS_STRINGIFY(X) BOOST_STATIC_VIEWS_DO_STRINGIFY(X)

#if defined(DOXYGEN_IN_HOUSE)
// It's a bad idea to let Doxygen try deduce noexcept-ness.
#define BOOST_STATIC_VIEWS_NOEXCEPT_IF(...) noexcept(whenever possible) /**/

#else

#if defined(BOOST_STATIC_VIEWS_DISABLE_CHECKS)                                 \
    || !defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)

#define BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(...)                             \
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
#define BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN(...)                       \
    BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(__VA_ARGS__))                      \
        ->decltype(__VA_ARGS__)                                                \
    {                                                                          \
        return (__VA_ARGS__);                                                  \
    } /* This is needed to make clang-format think we've just */               \
    /* declared a function. It then doesn't that annoying   */ /* extra        \
                                                                  indent. */   \
    static_assert(true, "")
#line 235 "detail/config.hpp"
// Automatic noexcept deduction + body creation
#define BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(...)                           \
    BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(__VA_ARGS__))                      \
    {                                                                          \
        return (__VA_ARGS__);                                                  \
    }                                                                          \
    static_assert(true, "")

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \cond
// See
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html
template <class T>
constexpr T _static_const{};
/// \endcond

BOOST_STATIC_VIEWS_END_NAMESPACE

#if defined(BOOST_STATIC_VIEWS_DISABLE_CHECKS)
#define BOOST_STATIC_VIEWS_EXPECT(cond, msg) BOOST_STATIC_VIEWS_ASSUME(cond)

#elif defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)

#include <exception>
#include <stdexcept>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \brief Exception that is thrown when an assert failure is
/// encountered.
class assertion_failure : public virtual std::runtime_error {
  public:
    assertion_failure(char const* file, int const line, char const* cond)
        : std::runtime_error{std::string{"Assertion failure in '"} + file
                             + "' on line " + std::to_string(line)
                             + ": condition " + cond + " not satisfied."}
    {
    }
};

BOOST_STATIC_VIEWS_END_NAMESPACE

#define BOOST_STATIC_VIEWS_EXPECT(cond, msg)                                   \
    ((BOOST_STATIC_VIEWS_LIKELY(!!(cond)))                                     \
            ? static_cast<void>(0)                                             \
            : (throw assertion_failure{                                        \
                  __FILE__, __LINE__, BOOST_STATIC_VIEWS_STRINGIFY(cond)}))

#elif defined(BOOST_STATIC_VIEWS_TERMINATE_ON_FAILURES)

#define BOOST_STATIC_VIEWS_EXPECT(cond, msg)                                   \
    ((BOOST_STATIC_VIEWS_LIKELY(!!(cond))) ? static_cast<void>(0)              \
                                           : std::terminate())

#else

#error "No error handling policy chosen."

#endif

#if defined(DOXYGEN_IN_HOUSE)
#define BOOST_STATIC_VIEWS_INLINE_VARIABLE(type, name)                         \
    inline namespace {                                                         \
        constexpr auto const& name =                                           \
            ::BOOST_STATIC_VIEWS_NAMESPACE::_static_const<type>;               \
    } /**/

#else
#define BOOST_STATIC_VIEWS_INLINE_VARIABLE(type, name)                         \
    constexpr auto name = type{}; /**/

#endif

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \brief Special value of that indicates that the size of a sequence
/// is unknown at compile-time.
constexpr std::ptrdiff_t dynamic_extent = -1;

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DETAIL_CONFIG_HPP
#line 1 "detail/wrapper.hpp"
//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Implementation of boost::static_views::make_wrapper.

#ifndef BOOST_STATIC_VIEWS_DETAIL_WRAPPER_HPP
#define BOOST_STATIC_VIEWS_DETAIL_WRAPPER_HPP

#include <type_traits>
#line 1 "../concepts.hpp"
//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_CONCEPTS_HPP
#define BOOST_STATIC_VIEWS_CONCEPTS_HPP
#line 1 "detail/utils.hpp"
//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_DETAIL_UTILS_HPP
#define BOOST_STATIC_VIEWS_DETAIL_UTILS_HPP

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

constexpr auto all() noexcept -> bool { return true; }

template <class... Bools>
constexpr auto all(bool const x, Bools... xs) noexcept -> bool
{
    return x && all(xs...);
}

constexpr auto any() noexcept -> bool { return false; }

template <class... Bools>
constexpr auto any(bool const x, Bools... xs) noexcept -> bool
{
    return x || any(xs...);
}

} // namespace detail

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DETAIL_UTILS_HPP
#line 1 "detail/invoke.hpp"
//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Implementation of boost::static_views::invoke. It is a
// BOOST_STATIC_VIEWS_CONSTEXPR

// equivalent of std::invoke.

#ifndef BOOST_STATIC_VIEWS_DETAIL_INVOKE_HPP
#define BOOST_STATIC_VIEWS_DETAIL_INVOKE_HPP

#include <functional>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

/// \brief Trait that determines whether `T` is a reference_wrapper.
template <class T>
struct is_reference_wrapper : std::false_type {
};

/// \cond
template <class T>
struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type {
};
/// \endcond
#line 55 "detail/invoke.hpp"
// clang-format off


template <class Any, class Pointer, class Object, class... Args>
BOOST_STATIC_VIEWS_FORCEINLINE
constexpr
auto invoke_member_func_impl
    ( std::true_type /*is_base_of*/
    , Any            /*is_reference_wrapper*/
    , Pointer f
    , Object&& obj
    , Args&&... args )
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    (std::forward<Object>(obj).*f)(std::forward<Args>(args)...)
);

template <class Pointer, class Object, class... Args>
BOOST_STATIC_VIEWS_FORCEINLINE
constexpr
auto invoke_member_func_impl
    ( std::false_type /*is_base_of*/
    , std::true_type  /*is_reference_wrapper*/
    , Pointer f
    , Object&& obj
    , Args&&... args )
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    (std::forward<Object>(obj).get().*f)(std::forward<Args>(args)...)
);

template <class Pointer, class Object, class... Args>
BOOST_STATIC_VIEWS_FORCEINLINE
constexpr
auto invoke_member_func_impl
    ( std::false_type /*is_base_of*/
    , std::false_type /*is_reference_wrapper*/
    , Pointer f
    , Object&& obj
    , Args&&... args )
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    ((*std::forward<Object>(obj)).*f)(std::forward<Args>(args)...)
);

template <class Function, class T, class Object, class... Args>
BOOST_STATIC_VIEWS_FORCEINLINE
constexpr
auto invoke_member_func
    ( Function T::*f
    , Object&& obj
    , Args&&... args )
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    invoke_member_func_impl
        ( std::is_base_of<T, std::decay_t<Object>>{}
        , is_reference_wrapper<std::decay_t<Object>>{}
        , f
        , std::forward<Object>(obj)
        , std::forward<Args>(args)... )
);

template <class Any, class Pointer, class Object>
BOOST_STATIC_VIEWS_FORCEINLINE
constexpr
auto invoke_member_data_impl
    ( std::true_type /*is_base_of*/
    , Any            /*is_reference_wrapper*/
    , Pointer f
    , Object&& obj )
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    std::forward<Object>(obj).*f
);

template <class Pointer, class Object>
BOOST_STATIC_VIEWS_FORCEINLINE
constexpr
auto invoke_member_data_impl
    ( std::false_type /*is_base_of*/
    , std::true_type  /*is_reference_wrapper*/
    , Pointer f
    , Object&& obj )
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    std::forward<Object>(obj).get().*f
);

template <class Pointer, class Object>
BOOST_STATIC_VIEWS_FORCEINLINE
constexpr
auto invoke_member_data_impl
    ( std::false_type /*is_base_of*/
    , std::false_type /*is_reference_wrapper*/
    , Pointer f
    , Object&& obj )
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    (*std::forward<Object>(obj)).*f
);

template <class Function, class T, class Object>
BOOST_STATIC_VIEWS_FORCEINLINE
constexpr
auto invoke_member_data
    ( Function T::*f
    , Object&& obj )
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    invoke_member_data_impl
        ( std::is_base_of<T, std::decay_t<Object>>{}
        , is_reference_wrapper<std::decay_t<Object>>{}
        , f
        , std::forward<Object>(obj) )
);

template <class Function, class... Args>
BOOST_STATIC_VIEWS_FORCEINLINE
constexpr
auto invoke_nonmember
    ( Function&& f
    , Args&&... args )
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    std::forward<Function>(f)(std::forward<Args>(args)... )
);


struct invoke_impl {

  private:
    template <class Function, class T, class Object,
        class = std::enable_if_t<
            std::is_member_object_pointer<Function T::*>::value>>
    BOOST_STATIC_VIEWS_FORCEINLINE
    static constexpr
    auto call_impl(Function T::*f, Object&& obj)
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        invoke_member_data(f, std::forward<Object>(obj))
    );

    template <class Function, class T, class Object, class... Args,
        class = std::enable_if_t<
            std::is_member_function_pointer<Function T::*>::value>>
    BOOST_STATIC_VIEWS_FORCEINLINE
    static constexpr
    auto call_impl(Function T::*f, Object&& obj, Args&&... args)
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        invoke_member_func(f, std::forward<Object>(obj),
            std::forward<Args>(args)...)
    );

    template <class Function, class... Args,
        class = std::enable_if_t<
            !std::is_member_pointer<std::decay_t<Function>>::value>>
    BOOST_STATIC_VIEWS_FORCEINLINE
    static constexpr
    auto call_impl(Function&& f, Args&&... args)
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        invoke_nonmember(
            std::forward<Function>(f), std::forward<Args>(args)...)
    );

  public:
    template <class Function, class... Args>
    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr
    auto operator()(Function&& f, Args&&... args) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        call_impl(std::forward<Function>(f), std::forward<Args>(args)...)
    );
};

// clang-format on

} // end namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::invoke_impl, invoke)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DETAIL_INVOKE_HPP
#line 12 "../concepts.hpp"
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \typedef void_t
/// \verbatim embed:rst:leading-slashes
/// It is an alias to :cpp:`std::void_t` if available. Otherwise it is
/// defined as:
///
/// .. code-block:: cpp
///
///   template <class...>
///   using void_t = void;
/// \endverbatim
#if defined(__cpp_lib_void_t) && __cpp_lib_void_t >= 201411
using std::void_t;
#else
template <class...>
using void_t = void;
#endif

#if defined(__has_include) && __has_include(<experimental/type_traits>)
BOOST_STATIC_VIEWS_END_NAMESPACE
#include <experimental/type_traits>
BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

using nonesuch = std::experimental::nonesuch;

template <template <class...> class Op, class... Args>
using is_detected = std::experimental::is_detected<Op, Args...>;

template <template <class...> class Op, class... Args>
using detected_t = std::experimental::detected_t<Op, Args...>;

#else

/// \verbatim embed:rst:leading-slashes
/// NOP type. It is an alias to :cpp:`std::experimental::nonesuch` if
/// available. Otherwise it is defined as:
///
/// .. code-block:: cpp
///
///   struct nonesuch {
///       nonesuch()                = delete;
///       nonesuch(nonesuch const&) = delete;
///       nonesuch(nonesuch&&)      = delete;
///       nonesuch& operator=(nonesuch const&) = delete;
///       nonesuch& operator=(nonesuch&&) = delete;
///   };
/// \endverbatim
struct nonesuch {
    nonesuch()                = delete;
    nonesuch(nonesuch const&) = delete;
    nonesuch(nonesuch&&)      = delete;
    nonesuch& operator=(nonesuch const&) = delete;
    nonesuch& operator=(nonesuch&&) = delete;
};

namespace detail {
// clang-format off
template < class Default
         , class
         , template <class...> class Op
         , class... Args
         >
struct detector {
    using value_t = std::false_type;
    using type    = Default;
};

template < class Default
         , template <class...> class Op
         , class... Args
         >
struct detector<Default, void_t<Op<Args...>>, Op, Args...> {
    using value_t = std::true_type;
    using type    = Op<Args...>;
};
// clang-format on
} // namespace detail

/// \verbatim embed:rst:leading-slashes
/// An alias to :cpp:`std::experimental::is_detected` if
/// available. Otherwise it is implemented from scratch.
/// \endverbatim
template <template <class...> class Op, class... Args>
using is_detected =
    typename detail::detector<nonesuch, void, Op, Args...>::value_t;

/// \verbatim embed:rst:leading-slashes
/// An alias to :cpp:`std::experimental::is_detected` if
/// available. Otherwise it is implemented from scratch.
/// \endverbatim
template <template <class...> class Op, class... Args>
using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

#endif

#if defined(__cpp_lib_experimental_invocation_type)                            \
    && __cpp_lib_experimental_invocation_type >= 201411

// #if defined(__has_include) && __has_include(<experimental/type_traits>)

template <class Function, class... Args>
using is_invocable = std::is_invocable<Function, Args...>;

template <class Function, class... Args>
using is_nothrow_invocable = std::is_nothrow_invocable<Function, Args...>;

template <class Return, class Function, class... Args>
using is_invocable_r = std::is_invocable_r<Return, Function, Args...>;

template <class Return, class Function, class... Args>
using is_nothrow_invocable_r =
    std::is_nothrow_invocable_r<Return, Function, Args...>;

#else

namespace detail {

template <class Function, class... Args>
using invoke_t =
    decltype(invoke(std::declval<Function>(), std::declval<Args>()...));

} // namespace detail

template <class Fn, class... Xs>
using is_invocable = is_detected<detail::invoke_t, Fn, Xs...>;

template <class Fn, class... Xs>
using is_nothrow_invocable = std::conditional_t<
    detail::all(is_detected<detail::invoke_t, Fn, Xs...>::value,
        noexcept(invoke(std::declval<Fn>(), std::declval<Xs>()...))),
    std::true_type, std::false_type>;

template <class R, class Fn, class... Xs>
using is_invocable_r = std::conditional_t<
    detail::all(is_invocable<Fn, Xs...>::value,
        std::is_convertible<detected_t<detail::invoke_t, Fn, Xs...>, R>::value),
    std::true_type, std::false_type>;

template <class R, class Fn, class... Xs>
using is_nothrow_invocable_r = std::conditional_t<
    detail::all(is_nothrow_invocable<Fn, Xs...>::value,
        std::is_convertible<detected_t<detail::invoke_t, Fn, Xs...>, R>::value),
    std::true_type, std::false_type>;

#endif

#if defined(__cpp_concepts) && __cpp_concepts >= 201507
#define BOOST_STATIC_VIEWS_CONCEPTS
#endif

#if defined(BOOST_STATIC_VIEWS_CONCEPTS)
#define BOOST_STATIC_VIEWS_REQUIRES(...) > requires __VA_ARGS__
#else
#define BOOST_STATIC_VIEWS_REQUIRES(...)                                       \
    , class = std::enable_if_t<__VA_ARGS__>>

#endif

#if defined(__cpp_lib_is_swappable) && __cpp_lib_is_swappable >= 201603

// We have std::is_swappable, let's use it
using std::is_nothrow_swappable;
using std::is_nothrow_swappable_with;
using std::is_swappable;
using std::is_swappable_with;

#else

namespace detail {
namespace swap_adl {
    using std::swap;

    template <class T, class U>
    using swap_t = decltype(swap(std::declval<T>(), std::declval<U>()));

    template <class T, class U>
    using is_swappable_with =
        std::conditional_t<is_detected<swap_t, T, U>::value
                               && is_detected<swap_t, U, T>::value,
            std::true_type, std::false_type>;

    template <class T, class U, class = void>
    struct is_nothrow_swappable_with : std::false_type {
    };

    template <class T, class U>
    struct is_nothrow_swappable_with<T, U,
        std::enable_if_t<is_swappable_with<T, U>::value>>
        : std::conditional_t<
              noexcept(swap(std::declval<T>(), std::declval<U>()))
                  && noexcept(swap(std::declval<U>(), std::declval<T>())),
              std::true_type, std::false_type> {
    };
} // namespace swap_adl
} // namespace detail

using detail::swap_adl::is_nothrow_swappable_with;
using detail::swap_adl::is_swappable_with;

template <class T>
using is_swappable = is_swappable_with<T&, T&>;

template <class T>
using is_nothrow_swappable = is_nothrow_swappable_with<T&, T&>;

#endif

#if defined(BOOST_STATIC_VIEWS_CONCEPTS)
#if defined(__has_include) && __has_include(<concepts>)
#include <concepts>
#elif defined(__has_include) && __has_include(<experimental/concepts>)
#include <experimental/concepts>
#else
template <class T, class U>
concept bool Same = std::is_same<T, U>::value;

template <class From, class To>
concept bool ConvertibleTo = std::is_convertible<From, To>::value&& requires(
    From (&f)())
{
    static_cast<To>(f());
};

template <class T>
concept bool Destructible = std::is_nothrow_destructible<T>::value;

template <class T, class... Args>
concept bool          Constructible =
    Destructible<T>&& std::is_constructible<T, Args...>::value;

template <class T>
concept bool MoveConstructible = Constructible<T, T>&& ConvertibleTo<T, T>;

template <class T, class Dummy>
concept bool _MoveConstructible = MoveConstructible<T>;

template <class T>
concept bool Swappable = is_swappable<T>::value;
#endif
#else
namespace detail {
template <class From, class To>
using funny_extra_test_in_convertible_t =
    decltype(static_cast<To>(std::declval<From (&)()>()()));
} // namespace detail

template <class T, class U>
constexpr bool Same = std::is_same<T, U>::value;

template <class From, class To>
constexpr bool ConvertibleTo = std::is_convertible<From, To>::value&&
               is_detected<detail::funny_extra_test_in_convertible_t, From, To>::value;

template <class T>
constexpr bool Destructible = std::is_nothrow_destructible<T>::value;

template <class T, class... Args>
constexpr bool        Constructible =
    Destructible<T>&& std::is_constructible<T, Args...>::value;

template <class T>
constexpr bool MoveConstructible = Constructible<T, T>&& ConvertibleTo<T, T>;

template <class T, class Dummy>
constexpr bool _MoveConstructible = MoveConstructible<T>;

template <class T>
constexpr bool Swappable = is_swappable<T>::value;
#endif

namespace detail {
template <class T, int = (T::extent(), 0)>
static constexpr auto has_extent_impl(int) noexcept -> std::true_type
{
    return {};
}

template <class T>
static constexpr auto has_extent_impl(...) noexcept -> std::false_type
{
    return {};
}

template <class T, class = void>
struct HasExtent : std::false_type {
};

template <class T>
struct HasExtent<T, std::enable_if_t<has_extent_impl<T>(int{})>>
    : std::conditional_t<noexcept(T::extent())
                             && (T::extent() >= 0
                                    || T::extent() == dynamic_extent),
          std::true_type, std::false_type> {
};

template <class T>
using has_size_t = decltype(std::declval<T const&>().size());

template <class T>
using has_size_type_t = typename T::size_type;

template <class T, class = void>
struct HasSize : std::false_type {
};

template <class T>
struct HasSize<T, std::enable_if_t<is_detected<has_size_t, T>::value>>
    : std::conditional_t<noexcept(std::declval<T const&>().size()),
          std::true_type, std::false_type> {
};

template <class T>
using HasSizeWithType =
    std::conditional_t<HasSize<T>::value
                           && std::is_same<detected_t<has_size_t, T>,
                                  detected_t<has_size_type_t, T>>::value,
        std::true_type, std::false_type>;

template <class T, class IndexType>
using has_index_operator_t =
    decltype(std::declval<T>()[std::declval<IndexType>()]);

template <class T, class IndexType>
using HasIndexOperator = is_detected<has_index_operator_t, T, IndexType>;

template <class T, class = void, class = void, class = void>
struct HasIndexOperatorWithType : std::false_type {
};

template <class T>
struct HasIndexOperatorWithType<T, void_t<typename T::index_type>,
    void_t<typename T::reference>

    >
    : std::conditional_t<std::is_same<detected_t<has_index_operator_t, T&,
                                          typename T::index_type>,
                             typename T::reference>::value

          ,
          std::true_type, std::false_type> {
};

template <class T, class IndexType>
using has_unsafe_at_t =
    decltype(std::declval<T>().unsafe_at(std::declval<IndexType>()));

template <class T, class IndexType>
using HasUnsafeAt = is_detected<has_unsafe_at_t, T, IndexType>;

template <class T, class = void, class = void, class = void>
struct HasUnsafeAtWithType : std::false_type {
};

template <class T>
struct HasUnsafeAtWithType<T, void_t<typename T::index_type>,
    void_t<typename T::reference>

    >
    : std::conditional_t<
          std::is_same<detected_t<has_unsafe_at_t, T&, typename T::index_type>,
              typename T::reference>::value

          ,
          std::true_type, std::false_type> {
};

template <class T, class IndexType>
using has_map_t = decltype(std::declval<T>().map(std::declval<IndexType>()));

template <class T>
using HasMap = is_detected<has_map_t, T const&, typename T::index_type>;

} // namespace detail

#if defined(BOOST_STATIC_VIEWS_CONCEPTS)
// clang-format off
template <class T>
concept bool HasExtent = requires() {
    { T::extent() } noexcept -> std::ptrdiff_t;
    T::extent() >= 0 || T::extent() == dynamic_extent;
};
template <class T>
concept bool HasSize = requires(T const& xs) {
    { xs.size() } noexcept;
};
template <class T>
concept bool HasSizeWithType = requires(T const& xs) {
    { xs.size() } noexcept -> typename T::size_type;
};

// TODO: Still not sure whether Views should preserve const'ness. Not preserving
// const'ness makes my life a whole lot easier, so I'll stick to that for now :)

template <class T, class IndexType>
concept bool HasIndexOperator = requires(T& ref, T const& cref,
                                         IndexType i) {
    ref[i];
    // cref[i];
};
// TODO: This is buggy on gcc-7.3.0, but works on gcc-8
template <class T>
concept bool HasIndexOperatorWithType = requires(T& ref, typename T::index_type i) {
    { ref[i] } -> typename T::reference;
    // { cref[i] } -> typename T::const_reference;
};
template <class T, class IndexType>
concept bool HasUnsafeAt = requires(T& ref, T const& cref,
                                    IndexType i) {
    ref.unsafe_at(i);
    // cref.unsafe_at(i);
};
template <class T>
concept bool HasUnsafeAtWithType = requires(T& ref, T const& cref,
                                            typename T::index_type i) {
    { ref.unsafe_at(i) } -> typename T::reference;
    // { cref.unsafe_at(i) } -> typename T::const_reference;
};
template <class T>
concept bool HasMap = requires(T const& x, typename T::index_type i) {
    x.map(i);
};
// clang-format on
#else
template <class T>
constexpr bool HasExtent = detail::HasExtent<T>::value;
template <class T>
constexpr bool HasSize = detail::HasSize<T>::value;
template <class T>
constexpr bool HasSizeWithType = detail::HasSizeWithType<T>::value;
template <class T, class IndexType>
constexpr bool HasIndexOperator = detail::HasIndexOperator<T, IndexType>::value;
template <class T>
constexpr bool HasIndexOperatorWithType =
    detail::HasIndexOperatorWithType<T>::value;
template <class T, class IndexType>
constexpr bool HasUnsafeAt = detail::HasUnsafeAt<T, IndexType>::value;
template <class T>
constexpr bool HasUnsafeAtWithType = detail::HasUnsafeAtWithType<T>::value;
template <class T>
constexpr bool HasMap = detail::HasMap<T>::value;
#endif

template <class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_CONCEPTS_HPP
#line 16 "detail/wrapper.hpp"
BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \brief Smart reference to ease the lifetime management stuff.
template <class T>
class wrapper;

template <class T>
struct is_wrapper : std::false_type {
};

template <class T>
struct is_wrapper<wrapper<T>> : std::true_type {
};

namespace detail {

template <class T, bool IsOwner, bool IsEmpty, class = void>
class wrapper_storage;

template <class T, bool IsEmpty>
class wrapper_storage<T, /*am I the owner of T?*/ false, IsEmpty> {

    static_assert(std::is_object<T>::value, BOOST_STATIC_VIEWS_BUG_MESSAGE);

  public:
    using value_type = T;
    using reference  = std::add_lvalue_reference_t<value_type>;
    using const_reference =
        std::add_lvalue_reference_t<std::add_const_t<value_type>>;

    explicit constexpr wrapper_storage(reference x) noexcept : _payload{&x} {}

    wrapper_storage(wrapper_storage const& other) noexcept = default;
    wrapper_storage(wrapper_storage&&) noexcept            = default;
    wrapper_storage& operator=(wrapper_storage const& other) noexcept = default;
    wrapper_storage& operator=(wrapper_storage&&) noexcept = default;

    constexpr auto const& get() const noexcept { return *_payload; }

    constexpr auto& get() noexcept { return *_payload; }

    constexpr explicit operator reference() noexcept { return get(); }

    constexpr explicit operator const_reference() const noexcept
    {
        return get();
    }

  private:
    value_type* _payload;
};

template <class T>
class wrapper_storage<T, /*am I the owner of T?*/ true,
    /*is T empty and non-final?*/ true> : public T {

    static_assert(std::is_object<T>::value, BOOST_STATIC_VIEWS_BUG_MESSAGE);
    static_assert(std::is_empty<T>::value && !std::is_final<T>::value,
        BOOST_STATIC_VIEWS_BUG_MESSAGE);

    using base = T;

  public:
    using value_type = T;
    using reference  = std::add_lvalue_reference_t<value_type>;
    using const_reference =
        std::add_lvalue_reference_t<std::add_const_t<value_type>>;

    // clang-format off
    template <class U
        BOOST_STATIC_VIEWS_REQUIRES(Constructible<base, U>)
    constexpr
    wrapper_storage(U&& x)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_constructible<base, U>::value)
        : base{std::forward<U>(x)}
    {
    }
    // clang-format on

    wrapper_storage(wrapper_storage const& other) = default;
    wrapper_storage(wrapper_storage&&)            = default;
    wrapper_storage& operator=(wrapper_storage const& other) = default;
    wrapper_storage& operator=(wrapper_storage&&) = default;

    constexpr auto const& get() const& noexcept
    {
        return static_cast<base const&>(*this);
    }

    constexpr auto& get() & noexcept { return static_cast<base&>(*this); }

    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(_MoveConstructible<value_type, Dummy>)
    constexpr
    auto get() && BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                     std::is_nothrow_move_constructible<base>::value)
    {
        return static_cast<base&&>(*this);
    }
    // clang-format on

    constexpr explicit operator reference() & noexcept { return get(); }

    constexpr explicit operator const_reference() const& noexcept
    {
        return get();
    }
};

template <class T>
class wrapper_storage<T, /*am I the owner of T?*/ true,
    /*is T empty and non-final?*/ false> {

    static_assert(std::is_object<T>::value, BOOST_STATIC_VIEWS_BUG_MESSAGE);
    static_assert(!std::is_empty<T>::value || std::is_final<T>::value,
        BOOST_STATIC_VIEWS_BUG_MESSAGE);

  public:
    using value_type = T;
    using reference  = std::add_lvalue_reference_t<value_type>;
    using const_reference =
        std::add_lvalue_reference_t<std::add_const_t<value_type>>;

    // clang-format off
    template <class U
        BOOST_STATIC_VIEWS_REQUIRES(Constructible<value_type, U>)
    explicit constexpr wrapper_storage(U&& x)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_constructible<value_type, U>::value)
        : _payload{std::forward<U>(x)}
    {
    }
    // clang-format on

    wrapper_storage(wrapper_storage const&) = default;
    wrapper_storage(wrapper_storage&&)      = default;
    wrapper_storage& operator=(wrapper_storage const&) = default;
    wrapper_storage& operator=(wrapper_storage&&) = default;

    constexpr auto const& get() const& noexcept { return _payload; }

    constexpr auto& get() & noexcept { return _payload; }

    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(_MoveConstructible<value_type, Dummy>)
    constexpr
    auto get() && BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                      std::is_nothrow_move_constructible<value_type>::value)
    {
        return std::move(_payload);
    }
    // clang-format on

    constexpr explicit operator reference() & noexcept { return get(); }

    constexpr explicit operator const_reference() const& noexcept
    {
        return get();
    }

    // NOTE: This should instead be
    // explicit operator value_type() && ...
    // but buggy GCC disagrees :)
    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(_MoveConstructible<value_type, Dummy>)
    constexpr
    explicit operator value_type&&() &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<value_type>::value)
    {
        return std::move(_payload);
    }
    // clang-format on

  private:
    value_type _payload;
};

template <class T>
using wrapper_base_t = wrapper_storage<std::remove_reference_t<T>,
    !std::is_lvalue_reference<T>::value,
    std::is_empty<std::remove_reference_t<T>>::value
        && !std::is_final<std::remove_reference_t<T>>::value>;

} // namespace detail

template <class T>
class wrapper : public detail::wrapper_base_t<T> {

    static_assert(!std::is_rvalue_reference<T>::value,
        "boost::static_views::wrapper: Please, do not use rvalue "
        "references as template parameters. If you want a `wrapper` "
        "to own an object of type `T`, use `wrapper<T>` instead.");

    static_assert(!std::is_void<std::remove_reference_t<T>>::value,
        "boost::static_views::wrapper: `void` is not supported. If "
        "you have a problem with that, please, submit an issue "
        "here " BOOST_STATIC_VIEWS_ISSUES_LINK ".");

    using base = detail::wrapper_base_t<T>;

  public:
    using typename base::const_reference;
    using typename base::reference;
    using typename base::value_type;

    // clang-format off
    template <class U
        BOOST_STATIC_VIEWS_REQUIRES(Constructible<base, U>)
    constexpr
    wrapper(U&& x) BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                       std::is_nothrow_constructible<base, U>::value)
        : base{std::forward<U>(x)}
    {
    }
    // clang-format on

    wrapper(wrapper const&) = default;
    wrapper(wrapper&&)      = default;
    wrapper& operator=(wrapper const&) = default;
    wrapper& operator=(wrapper&&) = default;
};

namespace detail {

struct make_wrapper_impl {
    // clang-format off
    template <class T
        BOOST_STATIC_VIEWS_REQUIRES(
            std::is_lvalue_reference<T>::value || MoveConstructible<T>)
    constexpr
    auto operator()(T&& x) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::is_nothrow_constructible<wrapper<T>, T>::value))
    // clang-format on
    {
        return wrapper<T>{std::forward<T>(x)};
    }
};

} // end namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_wrapper_impl, make_wrapper)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DETAIL_WRAPPER_HPP
#line 1 "view_base.hpp"
//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Implementation of boost::static_views::view_base,
// boost::static_views::is_view,
// boost::static_views::view_adaptor_core_access and
// boost::static_views::view_adaptor_base.

#ifndef BOOST_STATIC_VIEWS_VIEW_BASE_HPP
#define BOOST_STATIC_VIEWS_VIEW_BASE_HPP
#line 1 "errors.hpp"
//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_ERRORS_HPP
#define BOOST_STATIC_VIEWS_ERRORS_HPP

#include <stdexcept>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \brief Exception that is thrown when an "index out of bounds
/// error" is encountered.
class out_of_bound : public virtual std::runtime_error {
  public:
    out_of_bound() : std::runtime_error{"Index out of bounds."} {}
    using std::runtime_error::runtime_error;
};

/// \brief Exception that is thrown when an insert into a full bucket
/// is attempted.
class full_bucket : public virtual std::runtime_error {
  public:
    full_bucket() : std::runtime_error{"Bucket is full."} {}
    using std::runtime_error::runtime_error;
};

namespace detail {
BOOST_STATIC_VIEWS_NORETURN
auto make_out_of_bound() -> void { throw out_of_bound{}; }

BOOST_STATIC_VIEWS_NORETURN
auto make_out_of_bound(char const* msg) -> void { throw out_of_bound{msg}; }

BOOST_STATIC_VIEWS_NORETURN
auto make_full_bucket() -> void { throw full_bucket{}; }

BOOST_STATIC_VIEWS_NORETURN
auto make_full_bucket(char const* msg) -> void { throw full_bucket{msg}; }
} // end namespace detail

// clang-format off
void (*make_out_of_bound_error)(char const*) = &detail::make_out_of_bound;
// void (*make_invalid_range_error)(char const*) = &detail::make_invalid_range;
void (*make_full_bucket_error)(char const*) = &detail::make_full_bucket;
// clang-format on

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_ERRORS_HPP
#line 1 "iterator.hpp"
//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_ITERATOR_HPP
#define BOOST_STATIC_VIEWS_ITERATOR_HPP
#line 1 "view_concept.hpp"
//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_VIEW_CONCEPT_HPP
#define BOOST_STATIC_VIEWS_VIEW_CONCEPT_HPP

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

#if defined(BOOST_STATIC_VIEWS_CONCEPTS)
template <class T>
concept bool View = MoveConstructible<T>&& HasExtent<T>&& HasSizeWithType<T>&&
    HasIndexOperatorWithType<T>&& HasUnsafeAtWithType<T>;
#else
template <class T>
constexpr bool View = MoveConstructible<T>&& HasExtent<T>&& HasSizeWithType<T>&&
    HasIndexOperatorWithType<T>&& HasUnsafeAtWithType<T>;
#endif

template <class V>
constexpr auto why_is_my_argument_not_a_view() -> void
{
    static_assert(!View<V>, BOOST_STATIC_VIEWS_BUG_MESSAGE);

    static_assert(View<V>,
        "Type `V` does not model the View concept, because: (read "
        "on for more)");
    static_assert(MoveConstructible<V>, "`V` is not move-constructible.");
    static_assert(HasExtent<V>,
        "`V` does not have a `constexpr` `static` non-throwing "
        "member function `extent()` returning a value >= 0 or "
        "`boost::static_views::dynamic_extent`.");
    static_assert(HasSizeWithType<V>,
        "`V` does not have a `const` non-throwing member function `size()` "
        "returning a `V::size_type`.");
    static_assert(HasIndexOperatorWithType<V>,
        "`V` does not have an `operator[](V::index_type)` returning a "
        "`V::reference`.");
    static_assert(HasUnsafeAtWithType<V>,
        "`V` does not have a member function `unsafe_at(V::index_type)` "
        "returning a `V::reference`.");
}

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_VIEW_CONCEPT_HPP
#line 11 "iterator.hpp"
#include <iterator>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

template <class V BOOST_STATIC_VIEWS_REQUIRES(View<V>) struct view_iterator {

    using view_type         = V;
    using size_type         = typename V::size_type;
    using difference_type   = typename V::index_type;
    using value_type        = typename V::value_type;
    using reference         = typename V::reference;
    using pointer           = value_type*;
    using iterator_category = std::random_access_iterator_tag;

  private:
    view_type*      _view;
    difference_type _i;

    constexpr auto is_dereferenceable() const noexcept -> bool
    {
        return _view != nullptr && _i >= 0
               && static_cast<size_type>(_i) <= _view->size();
    }

    static constexpr auto unsafe_at_is_noexcept() noexcept -> bool
    {
        return noexcept(std::declval<view_type&>().unsafe_at(
            std::declval<difference_type>()));
    }

  public:
    constexpr explicit view_iterator(view_type* const view = nullptr,
        difference_type const i = 0) BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
        : _view{view}, _i{i}
    {
        BOOST_STATIC_VIEWS_EXPECT(
            (view == nullptr && i == 0)
                || (view != nullptr && 0 <= i
                       && static_cast<size_type>(i) <= view->size()),
            "It's a bad idea to create an iterator pointing neither "
            "into the view nor to the one-past-the-end element.");
    }

    view_iterator(view_iterator const&) = default;
    view_iterator(view_iterator&&)      = default;
    view_iterator& operator=(view_iterator const&) = default;
    view_iterator& operator=(view_iterator&&) = default;

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto* operator-> () const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(unsafe_at_is_noexcept())
    {
        BOOST_STATIC_VIEWS_EXPECT(
            is_dereferenceable(), "Nah, this iterator is not dereferenceable.");
        return &_view->unsafe_at(_i);
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator*() const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(unsafe_at_is_noexcept())
    {
        BOOST_STATIC_VIEWS_EXPECT(
            is_dereferenceable(), "Nah, this iterator is not dereferenceable.");
        return _view->unsafe_at(_i);
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](
        difference_type const n) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<view_iterator const&>().operator*()))
    {
        return *(*this + n);
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto operator++() noexcept -> view_iterator&
    {
        ++_i;
        return *this;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto operator--() noexcept -> view_iterator&
    {
        --_i;
        return *this;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto operator++(int) noexcept -> view_iterator
    {
        view_iterator temp{*this};
        ++(*this);
        return temp;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto operator--(int) noexcept -> view_iterator
    {
        view_iterator temp{*this};
        --(*this);
        return temp;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto operator+=(difference_type const n) noexcept
        -> view_iterator&
    {
        _i += n;
        return *this;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto operator-=(difference_type const n) noexcept
        -> view_iterator&
    {
        _i -= n;
        return *this;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr friend auto operator+(
        view_iterator x, difference_type const n) noexcept -> view_iterator
    {
        return x += n;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr friend auto operator-(
        view_iterator x, difference_type const n) noexcept -> view_iterator
    {
        return x -= n;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr friend auto operator+(
        difference_type const n, view_iterator x) noexcept -> view_iterator
    {
        return x += n;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr friend auto operator-(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> difference_type
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to subtract "
            "iterators to different views.");
        return x._i - y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr friend auto operator==(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare "
            "iterators to different views.");
        return x._i == y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr friend auto operator!=(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare "
            "iterators to different views.");
        return x._i != y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr friend auto operator<(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare "
            "iterators to different views.");
        return x._i < y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr friend auto operator>(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare iterators to different "
            "objects.");
        return x._i > y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr friend auto operator<=(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare "
            "iterators to different views.");
        return x._i <= y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr friend auto operator>=(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare "
            "iterators to different views.");
        return x._i >= y._i;
    }
};

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_ITERATOR_HPP
#line 20 "view_base.hpp"
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE
#line 57 "view_base.hpp"
// clang-format off
template <class T
    BOOST_STATIC_VIEWS_REQUIRES(View<T>)
constexpr auto begin(T const& xs)
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    view_iterator<T const>{&xs, 0}
);

template <class T
    BOOST_STATIC_VIEWS_REQUIRES(View<T>)
constexpr auto begin(T& xs)
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    view_iterator<T>{&xs, 0}
);

template <class T
    BOOST_STATIC_VIEWS_REQUIRES(View<T>)
constexpr auto end(T const& xs)
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    view_iterator<T const>{&xs, static_cast<typename view_iterator<T const>::difference_type>(xs.size())}
);

template <class T
    BOOST_STATIC_VIEWS_REQUIRES(View<T>)
constexpr auto end(T& xs)
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    view_iterator<T>{&xs, static_cast<typename view_iterator<T>::difference_type>(xs.size())}
);
// clang-format on

/// \brief
/// \verbatim embed:rst:leading-slashes
/// Base class to that helps with modeling the :ref:`View
/// <view-concept>` concept. \endverbatim
template <class Derived, class Wrapper>
struct view_adaptor_base : private Wrapper {

  private:
    using derived_type = Derived;
    using wrapper_type = Wrapper;
    using view_type    = typename Wrapper::value_type;

    template <class Dummy>
    struct traits {
        using derived = derived_type;
    };

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto derived() const & noexcept -> derived_type const&
    {
        return *static_cast<derived_type const*>(this);
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto derived() && noexcept -> derived_type&&
    {
        return static_cast<derived_type&&>(*this);
    }

  protected:
    using view_adaptor_base_type = view_adaptor_base<Derived, Wrapper>;

  public:
    using value_type = typename view_type::value_type;
    using reference  = typename view_type::reference;
    using size_type  = typename view_type::size_type;
    using index_type = typename view_type::index_type;

    /// \name Constructors
    ///
    /// \brief Constructs an adaptor of \p view.
    /// \details Here, \p view is a wrapper around the actual view
    /// created with #make_wrapper(T&&) function.
    constexpr view_adaptor_base(wrapper_type&& view)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<wrapper_type>::value)
        : wrapper_type{std::move(view)}
    {
    }

    /// \name Copy and move constructors/assignments
    ///
    /// Defines default copy and move constructors and assignments,
    /// i.e. is copy/move-constructible/assignable if \p View is.
    /// \{
    view_adaptor_base(view_adaptor_base const&) = default;
    view_adaptor_base(view_adaptor_base&&)      = default;
    view_adaptor_base& operator=(view_adaptor_base const&) = default;
    view_adaptor_base& operator=(view_adaptor_base&&) = default;
    /// \}

    /// \brief Returns the underlying view.
    /// \{
    BOOST_STATIC_VIEWS_PURE
    constexpr auto const& parent() const& noexcept
    {
        static_assert(noexcept(this->get()), BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return this->get();
    }

    constexpr auto parent()
        && BOOST_STATIC_VIEWS_NOEXCEPT_IF(
               std::is_nothrow_move_constructible<wrapper_type>::value)
    {
        return static_cast<wrapper_type&&>(*this).get();
    }
    /// \}

    /// \brief
    /// \verbatim embed:rst:leading-slashes
    /// Default implementation of the ``capacity`` function required
    /// by the :ref:`view concept <view-concept>`. \endverbatim

    static constexpr auto extent() noexcept { return view_type::extent(); }

    /// \brief
    /// \verbatim embed:rst:leading-slashes
    /// Default implementation of the ``size`` function required by
    /// the :ref:`view concept <view-concept>`. \endverbatim

    /// Just calls `size()` on the underlying view.
    BOOST_STATIC_VIEWS_PURE
    constexpr auto size() const noexcept
    {
        static_assert(noexcept(parent()), BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return parent().size();
    }

    /// \name Element access
    /// \{

    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(HasMap<typename traits<Dummy>::derived>)
    constexpr
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO unsafe_at(index_type const i) const&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<view_type const&>().unsafe_at(
                std::declval<derived_type const&>().map(i))))
    // clang-format on
    {
        static_assert(noexcept(parent()) && noexcept(derived()),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return parent().unsafe_at(derived().map(i));
    }

    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(HasMap<typename traits<Dummy>::derived>)
    constexpr
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO unsafe_at(index_type const i) &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<view_type&&>().unsafe_at(
                std::declval<derived_type const&>().map(i))))
    // clang-format on
    {
        static_assert(noexcept(parent()) && noexcept(derived()),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return static_cast<wrapper_type&&>(*this).get().unsafe_at(
            derived().map(i));
    }

    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(
            HasUnsafeAt<typename traits<Dummy>::derived const&, index_type>)
    constexpr
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](index_type const i) const&
    // clang-format on
    {
        if (BOOST_STATIC_VIEWS_UNLIKELY(
                i >= static_cast<index_type>(derived().size()))) {
            make_out_of_bound_error(
                "Precondition `i < size()` not satisfied in "
                "boost::static_views::view_base::operator[].");
            BOOST_STATIC_VIEWS_UNREACHABLE;
        }
        return derived().unsafe_at(i);
    }

    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(
            HasUnsafeAt<typename traits<Dummy>::derived, index_type>)
    constexpr
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](index_type const i) &&
    // clang-format on
    {
        if (BOOST_STATIC_VIEWS_UNLIKELY(
                i >= static_cast<index_type>(derived().size()))) {
            make_out_of_bound_error(
                "Precondition `i < size()` not satisfied in "
                "boost::static_views::view_base::operator[].");
            BOOST_STATIC_VIEWS_UNREACHABLE;
        }
        return static_cast<derived_type&&>(*this).unsafe_at(i);
    }
    /// \}

    constexpr auto begin() const BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
        ::boost::static_views::begin(std::declval<derived_type const&>())))
    {
        return ::boost::static_views::begin(derived());
    }

    constexpr auto end() const BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
        ::boost::static_views::end(std::declval<derived_type const&>())))
    {
        return ::boost::static_views::end(derived());
    }
};

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_VIEW_BASE_HPP
#line 12 "static_views/algorithm_base.hpp"
#include <tuple>
#include <type_traits>
#include <utility>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
template <class Function, class... Args>
struct lazy_adaptor_impl : private std::tuple<Function, Args...> {

    using base_type = std::tuple<Function, Args...>;

    template <class Dummy>
    struct traits {
        using base_type = lazy_adaptor_impl::base_type;
    };

    template <class... Bools>
    static constexpr auto all_of(Bools... xs) noexcept -> bool
    {
        constexpr auto n             = sizeof...(Bools);
        bool           conditions[n] = {xs...};
        bool           all           = true;
        for (std::size_t i = 0; i < n; ++i) {
            all = all && conditions[i];
        }
        return all;
    }

    // The following functions kind of try to "fix" std::tuple.

    static constexpr auto is_base_nothrow_move_constructible() noexcept -> bool
    {
        return all_of(std::is_nothrow_move_constructible<Function>::value,
            std::is_nothrow_move_constructible<Args>::value...);
    }

    static constexpr auto is_base_nothrow_copy_constructible() noexcept -> bool
    {
        return all_of(std::is_nothrow_copy_constructible<Function>::value,
            std::is_nothrow_copy_constructible<Args>::value...);
    }

    static constexpr auto is_base_nothrow_move_assignable() noexcept -> bool
    {
        return all_of(std::is_nothrow_move_assignable<Function>::value,
            std::is_nothrow_move_assignable<Args>::value...);
    }

    static constexpr auto is_base_nothrow_copy_assignable() noexcept -> bool
    {
        return all_of(std::is_nothrow_copy_assignable<Function>::value,
            std::is_nothrow_copy_assignable<Args>::value...);
    }

    constexpr auto const& base() const& noexcept
    {
        return static_cast<base_type const&>(*this);
    }

    constexpr auto& base() & noexcept { return static_cast<base_type&>(*this); }

    constexpr auto base()
        && BOOST_STATIC_VIEWS_NOEXCEPT_IF(is_base_nothrow_move_constructible())
    {
        return static_cast<base_type&&>(*this);
    }

    template <class Base, class View, std::size_t... Is>
    static constexpr auto is_call_noexcept(
        std::index_sequence<Is...> /*unused*/) noexcept -> bool
    {
        static_assert(
            sizeof...(Args) == sizeof...(Is), BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return noexcept(invoke(std::get<0>(std::declval<Base>()),
            std::declval<View>(), std::get<Is + 1>(std::declval<Base>())...));
    }

    template <class Base, class View>
    static constexpr auto is_call_noexcept() noexcept -> bool
    {
        return is_call_noexcept<Base, View>(
            std::make_index_sequence<sizeof...(Args)>{});
    }

    // clang-format off
    template <class V, std::size_t... Is>
    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr
    decltype(auto) call_impl(V&& xs, std::index_sequence<Is...> /*unused*/)
        // clang-format on
        const& BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            is_call_noexcept<base_type const&, V>())
    {
        static_assert(
            sizeof...(Is) == sizeof...(Args), BOOST_STATIC_VIEWS_BUG_MESSAGE);
        static_assert(View<std::remove_cv_t<std::remove_reference_t<V>>>,
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return invoke(std::get<0>(base()), std::forward<V>(xs),
            std::get<Is + 1>(base())...);
    }

    // clang-format off
    template <class V, std::size_t... Is>
    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr
    decltype(auto) call_impl(V&& xs, std::index_sequence<Is...> /*unused*/) &&
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(is_call_noexcept<base_type&&, V>())
    {
        static_assert(
            sizeof...(Is) == sizeof...(Args), BOOST_STATIC_VIEWS_BUG_MESSAGE);
        static_assert(View<std::remove_cv_t<std::remove_reference_t<V>>>,
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return invoke(
            std::get<0>(static_cast<lazy_adaptor_impl&&>(*this).base()),
            std::forward<V>(xs),
            std::get<Is + 1>(
                std::move(static_cast<lazy_adaptor_impl&&>(*this)))...);
    }

  public:
    constexpr lazy_adaptor_impl(Function f, Args... args)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(is_base_nothrow_move_constructible())
        : base_type{std::move(f), std::move(args)...}
    {
    }

    // It's a real pity we can't simply "default" these...
    // The reason we can't is that std::tuple is not very noexcept-friendly.

    // clang-format off
    template <class Dummy
        BOOST_STATIC_VIEWS_REQUIRES(
            std::is_copy_constructible<
                typename traits<Dummy>::base_type>::value)
    constexpr
    lazy_adaptor_impl(lazy_adaptor_impl const& other)
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            is_base_nothrow_copy_constructible()) : base_type{other.base()}
    {
    }

    // clang-format off
    // TODO: Do we need this? The first constructor already assumes that
    // Function and Args... are move-constructible.
    template <class Dummy
        BOOST_STATIC_VIEWS_REQUIRES(
            std::is_move_constructible<
                typename traits<Dummy>::base_type>::value)
    constexpr
    lazy_adaptor_impl(lazy_adaptor_impl&& other)
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            is_base_nothrow_move_constructible()) : base_type{
            std::move(other).base()}
    {
    }

    // clang-format off
    template <class Dummy
        BOOST_STATIC_VIEWS_REQUIRES(
            std::is_copy_assignable<
                typename traits<Dummy>::base_type>::value)
    constexpr
    lazy_adaptor_impl& operator=(lazy_adaptor_impl const& other)
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(is_base_nothrow_copy_assignable())
    {
        base() = other.base();
        return *this;
    }

    // clang-format off
    template <class Dummy
        BOOST_STATIC_VIEWS_REQUIRES(
            std::is_move_assignable<
                typename traits<Dummy>::base_type>::value)
    constexpr
    lazy_adaptor_impl& operator=(lazy_adaptor_impl&& other)
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(is_base_nothrow_move_assignable())
    {
        base() = std::move(other).base();
        return *this;
    }

    // clang-format off
    template <class V, class = void
        BOOST_STATIC_VIEWS_REQUIRES(
            !View<std::remove_cv_t<std::remove_reference_t<V>>>)
    constexpr
    decltype(auto) operator()(V&& /*unused*/) const noexcept
    // clang-format on
    {
        return why_is_my_argument_not_a_view<
            std::remove_cv_t<std::remove_reference_t<V>>>();
    }

    // clang-format off
    template <class V
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>)
    constexpr
    decltype(auto) operator()(V&& xs) const&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<lazy_adaptor_impl const&>().call_impl(
                std::forward<V>(xs),
                std::make_index_sequence<sizeof...(Args)>{})))
    // clang-format on
    {
        return call_impl(
            std::forward<V>(xs), std::make_index_sequence<sizeof...(Args)>{});
    }

    // clang-format off
    template <class V
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>)
    constexpr
    decltype(auto) operator()(V&& xs) &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<lazy_adaptor_impl&&>().call_impl(
                std::forward<V>(xs),
                std::make_index_sequence<sizeof...(Args)>{})))
    // clang-format on
    {
        return static_cast<lazy_adaptor_impl&&>(*this).call_impl(
            std::forward<V>(xs), std::make_index_sequence<sizeof...(Args)>{});
    }
};

struct make_lazy_adaptor_impl {
    // clang-format off
    template <class Function, class... Args>
    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr
    auto operator()(Function&& fn, Args&&... args) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        lazy_adaptor_impl<std::decay_t<Function>,
            Args...>(std::forward<Function>(fn),
                std::forward<Args>(args)...)
    );
    // clang-format on
};

} // namespace detail

/*
#define BOOST_STATIC_VIEWS_INLINE_ALGO_VARIABLE(type, name)              inline
namespace { BOOST_STATIC_VIEWS_CONSTEXPR auto const& name =
::BOOST_STATIC_VIEWS_NAMESPACE::_static_const<
::BOOST_STATIC_VIEWS_NAMESPACE::detail:: make_algorithm_impl<type>>; }






*/
#line 276 "static_views/algorithm_base.hpp"
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_lazy_adaptor_impl, lazy_adaptor)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP
#line 1 "static_views/chunk.hpp"
//          Copyright Tom Westerhout 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/chunk.hpp
///
/// \brief Implementation of #chunk.

#ifndef BOOST_STATIC_VIEWS_CHUNK_HPP
#define BOOST_STATIC_VIEWS_CHUNK_HPP
#line 1 "drop.hpp"
//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/drop.hpp
///
/// \brief Implementation of #drop.

#ifndef BOOST_STATIC_VIEWS_DROP_HPP
#define BOOST_STATIC_VIEWS_DROP_HPP
#line 1 "compact_index.hpp"
//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_COMPACT_INDEX_HPP
#define BOOST_STATIC_VIEWS_COMPACT_INDEX_HPP

#include <algorithm>
#include <type_traits>
#include <utility>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

template <class Tag, class IndexType, std::ptrdiff_t Extent = dynamic_extent,
    class = void>
struct compact_index {
  private:
    IndexType _payload;

    static_assert(std::is_integral<IndexType>::value,
        "boost::static_views::detail::compact_index is meant to be used "
        "with integral types.");

  public:
    constexpr compact_index(IndexType const index) noexcept : _payload{index}
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
    constexpr auto index() const noexcept { return _payload; }

    BOOST_STATIC_VIEWS_PURE
    constexpr operator IndexType() const noexcept { return index(); }
};

template <class Tag, class IndexType, std::ptrdiff_t Extent>
struct compact_index<Tag, IndexType, Extent, std::enable_if_t<Extent >= 0>> {
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
    constexpr auto index() const noexcept
    {
        return static_cast<IndexType>(Extent);
    }

    static constexpr auto as_integral_constant() noexcept
    {
        return std::integral_constant<IndexType, Extent>{};
    }

    BOOST_STATIC_VIEWS_PURE
    constexpr operator IndexType() const noexcept { return index(); }
};

// clang-format off
template <class Tag = void, class IndexType = int
    BOOST_STATIC_VIEWS_REQUIRES(std::is_integral<IndexType>::value)
constexpr auto index(IndexType const x) noexcept
// clang-format on
{
    return compact_index<Tag, IndexType>{x};
}

template <class Tag = void, class IndexType = int, IndexType I = 0>
constexpr auto index(std::integral_constant<IndexType, I> /*unused*/) noexcept
{
    return compact_index<Tag, IndexType, I>{};
}

#define BOOST_STATIC_VIEWS_DECLARE_BIN_OP(op)                                  \
    template <class Tag, class IndexType, std::ptrdiff_t ExtentA,              \
        std::ptrdiff_t ExtentB>                                                \
    BOOST_STATIC_VIEWS_PURE BOOST_STATIC_VIEWS_CONSTEXPR auto operator op(     \
        compact_index<Tag, IndexType, ExtentA> const a,                        \
        compact_index<Tag, IndexType, ExtentB> const b) noexcept->bool         \
    {                                                                          \
        return a.index() op b.index();                                         \
    }
#line 112 "compact_index.hpp"
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(==)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(!=)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(<)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(<=)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(>)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(>=)

#undef BOOST_STATIC_VIEWS_DECLARE_BIN_OP

#define BOOST_STATIC_VIEWS_DECLARE_BIN_OP(op)                                  \
    template <class Tag, class IndexType, std::ptrdiff_t Extent>               \
    BOOST_STATIC_VIEWS_PURE BOOST_STATIC_VIEWS_CONSTEXPR auto operator op(     \
        compact_index<Tag, IndexType, Extent> const a,                         \
        IndexType const                             b) noexcept->bool                                      \
    {                                                                          \
        return a.index() op b;                                                 \
    }                                                                          \
    template <class Tag, class IndexType, std::ptrdiff_t Extent>               \
    BOOST_STATIC_VIEWS_PURE BOOST_STATIC_VIEWS_CONSTEXPR auto operator op(     \
        IndexType const                             a,                         \
        compact_index<Tag, IndexType, Extent> const b) noexcept->bool          \
    {                                                                          \
        return a op b.index();                                                 \
    }
#line 138 "compact_index.hpp"
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(==)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(!=)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(<)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(<=)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(>)
BOOST_STATIC_VIEWS_DECLARE_BIN_OP(>=)

#undef BOOST_STATIC_VIEWS_DECLARE_BIN_OP

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_COMPACT_INDEX_HPP
#line 1 "copy.hpp"
//          Copyright Tom Westerhout 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/copy.hpp
///
/// \brief Implementation of #copy.

#ifndef BOOST_STATIC_VIEWS_COPY_HPP
#define BOOST_STATIC_VIEWS_COPY_HPP

#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

struct copy_impl {

  public:
    // clang-format off
    template <class T
        BOOST_STATIC_VIEWS_REQUIRES(std::is_copy_constructible<T>::value)
    constexpr
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
            "`boost::static_views::copy(x)` requires x to be "
            "copy-constructible.");
    }
#endif
};

} // end namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::copy_impl, copy)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_COPY_HPP
#line 18 "drop.hpp"
#include <algorithm>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

struct drop_index_tag {
};

template <class Wrapper, std::ptrdiff_t Extent>
struct drop_view_impl
    : view_adaptor_base<drop_view_impl<Wrapper, Extent>, Wrapper>
    , private compact_index<drop_index_tag,
          typename view_adaptor_base<drop_view_impl<Wrapper, Extent>,
              Wrapper>::index_type,
          Extent> {

  private:
    using wrapper_type = Wrapper;
    using base = view_adaptor_base<drop_view_impl<Wrapper, Extent>, Wrapper>;
    using compact_index_type =
        compact_index<drop_index_tag, typename base::index_type, Extent>;
    using compact_index_type::index;

  public:
    using typename base::index_type;
    using typename base::reference;
    using typename base::size_type;
    using typename base::value_type;

    using base::parent;

    /// \brief Constructs a view of \p xs with the first \p b elements
    /// dropped.

    /// \param xs **Rvalue** reference to a wrapper around a view.
    /// \param b  Number of elements to drop. \p b must not exceed the size of
    ///           \p xs.
    constexpr drop_view_impl(Wrapper&& xs, compact_index_type const b)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            std::is_nothrow_constructible<base, Wrapper&&>::value)
        : base{std::move(xs)}, compact_index_type{b}
    {
        BOOST_STATIC_VIEWS_EXPECT(
            0 <= b && b <= static_cast<index_type>(parent().size()),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
    }

    drop_view_impl(drop_view_impl const&) = default;
    drop_view_impl(drop_view_impl&&)      = default;
    drop_view_impl& operator=(drop_view_impl const&) = default;
    drop_view_impl& operator=(drop_view_impl&&) = default;

    /// \brief Returns the number of elements viewed.
    BOOST_STATIC_VIEWS_PURE
    constexpr auto size() const noexcept -> size_type
    {
        static_assert(
            noexcept(parent().size()), BOOST_STATIC_VIEWS_BUG_MESSAGE);
        // Per construction, 0 <= _b <= parent().size(), so this is safe
        return parent().size() - static_cast<size_type>(index());
    }

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        static_assert(base::extent() == dynamic_extent
                          || base::extent() >= compact_index_type::extent(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return base::extent() == dynamic_extent
                   ? dynamic_extent
                   : compact_index_type::extent() == dynamic_extent
                         ? base::extent()
                         : base::extent() - compact_index_type::extent();
    }

    /// \brief "Maps" index \p i to the corresponding index in the
    /// parent view.

    /// For \f$ i \geq \text{xs.size}()\f$ behavior of this function
    /// is undefined
    constexpr auto map(index_type const i) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    {
        BOOST_STATIC_VIEWS_EXPECT(0 <= i && i < static_cast<index_type>(size()),
            "boost::static_views::drop_impl::map: Precondition "
            "`0 <= i < size()` is not satisfied.");
        return index() + i;
    }
};

struct drop_exactly_impl {
  private:
    template <class Wrapper, class IndexType>
    constexpr auto call_impl(Wrapper xs, IndexType const b) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            drop_view_impl<Wrapper, IndexType::extent()>{std::move(xs), b});

    template <class T>
    using index_t =
        typename std::remove_cv_t<std::remove_reference_t<T>>::index_type;

  public:
    // clang-format off
    template <class V
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>)
    constexpr
    auto operator()(V&& xs,
        typename std::remove_cv_t<
                 std::remove_reference_t<V>>::index_type const b) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            noexcept(std::declval<drop_exactly_impl const&>().call_impl(
                make_wrapper(std::forward<V>(xs)), index(b))))
    {
        using index_type = index_t<V>;
        BOOST_STATIC_VIEWS_EXPECT(
            0 <= b && b <= static_cast<index_type>(xs.size()),
            "boost::static_views::drop_exactly(xs, b): Precondition "
            "`0 <= b <= xs.size()` is not satisfied.");
        return call_impl(
            make_wrapper(std::forward<V>(xs)), index<drop_index_tag>(b));
    }

    // clang-format off
    template <class IndexType
        BOOST_STATIC_VIEWS_REQUIRES(std::is_integral<IndexType>::value)
    constexpr
    auto operator()(IndexType const b) const noexcept
    // clang-format on
    {
        return lazy_adaptor(drop_exactly_impl{}, copy(b));
    }

    // clang-format off
    template <class V, class Int, Int I
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>)
    constexpr
    auto operator()(V&& xs, std::integral_constant<Int, I> /*unused*/) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            noexcept(std::declval<drop_exactly_impl const&>().call_impl(
                make_wrapper(std::forward<V>(xs)),
                index(std::integral_constant<index_t<V>, I>{}))))
    {
        using index_type = index_t<V>;
        using view_type  = std::remove_cv_t<std::remove_reference_t<V>>;
        constexpr auto b = static_cast<index_type>(I);
        static_assert(
            view_type::extent() == dynamic_extent || view_type::extent() >= b,
            "boost::static_views::drop_exactly(xs, b): Can't drop more "
            "elements than there are in the view.");
        BOOST_STATIC_VIEWS_EXPECT(
            0 <= b && b <= static_cast<index_type>(xs.size()),
            "boost::static_views::drop_exactly(xs, b): Precondition "
            "`0 <= b <= xs.size()` is not satisfied.");
        return call_impl(make_wrapper(std::forward<V>(xs)),
            index<drop_index_tag>(std::integral_constant<index_type, I>{}));
    }

    // clang-format off
    template <class IndexType, IndexType I>
    constexpr
    auto operator()(std::integral_constant<IndexType, I> const b) const noexcept
    // clang-format on
    {
        return lazy_adaptor(drop_exactly_impl{}, copy(b));
    }

#if !defined(BOOST_STATIC_VIEWS_SFINAE)
    // clang-format off
    template <class V, class IndexType
        BOOST_STATIC_VIEWS_REQUIRES(
            !View<std::remove_cv_t<std::remove_reference_t<V>>>)
    constexpr auto operator()(V&& /*unused*/, IndexType /*unused*/) const noexcept
    // clang-format on
    {
        static_assert(View<std::remove_cv_t<std::remove_reference_t<V>>>,
            "`boost::static_views::drop_exactly(xs, b)` requires xs to model "
            "the View concept.");
        return why_is_my_argument_not_a_view<
            std::remove_cv_t<std::remove_reference_t<V>>>();
    }
#endif
};

} // end namespace detail

/// \verbatim embed:rst:leading-slashes
/// :math:`\mathtt{drop} : \mathbb{N} \to \text{View} \to
/// \text{View}` is a functor that let's you create dropped views.
/// Given a count :math:`n \in \mathbb{N}` and a :ref:`view
/// <view-concept>` :math:`xs`, creates a view of :math:`xs` with the
/// first :math:`n` elements dropped. If the size of :math:`xs` is
/// less than :math:`n`, an empty view is returned.
/// \endverbatim
// BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_drop_algo_impl, drop)

/// \verbatim embed:rst:leading-slashes
/// :math:`\mathtt{drop} : \mathbb{N} \to \text{View} \to \text{View}`
/// is a functor that let's you create dropped views. Very similar to
/// :cpp:var:`drop`, but has an extra precondition. When given a count
/// :math:`n` and a :ref:`view <view-concept>` :math:`xs`, it assumes
/// that the size of :math:`xs` is at least :math:`n`. If that's not
/// the case, an assertion failure is triggered.
///
/// .. warning::
///
///   If you disable the checks (i.e. define
///   ``BOOST_STATIC_VIEWS_DISABLE_CHECKS``) and pass an :math:`n >
///   xs\mathtt{.size()}`, **the behavior is undefined**.
/// \endverbatim
// BOOST_STATIC_VIEWS_INLINE_VARIABLE(
//     detail::make_drop_exactly_algo_impl, drop_exactly)
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::drop_exactly_impl, drop_exactly)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DROP_HPP
#line 1 "take.hpp"
//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/take.hpp
///
/// \brief Implementation of #take.

#ifndef BOOST_STATIC_VIEWS_TAKE_HPP
#define BOOST_STATIC_VIEWS_TAKE_HPP

#include <algorithm>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class Wrapper, std::ptrdiff_t Extent>
struct take_view_impl
    : view_adaptor_base<take_view_impl<Wrapper, Extent>, Wrapper>
    , private compact_index<void,
          typename view_adaptor_base<take_view_impl<Wrapper, Extent>,
              Wrapper>::index_type,
          Extent> {

  private:
    using wrapper_type = Wrapper;
    using base = view_adaptor_base<take_view_impl<Wrapper, Extent>, Wrapper>;
    using compact_index_type =
        compact_index<void, typename base::index_type, Extent>;
    using compact_index_type::index;

  public:
    using typename base::index_type;
    using typename base::reference;
    using typename base::size_type;
    using typename base::value_type;

    using base::parent;

    /// \brief Constructs a view of \p xs consisting of at most \p n
    /// elements of \p xs.

    /// \param xs **Rvalue** reference to a wrapper around a view.
    /// \param n  Number of elements to take. `n` must not exceed the size of
    ///           `xs`.
    constexpr take_view_impl(wrapper_type&& xs, compact_index_type const n)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            std::is_nothrow_constructible<base, Wrapper&&>::value)
        : base{std::move(xs)}, compact_index_type{n}
    {
        BOOST_STATIC_VIEWS_EXPECT(
            0 <= n && n <= static_cast<index_type>(parent().size()),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
    }

    take_view_impl(take_view_impl const&) = default;
    take_view_impl(take_view_impl&&)      = default;
    take_view_impl& operator=(take_view_impl const&) = default;
    take_view_impl& operator=(take_view_impl&&) = default;

    /// \brief Returns the number of elements viewed.
    BOOST_STATIC_VIEWS_PURE
    constexpr auto size() const noexcept -> size_type
    {
        return static_cast<size_type>(index());
    }

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        static_assert(base::extent() == dynamic_extent
                          || compact_index_type::extent() == dynamic_extent
                          || base::extent() >= compact_index_type::extent(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return compact_index_type::extent() == dynamic_extent
                   ? base::extent()
                   : compact_index_type::extent();
    }

    /// \brief "Maps" index \p i to the corresponding index in the
    /// parent view.

    /// For \f$ i \geq \text{xs.size}()\f$ behavior of this function
    /// is undefined
    constexpr auto map(index_type const i) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    {
        BOOST_STATIC_VIEWS_EXPECT(0 <= i && i < static_cast<index_type>(size()),
            "boost::static_views::take_view_impl::map: Precondition "
            "`0 <= i < size()` is not satisfied.");
        return i;
    }
};

struct take_exactly_impl {
  private:
    template <class Wrapper, class IndexType>
    constexpr auto call_impl(Wrapper xs, IndexType const n) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            take_view_impl<Wrapper, IndexType::extent()>{std::move(xs), n});

    template <class T>
    using index_t =
        typename std::remove_cv_t<std::remove_reference_t<T>>::index_type;

  public:
    // clang-format off
    template <class V
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>)
    constexpr
    auto operator()(V&& xs,
        typename std::remove_cv_t<
                 std::remove_reference_t<V>>::index_type const n) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            noexcept(std::declval<take_exactly_impl const&>().call_impl(
                make_wrapper(std::forward<V>(xs)), index(n))))
    {
        using index_type = index_t<V>;
        BOOST_STATIC_VIEWS_EXPECT(
            0 <= n && n <= static_cast<index_type>(xs.size()),
            "boost::static_views::take_exactly(xs, n): Precondition "
            "`0 <= n <= xs.size()` is not satisfied.");
        return call_impl(make_wrapper(std::forward<V>(xs)), index(n));
    }

    // clang-format off
    template <class IndexType
        BOOST_STATIC_VIEWS_REQUIRES(std::is_integral<IndexType>::value)
    constexpr
    auto operator()(IndexType const n) const noexcept
    // clang-format on
    {
        return lazy_adaptor(take_exactly_impl{}, copy(n));
    }

    // clang-format off
    template <class V, class Int, Int I
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>)
    constexpr
    auto operator()(V&& xs, std::integral_constant<Int, I> /*unused*/) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            noexcept(std::declval<take_exactly_impl const&>().call_impl(
                make_wrapper(std::forward<V>(xs)),
                index(std::integral_constant<index_t<V>, I>{}))))
    {
        using index_type = index_t<V>;
        using view_type  = std::remove_cv_t<std::remove_reference_t<V>>;
        constexpr auto n = static_cast<index_type>(I);
        static_assert(
            view_type::extent() == dynamic_extent || view_type::extent() >= n,
            "boost::static_views::take_exactly(xs, n): Can't take more "
            "elements than there are in the view.");
        BOOST_STATIC_VIEWS_EXPECT(
            0 <= n && n <= static_cast<index_type>(xs.size()),
            "boost::static_views::take_exactly(xs, b): Precondition "
            "`0 <= n <= xs.size()` is not satisfied.");
        return call_impl(make_wrapper(std::forward<V>(xs)),
            index(std::integral_constant<index_type, I>{}));
    }

    // clang-format off
    template <class IndexType, IndexType I>
    constexpr
    auto operator()(std::integral_constant<IndexType, I> const n) const noexcept
    // clang-format on
    {
        return lazy_adaptor(take_exactly_impl{}, copy(n));
    }

#if !defined(BOOST_STATIC_VIEWS_SFINAE)
    // clang-format off
    template <class V, class IndexType
        BOOST_STATIC_VIEWS_REQUIRES(
            !View<std::remove_cv_t<std::remove_reference_t<V>>>)
    constexpr auto operator()(V&& /*unused*/, IndexType /*unused*/) const noexcept
    // clang-format on
    {
        static_assert(View<std::remove_cv_t<std::remove_reference_t<V>>>,
            "`boost::static_views::take_exactly(xs, n)` requires xs to model "
            "the View concept.");
        return why_is_my_argument_not_a_view<
            std::remove_cv_t<std::remove_reference_t<V>>>();
    }
#endif
};

} // end namespace detail

/// \verbatim embed:rst:leading-slashes
/// :math:`\mathtt{take} : \mathbb{N} \to \text{View} \to
/// \text{View}` is a functor that let's you create take views. Given
/// a count :math:`n \in \mathbb{N}` and a :ref:`view <view-concept>`
/// :math:`xs`, creates a view of :math:`xs` consisting only of the
/// first :math:`n` elements of :math:`xs`. If the size of :math:`xs`
/// is less than :math:`n`, view of the whole :math:`xs` is returned.
/// \endverbatim
// BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_take_algo_impl, take)

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::take_exactly_impl, take_exactly)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_TAKE_HPP
#line 18 "static_views/chunk.hpp"
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

    constexpr auto chunk_count() const noexcept -> index_type
    {
        return _chunk_count;
    }

    constexpr auto chunk_size() const noexcept -> index_type { return index(); }

  public:
    constexpr chunk_view_impl(Wrapper&& xs, index_type const chunk_count,
        chunk_size_type const chunk_size)
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
    constexpr auto size() const noexcept -> size_type
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
    constexpr
    auto unsafe_at_impl(index_type const i, std::true_type /*is static*/) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    // clang-format on
    {
        return take_exactly(drop_exactly(parent(), i),
            std::integral_constant<index_type, ChunkSize>{});
    }

    // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr
    auto unsafe_at_impl(index_type const i, std::false_type /*is static*/) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    // clang-format on
    {
        return take_exactly(drop_exactly(parent(), i), chunk_size());
    }

  public:
    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto unsafe_at(
        index_type const i) const& BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
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
    constexpr auto operator[](index_type const i) const&
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

    using reference = decltype(
        std::declval<chunk_view_impl const&>()[std::declval<index_type>()]);
    using value_type = std::remove_reference_t<reference>;
};

struct chunk_impl {
  private:
    template <class Wrapper, class ChunkCount, class ChunkSize>
    constexpr auto call_impl(
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
    constexpr
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
    constexpr auto operator()(
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
    constexpr auto operator()(
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
    constexpr auto operator()(Int const chunk_size) const noexcept
    // clang-format on
    {
        return lazy_adaptor(chunk_impl{}, copy(chunk_size));
    }

    // clang-format off
    template <class IndexType, IndexType N>
    constexpr
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
#line 1 "static_views/flatten.hpp"
//          Copyright Tom Westerhout 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/flatten.hpp
///
/// \brief Implementation of #flatten.

#ifndef BOOST_STATIC_VIEWS_FLATTEN_HPP
#define BOOST_STATIC_VIEWS_FLATTEN_HPP

#include <algorithm>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class Wrapper>
struct flatten_view_impl
    : view_adaptor_base<flatten_view_impl<Wrapper>, Wrapper> {

  private:
    using wrapper_type = Wrapper;
    using base         = view_adaptor_base<flatten_view_impl<Wrapper>, Wrapper>;

  public:
    using typename base::index_type;
    using typename base::size_type;
    using reference  = typename base::value_type::reference;
    using value_type = std::remove_reference_t<reference>;
    using base::parent;

  public:
    constexpr flatten_view_impl(Wrapper&& xs) BOOST_STATIC_VIEWS_NOEXCEPT_IF(
        std::is_nothrow_constructible<base, Wrapper&&>::value)
        : base{std::move(xs)}
    {
    }

    flatten_view_impl(flatten_view_impl const&) = default;
    flatten_view_impl(flatten_view_impl&&)      = default;
    flatten_view_impl& operator=(flatten_view_impl const&) = default;
    flatten_view_impl& operator=(flatten_view_impl&&) = default;

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        constexpr auto a = wrapper_type::value_type::extent();
        constexpr auto b = wrapper_type::value_type::value_type::extent();
        if (a == dynamic_extent || b == dynamic_extent) {
            return dynamic_extent;
        }
        else {
            return a * b;
        }
    }

    constexpr auto size() const noexcept -> size_type
    {
        if (parent().size() != 0) {
            return parent().size() * parent().unsafe_at(0).size();
        }
        else {
            return 0;
        }
    }

    constexpr decltype(auto) unsafe_at(index_type const i) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    {
        BOOST_STATIC_VIEWS_EXPECT(0 <= i && i < static_cast<index_type>(size()),
            "boost::static_views::flatten_view_impl::unsafe_at: Precondition "
            "`0 <= i < size()` is not satisfied.");
        auto const n = static_cast<index_type>(parent().unsafe_at(0).size());
        auto const first  = i / n;
        auto const second = i % n;
        return parent().unsafe_at(first).unsafe_at(second);
    }
};

struct flatten_impl {
  private:
    template <class Wrapper>
    constexpr auto call_impl(Wrapper xs) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            flatten_view_impl<Wrapper>{std::move(xs)});

    // clang-format off
    template <class V
        BOOST_STATIC_VIEWS_REQUIRES(View<remove_cvref_t<V>>)
    constexpr auto call(V&& xs, int /*unused*/) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<flatten_impl const&>().call_impl(
                make_wrapper(std::forward<V>(xs)))))
    {
        return call_impl(make_wrapper(std::forward<V>(xs)));
    }

#if !defined(BOOST_STATIC_VIEWS_SFINAE)
    template <class V                            BOOST_STATIC_VIEWS_REQUIRES(
        !View<remove_cvref_t<V>>) constexpr auto call(V&& /*unused*/, ...)
            const noexcept
    {
        static_assert(View<std::remove_cv_t<std::remove_reference_t<V>>>,
            "`boost::static_views::flatten(xs)` requires xs to model "
            "the View concept.");
        return why_is_my_argument_not_a_view<remove_cvref_t<V>>();
    }
#endif

  public:
    template <class V>
    constexpr auto operator()(V&& xs) const BOOST_STATIC_VIEWS_NOEXCEPT_IF(
        noexcept(std::declval<flatten_impl const&>().call(
            std::forward<V>(xs), int{})))
    {
        return call(std::forward<V>(xs), int{});
    }

    template <class F>
    constexpr auto operator()() const noexcept
    {
        return lazy_adaptor(flatten_impl{});
    }
};

} // end namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::flatten_impl, flatten)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_FLATTEN_HPP
#line 1 "static_views/hash_c.hpp"
//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_HASH_C_HPP
#define BOOST_STATIC_VIEWS_HASH_C_HPP

#include <type_traits>

// clang-format off
#if __cplusplus > 201402
#if defined(__has_include)
#   if __has_include(<string_view>)
#       include <string_view>
#       define BOOST_STATIC_VIEWS_HAVE_STRING_VIEW
        BOOST_STATIC_VIEWS_BEGIN_NAMESPACE
        template <class CharT, class Traits = std::char_traits<CharT>>
        using basic_string_view =
            std::basic_string_view<CharT, Traits>;
        using std::string_view;
        using std::u16string_view;
        using std::u32string_view;
        using std::wstring_view;
        BOOST_STATIC_VIEWS_END_NAMESPACE
#   elif __has_include(<experimental/string_view>)
#       include <experimental/string_view>
#       define BOOST_STATIC_VIEWS_HAVE_STRING_VIEW
        BOOST_STATIC_VIEWS_BEGIN_NAMESPACE
        template <class CharT, class Traits = std::char_traits<CharT>>
        using basic_string_view =
            std::experimental::basic_string_view<CharT, Traits>;
        using std::experimental::string_view;
        using std::experimental::u16string_view;
        using std::experimental::u32string_view;
        using std::experimental::wstring_view;
        BOOST_STATIC_VIEWS_END_NAMESPACE
#   endif
#endif
#endif
// clang-format on

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
template <class T>
using not_equal_t = decltype(std::declval<T>() != std::declval<T>());
} // namespace detail

// clang-format off
template <class Char>
BOOST_STATIC_VIEWS_FORCEINLINE
constexpr
auto strlen_c(Char const* const str) noexcept -> std::size_t
// clang-format on
{
    using boost::static_views::is_detected;
    static_assert(std::is_nothrow_default_constructible<Char>::value,
        "`boost::static_views::strlen_c<Char>` requires `Char` to be nothrow "
        "default-constructible.");
    static_assert(is_detected<detail::not_equal_t, Char>::value,
        "`boost::static_views::strlen_c<Char>` needs an `operator!=` for "
        "`Char`.");
    static_assert(noexcept(std::declval<Char>() != std::declval<Char>()),
        "`boost::static_views::strlen_c<Char>` assumes that `Char`'s "
        "`operator!=` is noexcept.");

    std::size_t i = 0;
    while (str[i] != Char{}) {
        ++i;
    }
    return (i == 0) ? 0 : (i - 1);
}

// This algorithm is "stolen" from the Internet.
// clang-format off
template <class Char>
BOOST_STATIC_VIEWS_FORCEINLINE
constexpr
auto crc32_hash(Char const* const str) noexcept -> std::uint32_t
// clang-format on
{
    static_assert(std::is_integral<Char>::value,
        "`boost::static_views::crc32_hash` requires `Char` to be an integral "
        "type.");
    constexpr std::uint32_t INITXOR  = 0xFFFFFFFF;
    constexpr std::uint32_t FINALXOR = 0xFFFFFFFF;
    constexpr std::uint32_t CRCPOLY  = 0xEDB88320;

    auto const l      = length_c(str);
    auto       crcreg = INITXOR;

    for (std::size_t j = 0; j < l; ++j) {
        auto b = static_cast<std::make_unsigned_t<Char>>(str[j]);
        for (std::size_t i = 0; i < 8 * sizeof(Char); ++i) {
            if ((crcreg ^ b) & 1)
                crcreg = (crcreg >> 1) ^ CRCPOLY;
            else
                crcreg >>= 1;
            b >>= 1;
        }
    }

    return crcreg ^ FINALXOR;
}

namespace detail {
// clang-format off
template <class Char>
BOOST_STATIC_VIEWS_FORCEINLINE
constexpr
auto simple_hash(Char const* const str) noexcept
// clang-format on
{
    using boost::static_views::is_detected;
    static_assert(std::is_nothrow_default_constructible<Char>::value,
        "`boost::static_views::detail::simple_hash<Char>` requires `Char` to "
        "be nothrow default-constructible.");
    static_assert(is_detected<detail::not_equal_t, Char>::value,
        "`boost::static_views::detail::simple_hash<Char>` needs an "
        "`operator!=` for `Char`.");
    static_assert(noexcept(std::declval<Char>() != std::declval<Char>()),
        "`boost::static_views::detail::simple_hash<Char>` assumes that "
        "`Char`'s `operator!=` is noexcept.");
    std::size_t hash = 0;
    std::size_t i    = 0;
    while (str[i] != Char{}) {
        hash = 37 * hash + static_cast<std::make_unsigned_t<Char>>(str[i]);
        ++i;
    }
    return hash;
}
} // namespace detail

template <class T, class = void>
struct hash_impl;

template <class T>
struct hash_impl<T, std::enable_if_t<std::is_integral<T>::value>> {
    constexpr auto operator()(T const x) const noexcept -> std::size_t
    {
        return static_cast<std::size_t>(x);
    }
};

template <class Char>
struct hash_impl<Char const*, std::enable_if_t<std::is_integral<Char>::value>> {
    constexpr auto operator()(Char const* const x) const noexcept -> std::size_t
    {
        return simple_hash(x);
        // return static_cast<std::size_t>(crc32_hash(x));
    }
};

#if defined(BOOST_STATIC_VIEWS_HAVE_STRING_VIEW)
template <class Char>
struct hash_impl<basic_string_view<Char>> {
    constexpr auto operator()(basic_string_view<Char> const x) const noexcept
        -> std::size_t
    {
        return hash_impl<Char const*>{}(x.data());
    }
};
#endif

struct hash_c {
    // clang-format off
    template <class T>
    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr
    auto operator()(T&& x) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(hash_impl<std::remove_cv_t<std::remove_reference_t<T>>>{}(
                std::forward<T>(x)))) -> std::size_t
    {
        return hash_impl<std::decay_t<T>>{}(std::forward<T>(x));
    }
};

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_HASH_C_HPP
#line 1 "static_views/hashed.hpp"
//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_HASHED_HPP
#define BOOST_STATIC_VIEWS_HASHED_HPP
#line 1 "detail/find_first.hpp"
//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_DETAIL_FIND_FIRST_HPP
#define BOOST_STATIC_VIEWS_DETAIL_FIND_FIRST_HPP

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
struct find_first_i_impl {
    // clang-format off
    template <class V, class Predicate
        BOOST_STATIC_VIEWS_REQUIRES(View<V>
            && is_invocable_r<bool, Predicate, typename V::reference>::value)
    constexpr
    auto operator()(V const& xs, Predicate&& p) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            invoke(p, xs.unsafe_at(std::declval<typename V::index_type>()))))
    {
        using index_type   = typename V::index_type;
        index_type const n = static_cast<index_type>(xs.size());
        index_type       i = 0;
        while (i < n && !invoke(p, xs.unsafe_at(i))) {
            ++i;
        }
        return i;
    }
};
} // namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::find_first_i_impl, find_first_i)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DETAIL_FIND_FIRST_HPP
#line 1 "raw_view.hpp"
//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/raw_view.hpp
///
/// \brief Implementation of #raw_view.

#ifndef BOOST_STATIC_VIEWS_RAW_VIEW_HPP
#define BOOST_STATIC_VIEWS_RAW_VIEW_HPP
#line 1 "sequence_traits.hpp"
//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// \file sequence_traits.hpp
// \brief Implementats #boost::static_views::sequence_traits.

#ifndef BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP
#define BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP

#include <array>
#include <limits>
#include <tuple>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \brief A proxy through which functions access sequences.

// clang-format off
/// \verbatim embed:rst:leading-slashes
///
/// :ref:`As mentioned <sequence-concept>` earlier, sequences are
/// homogeneous collections with random random access to elements.
/// Sequences of length known at compile-time are called *static
/// sequences*. Sequences of unknown (at compile-time) length are,
/// obviously, called *dynamic sequences*. The distinction is made
/// using the property called *extent* which is similar to the
/// ``Extent`` template parameter in the |std::span proposal|_.
///
/// .. |std::span proposal| replace:: ``std::span`` proposal
/// .. _std::span proposal: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0122r4.pdf
///
/// :cpp:class:`sequence_traits` provides a uniform interface for
/// working with sequences. A general :cpp:class:`sequence_traits`
/// declaration may look like this:
///
/// .. code-block:: cpp
///
///   namespace boost {
///   namespace static_views {
///
///   template <class T>
///   struct sequence_traits {
///       static constexpr auto size(T const&) noexcept ->std::size_t;
///
///       static constexpr auto extent() noexcept ->std::ptrdiff_t;
///
///       static constexpr auto at(T const&, std::size_t) noexcept ->const_reference;
///       static constexpr auto at(T&, std::size_t) noexcept ->reference;
///       static constexpr auto at(T&&, std::size_t) noexcept ->rvalue_reference;
///   };
///
///   } // namespace static_views
///   } // namespace boost
///
/// * Elements are accessed by calling
///   ``sequence_traits<T>::at(xs, i)`` with a (const) reference to
///   ``T`` ``xs`` and index ``i``. ``at()`` must furthermore be
///   ``static`` and ``noexcept``. You're by no means required to
///   create all three overloads of ``at()``, one is sufficient.
///
/// * Extent is detemined by calling ``sequence_traits<T>::extent()``.
///   It should be ``static``, ``noexcept``, and have return type
///   convertible to ``std::ptrdiff_t``. If the size of ``T`` is known
///   at compile-time, ``extent()`` should return it. Otherwise, a
///   special value :cpp:var:`dynamic_extent` should be returned.
///
/// * Number of elements in a sequence ``xs`` is determined by calling
///   ``sequence_traits<T>::size(xs)``. ``size()`` must be ``static``,
///   ``noexcept``, and have return type convertible to
///   ``std::size_t``. Furthermore it it assumed that if
///   ``sequence_traits<T>::extent()`` returns a non-negative number,
///   calling ``size`` with any sequence of type ``T`` will return the
///   same result.
///
/// Currently, only arrays (``T[N]`` and ``std::array<T, N>``) and
/// homogeneous tuples (``std::tuple<Ts...>`` where :math:`\exists
/// \text{U}: \forall \text{T} \in \{\text{Ts}\dots\}: \text{T} =
/// \text{U}`) have specialisations of :cpp:class:`sequencce_traits`.
/// Specialising it for other types is really simple though.
/// :doc:`Here's <examples/support_span>` how one can add
/// support for ``std::span``:
///
/// .. literalinclude:: ../example/support_span.cpp
///    :lines: 10-37
///
///
/// \endverbatim
// clang-format on
template <class T>
struct sequence_traits;

template <class Derived, class Sequence>
struct sequence_traits_default {

    using size_type       = unsigned; // std::size_t;
    using difference_type = int;      // std::ptrdiff_t;
    using index_type      = int;      // difference_type;

  private:
    template <class Dummy>
    struct traits {
        using derived  = Derived;
        using sequence = Sequence;
        using type     = sequence_traits_default<Derived, Sequence>;
    };

    /// \brief Returns whether U "is similar enough to" S, i.e. U is either
    /// equal to S or a const of volatile qualified version thereof.
    ///
    /// This is a hack to make sequence_traits work with const and volatile
    /// qualifiers.
    template <class U, class S>
    static constexpr auto is_similar_enough_to()
    {
        using U2 = std::remove_reference_t<U>;
        return Same<S,
                   U2> || Same<S const, U2> || Same<S volatile, U2> || Same<S const volatile, U2>;
    }

    // Derived::extent() is non-negative
    // clang-format off
    template <class U, class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(traits<Dummy>::derived::extent() >= 0
            && is_similar_enough_to<U, Sequence>())
    static constexpr auto size_impl(U& /*unused*/, int /*unused*/) noexcept
    // clang-format on
    {
        return static_cast<size_type>(Derived::extent());
    }

    // Sequence::size() exists
    // clang-format on
    template <class U,
        class Dummy = void BOOST_STATIC_VIEWS_REQUIRES(HasSize<Sequence>&&
                is_similar_enough_to<U, Sequence>()) static constexpr auto
                           size_impl(U& xs, ... /*unused*/) noexcept
    // clang-format off
    {
        static_assert(noexcept(xs.size()), "Throwing size() is a bad idea :)");
        return xs.size();
    }

    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(
            HasExtent<typename traits<Dummy>::sequence>)
    static constexpr auto extent_impl(int /*unused*/) noexcept
    // clang-format on
    {
        return Sequence::extent();
    }

    static constexpr auto extent_impl(...) noexcept { return dynamic_extent; }

  public:
    // clang-format off
    template <class IndexType, class S
        BOOST_STATIC_VIEWS_REQUIRES(
            HasIndexOperator<Sequence, IndexType>
                && is_similar_enough_to<S, Sequence>())
    static constexpr decltype(auto) at(S&& xs, IndexType i)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(std::forward<S>(xs)[i]))
    // clang-format on
    {
        return std::forward<S>(xs)[i];
    }

    // Only enable if size_impl is available
    template <class S>
    static constexpr auto size(S& xs) noexcept
        -> decltype(sequence_traits_default::size_impl(xs, int{}))
    {
        return sequence_traits_default::size_impl(xs, int{});
    }

    template <class Dummy = void>
    static constexpr auto extent() noexcept
        -> decltype(traits<Dummy>::type::extent_impl(int{}))
    {
        constexpr auto extent = sequence_traits_default::extent_impl(int{});
        static_assert(extent >= 0 || extent == dynamic_extent,
            "Extent of a Sequence cannot be negative.");
        return extent;
    }
};

#if defined(BOOST_STATIC_VIEWS_CONCEPTS)
// clang-format off
template <class T>
concept bool Sequence = requires(T& a, typename sequence_traits<T>::index_type i) {
    { sequence_traits<T>::extent() } noexcept -> std::ptrdiff_t;
    sequence_traits<T>::extent() >= 0
        || sequence_traits<T>::extent() == dynamic_extent;
    { sequence_traits<T>::size(a) } noexcept
        -> typename sequence_traits<T>::size_type;
    { sequence_traits<T>::at(a, i) }
        ->typename sequence_traits<T>::reference;
};

template <class T>
concept bool StaticSequence = requires() {
    Sequence<T>;
    sequence_traits<T>::extent() >= 0;
};
// clang-format on
#else
namespace detail {

template <class T>
using has_sequence_traits_specialisation_t = decltype(sequence_traits<T>{});

template <class T>
constexpr bool HasSequenceTraitsSpecialisation =
    is_detected<has_sequence_traits_specialisation_t, T>::value;

template <class T, class = void>
struct _SequenceTraitsHaveExtent : std::false_type {
};

template <class T>
struct _SequenceTraitsHaveExtent<T,
    std::enable_if_t<
        // First of all, make sure sequence_traits<T>::extent() exists
        // and returns a std::ptrdiff_t
        // P.S. No need to go through the detected_t path as
        // std::is_same is lazy. Yay! :)
        std::is_same<decltype(sequence_traits<T>::extent()),
            std::ptrdiff_t>::value
// std::is_same<detected_t<sequence_traits_have_extent_t, T>,
//     std::ptrdiff_t>::value
// Next, make sure it is constexpr
#if !defined(BOOST_STATIC_VIEWS_MSVC)
        // TODO: I still don't understand why this fails...
        && (sequence_traits<T>::extent(), true)
#endif
        >>
    // Finally, check that the returned value makes sense.
    : std::conditional_t<sequence_traits<T>::extent() >= 0
                             || sequence_traits<T>::extent() == dynamic_extent,
          std::true_type, std::false_type> {
    // This is a hack
    using is_static = std::conditional_t<sequence_traits<T>::extent() >= 0,
        std::true_type, std::false_type>;
};

template <class T>
constexpr bool SequenceTraitsHaveExtent = _SequenceTraitsHaveExtent<T>::value;

template <class T, class = void>
struct _SequenceTraitsHaveSize : std::false_type {
};

template <class T>
struct _SequenceTraitsHaveSize<T,
    std::enable_if_t<std::is_same<decltype(sequence_traits<T>::size(
                                      std::declval<T const&>())),
        typename sequence_traits<T>::size_type>::value>> : std::true_type {
};

template <class T>
constexpr bool SequenceTraitsHaveSize = _SequenceTraitsHaveSize<T>::value;

template <class T, class = void>
struct _SequenceTraitsHaveAt : std::false_type {
};

template <class T>
struct _SequenceTraitsHaveAt<T,
    std::enable_if_t<std::is_same<
        decltype(sequence_traits<T>::at(std::declval<T&>(),
            std::declval<typename sequence_traits<T>::index_type>())),
        typename sequence_traits<T>::reference>::value>> : std::true_type {
};

template <class T>
constexpr bool SequenceTraitsHaveAt = _SequenceTraitsHaveAt<T>::value;

} // namespace detail

// clang-format off
template <class T>
constexpr bool Sequence =
    detail::HasSequenceTraitsSpecialisation<T>
    && detail::SequenceTraitsHaveExtent<T>
    && detail::SequenceTraitsHaveSize<T>
    && detail::SequenceTraitsHaveAt<T>;
// clang-format on

template <class T>
constexpr bool StaticSequence = Sequence<T>&&
               std::is_same<typename detail::_SequenceTraitsHaveExtent<T>::is_static,
        std::true_type>::value;
#endif

/// \brief Specialisation of #sequence_traits for C-arrays.
template <class T, std::size_t N>
struct sequence_traits<T[N]>
    : sequence_traits_default<sequence_traits<T[N]>, T[N]> {

    using value_type      = T;
    using reference       = std::add_lvalue_reference_t<value_type>;
    using index_type      = int;
    using size_type       = unsigned;
    using difference_type = index_type;

    static_assert(std::numeric_limits<index_type>::max() >= N,
        "Wow! That's one long array! Please, submit a bug report "
        "here " BOOST_STATIC_VIEWS_ISSUES_LINK);

    static constexpr auto extent() noexcept
    {
        return static_cast<std::ptrdiff_t>(N);
    }
};

/// \brief Specialisation of #sequence_traits for std::array.
template <class T, std::size_t N>
struct sequence_traits<std::array<T, N>>
    : sequence_traits_default<sequence_traits<std::array<T, N>>,
          std::array<T, N>> {

    using value_type      = T;
    using reference       = std::add_lvalue_reference_t<T>;
    using index_type      = int;
    using size_type       = unsigned;
    using difference_type = index_type;

    static_assert(std::numeric_limits<index_type>::max() >= N,
        "Wow! That's one long array! Please, submit a bug report "
        "here " BOOST_STATIC_VIEWS_ISSUES_LINK);

    template <class S>
    static constexpr decltype(auto) at(S& xs, index_type const i) noexcept
    {
        return xs.data()[i];
    }

    static constexpr auto extent() noexcept
    {
        return static_cast<std::ptrdiff_t>(N);
    }
};

template <class T, std::size_t N>
struct sequence_traits<std::array<T, N> const>
    : sequence_traits<std::array<T, N>> {
    using value_type = std::add_const_t<T>;
    using reference  = std::add_lvalue_reference_t<T>;
};

template <class T, std::size_t N>
struct sequence_traits<std::array<T, N> volatile>
    : sequence_traits<std::array<T, N>> {
    using value_type = std::add_volatile_t<T>;
    using reference  = std::add_lvalue_reference_t<T>;
};

template <class T, std::size_t N>
struct sequence_traits<std::array<T, N> const volatile>
    : sequence_traits<std::array<T, N>> {
    using value_type = std::add_volatile_t<std::add_const_t<T>>;
    using reference  = std::add_lvalue_reference_t<T>;
};

namespace detail {

template <class Tuple, class = void>
struct sequence_traits_tuple;

template <>
struct sequence_traits_tuple<std::tuple<>> {
    static constexpr auto at(std::tuple<> xs, std::size_t i) noexcept
        -> nonesuch;
};

template <class T, class... Ts>
struct sequence_traits_tuple<std::tuple<T, Ts...>,
    std::enable_if_t<all(std::is_same<T, Ts>::value...)>> {

  public:
    using value_type = T;
    using reference  = std::add_lvalue_reference_t<T>;

  private:
    using tuple_type = std::tuple<T, Ts...>;

    template <class U>
    static constexpr decltype(auto) dummy_get(U&& tpl) noexcept
    {
        return std::get<0>(std::forward<U>(tpl));
    }

    // This is just wonderful!
    template <class U>
    using get_type =
        std::decay_t<auto(U)->decltype(dummy_get(std::declval<U>()))>;

    template <class Tuple, class IndexType, std::size_t... Is>
    static constexpr decltype(auto) at_impl(
        Tuple&& xs, IndexType i, std::index_sequence<Is...> /*unused*/) noexcept
    {
        constexpr get_type<Tuple&&> getters[] = {&std::get<Is>...};
        return (*getters[i])(std::forward<Tuple>(xs));
    }

  public:
    // clang-format off
    template <class Tuple, class IndexType
        BOOST_STATIC_VIEWS_REQUIRES(
            Same<tuple_type, std::remove_cv_t<std::remove_reference_t<Tuple>>>)
    static constexpr decltype(auto) at(Tuple&& xs, IndexType i)
        noexcept
    // clang-format on
    {
        return at_impl(std::forward<Tuple>(xs), i,
            std::make_index_sequence<sizeof...(Ts) + 1>{});
    }
};

} // namespace detail

template <class... Ts>
struct sequence_traits<std::tuple<Ts...>>
    : detail::sequence_traits_tuple<std::tuple<Ts...>>
    , sequence_traits_default<sequence_traits<std::tuple<Ts...>>,
          std::tuple<Ts...>> {

    using detail::sequence_traits_tuple<std::tuple<Ts...>>::at;

    static constexpr auto extent() noexcept
    {
        return static_cast<std::ptrdiff_t>(sizeof...(Ts));
    }
};

template <class... Ts>
struct sequence_traits<std::tuple<Ts...> const>
    : sequence_traits<std::tuple<Ts...>> {

    using value_type = std::add_const_t<
        typename sequence_traits<std::tuple<Ts...>>::value_type>;
    using reference = std::add_lvalue_reference_t<value_type>;
};

template <class... Ts>
struct sequence_traits<std::tuple<Ts...> volatile>
    : sequence_traits<std::tuple<Ts...>> {

    using value_type = std::add_volatile_t<
        typename sequence_traits<std::tuple<Ts...>>::value_type>;
    using reference = std::add_lvalue_reference_t<value_type>;
};

template <class... Ts>
struct sequence_traits<std::tuple<Ts...> const volatile>
    : sequence_traits<std::tuple<Ts...>> {

    using value_type =
        std::add_cv_t<typename sequence_traits<std::tuple<Ts...>>::value_type>;
    using reference = std::add_lvalue_reference_t<value_type>;
};

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP
#line 16 "raw_view.hpp"
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class S BOOST_STATIC_VIEWS_REQUIRES(
    Sequence<S>) struct raw_view_impl {

  private:
    using sequence_type = S;
    using traits_type   = sequence_traits<sequence_type>;
    S* _xs;

  public:
    using value_type      = typename traits_type::value_type;
    using reference       = typename traits_type::reference;
    using size_type       = typename traits_type::size_type;
    using index_type      = typename traits_type::index_type;
    using difference_type = typename traits_type::difference_type;

    // TODO: Enabling a default constructor would make raw_view_impl trivial,
    // but unsafe. If you have a strong preference for one or another, feel free
    // to submit an issue.
    // constexpr raw_view_impl() noexcept = default;

    constexpr explicit raw_view_impl(S& xs) noexcept : _xs{&xs} {}

    constexpr raw_view_impl(raw_view_impl const&) noexcept = default;

    constexpr raw_view_impl(raw_view_impl&&) noexcept = default;

    constexpr raw_view_impl& operator=(raw_view_impl const&) noexcept = default;

    constexpr raw_view_impl& operator=(raw_view_impl&&) noexcept = default;

    static constexpr auto extent() noexcept
    {
        return sequence_traits<sequence_type>::extent();
    }

    // This annotation is important, because we could be dereferencing a nullptr
    // which is UB and not really "pure" :)
    // However, per construction this never happens, so we're safe.
    BOOST_STATIC_VIEWS_PURE
    constexpr auto size() const noexcept
    {
        BOOST_STATIC_VIEWS_ASSUME(_xs != nullptr);
        return sequence_traits<sequence_type>::size(*_xs);
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto unsafe_at(index_type const i) const
        // clang-format off
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        sequence_traits<sequence_type>::at(*_xs, i)
    );
    // clang-format on

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr decltype(auto) operator[](index_type const i) const
    {
        if (BOOST_STATIC_VIEWS_UNLIKELY(
                0 > i || static_cast<size_type>(i) >= size())) {
            make_out_of_bound_error(
                "Precondition `0 <= i < size()` not satisfied in "
                "boost::static_views::raw_view::operator[].");
        }
        return unsafe_at(i);
    }
};

/// \cond
struct raw_view_fn {
  public:
    // clang-format off
    template <class S
        BOOST_STATIC_VIEWS_REQUIRES(Sequence<S>)
    constexpr auto operator()(S& sequence) const
        // clang-format on
        noexcept
    {
        return raw_view_impl<S>{sequence};
    }
};
/// \endcond

} // namespace detail

/// \verbatim embed:rst:leading-slashes
/// :math:`\mathtt{raw\_view} : \mathtt{Sequence} \to \mathtt{View}`
/// (i.e.  takes a :ref:`sequence <sequence-concept>` and returns a
/// :ref:`view <view-concept>`). It will trigger ``static_assert``
/// failures if passed a type not modeling the :ref:`sequence
/// <sequence-concept>` concept.
/// \endverbatim
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::raw_view_fn, raw_view)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_RAW_VIEW_HPP
#line 1 "slice.hpp"
//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_SLICE_HPP
#define BOOST_STATIC_VIEWS_SLICE_HPP

#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class DropTake BOOST_STATIC_VIEWS_REQUIRES(
    View<DropTake>) struct slice_view_impl : DropTake {

  private:
    using self_type = slice_view_impl;
    using base      = DropTake;

  public:
    using typename base::index_type;
    using typename base::reference;
    using typename base::size_type;
    using typename base::value_type;

    /// \brief Converts \p xs to "slice view".

    /// \tparam View
    /// \verbatim embed:rst:leading-slashes
    /// Wrapper around a *view* (i.e. ``typename View::type`` must
    /// model the :ref:`view <view-concept>` concept). It is
    /// furthermore assumed that ``View`` is of the form
    /// ``wrapper<drop_impl<wrapper<take_impl<wrapper<V>&&>>>&&>``,
    /// where ``V`` is either lvalue or rvalue reference to some view.
    /// \endverbatim
    /// \param xs **Rvalue** reference to a wrapper around a take view
    /// of a drop view of a view.
    ///
    /// \verbatim embed:rst:leading-slashes
    /// .. note::
    ///   It's pretty much impossible to specify ``DropTake`` and
    ///   ``View`` template parameters correctly. For this reason
    ///   :cpp:var:`slice` factory function is provided. Use it
    ///   instead to construct slice views.
    /// \endverbatim
    constexpr slice_view_impl(DropTake&& xs) BOOST_STATIC_VIEWS_NOEXCEPT_IF(
        std::is_nothrow_move_constructible<DropTake>::value)
        : DropTake{std::move(xs)}
    {
    }

    slice_view_impl(slice_view_impl const&) = default;
    slice_view_impl(slice_view_impl&&)      = default;
    slice_view_impl& operator=(slice_view_impl const&) = default;
    slice_view_impl& operator=(slice_view_impl&&) = default;
#line 113 "slice.hpp"
};

struct slice_impl {
  private:
    template <class T>
    using index_type_t =
        typename std::remove_cv_t<std::remove_reference_t<T>>::index_type;

    template <class DropTake>
    constexpr auto call_impl(DropTake xs) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            slice_view_impl<DropTake>{std::move(xs)});

  public:
    // clang-format off
    template <class V, class Index1, class Index2
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>)
    constexpr
    auto operator()(V&& xs, Index1 const b, Index2 const e)
        // clang-format on
        const BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            noexcept(std::declval<slice_impl const&>().call_impl(
                drop_exactly(take_exactly(std::forward<V>(xs), e), b))))
    {
        return call_impl(drop_exactly(take_exactly(std::forward<V>(xs), e), b));
    }

    template <class Index1, class Index2>
    constexpr auto operator()(Index1 const b, Index2 const e) const noexcept
    {
        return lazy_adaptor(slice_impl{}, copy(b), copy(e));
    }
};
#line 165 "slice.hpp"
} // end namespace detail

/// \verbatim embed:rst:leading-slashes
/// :math:`\mathtt{slice} : \mathbb{N} \times \mathbb{N} \to
/// \text{View} \to \text{View}` is a functor that let's you create
/// slices of views. Given a index :math:`b \in \mathbb{N}`, an index
/// :math:`e \in \mathbb{N}` and a :ref:`view <view-concept>`
/// :math:`xs`, creates a view of elements of :math:`xs` with indices
/// :math:`i \in \mathbb{N} \cap [b, e)`. \endverbatim
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::slice_impl, slice)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_SLICE_HPP
#line 1 "through.hpp"
//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_THROUGH_HPP
#define BOOST_STATIC_VIEWS_THROUGH_HPP

#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
template <class Wrapper, class Proxy>
struct through_view_impl
    : public view_adaptor_base<through_view_impl<Wrapper, Proxy>, Wrapper>
    // TODO: I'm not sure if this is safe, i.e. could the user come up with a
    // really tricky combination of views such that view_adaptor_base<...> would
    // end up deriving from Proxy too?
    , private Proxy {

  private:
    using wrapper_type    = Wrapper;
    using proxy_type      = Proxy;
    using proxy_view_type = typename proxy_type::value_type;
    using base = view_adaptor_base<through_view_impl<Wrapper, Proxy>, Wrapper>;

    constexpr auto const& proxy() const& noexcept
    {
        return static_cast<Proxy const&>(*this).get();
    }

    constexpr auto& proxy() & noexcept
    {
        return static_cast<Proxy&>(*this).get();
    }

    constexpr auto proxy()
        && BOOST_STATIC_VIEWS_NOEXCEPT_IF(
               std::is_nothrow_move_constructible<proxy_view_type>::value)
    {
        return static_cast<Proxy&&>(*this).get();
    }

  public:
    using index_type = typename proxy_view_type::index_type;
    using typename base::reference;
    using typename base::size_type;
    using typename base::value_type;

    /// \brief Constructs a view of \p xs through \p proxy.

    /// \tparam View
    /// \verbatim embed:rst:leading-slashes
    /// Wrapper around a view, i.e. ``typename View::type`` is a view
    /// and
    /// must model the :ref:`view <view-concept>` concept.
    /// \endverbatim
    /// \tparam Proxy
    /// \verbatim embed:rst:leading-slashes
    /// Just like ``View``, a wrapper around a view, i.e.
    /// ``typename Proxy::type`` must be a view. Furthermore, viewed
    /// elements must be of type ``std::size_t``.
    /// \endverbatim
    /// \param xs    Rvalue reference to a wrapper around a view.
    /// \param proxy Rvalue reference to a wrapper around a view. It
    /// is
    ///              used as a proxy to access elements of \p xs, i.e.
    ///              if `ys = through_impl{xs, proxy}`, then
    ///              \f[
    ///                 \text{ys}[i] = \text{xs.get}()\!\left[
    ///                     \text{proxy.get}()[i]\right]\;,
    ///                 \forall i \in \{0,1,\dots,\text{xs.size}() -
    ///                 1\}.
    ///              \f]
    ///
    /// \verbatim embed:rst:leading-slashes
    /// .. note::
    ///   It's annoying to have to specify the ``View`` and ``Proxy``
    ///   template parameters all the time. For this reason a
    ///   :cpp:var:`through` factory function is provided. Use it
    ///   instead
    ///   to construct views through other views.
    /// \endverbatim
    constexpr through_view_impl(Wrapper&& xs, Proxy&& proxy)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_constructible<base, Wrapper&&>::value&&
                std::is_nothrow_move_constructible<Proxy>::value)
        : base{std::move(xs)}, proxy_type{std::move(proxy)}
    {
    }

    /// \brief Returns the capacity of this view.

    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. It just calls ``capacity()`` on the proxy view. This
    /// function never fails.
    /// \endverbatim
    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return proxy_view_type::extent();
    }

    /// \brief Returns the number of elements viewed.

    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. It just calls ``size()`` on the proxy view. This
    /// function never fails unless a call to ``proxy.size()``
    /// fails.
    /// \endverbatim
    constexpr auto size() const noexcept { return proxy().size(); }

    /// \brief "Maps" index \p i to the corresponding index in the
    /// parent
    /// view. Actually, it just calls ``operator[]`` on the proxy
    /// view.

    /// Let `ys` be of type #through_impl, i.e. `ys = ` #through
    /// `(proxy)(xs)`. The following relation then holds
    ///
    /// `ys.` #map `(i) = proxy[i],`
    ///     \f$\forall i \in \{0, 1, \dots,
    ///     \text{proxy.size}()-1\}.\f$
    constexpr auto map(index_type const i) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            noexcept(std::declval<proxy_view_type const&>().unsafe_at(
                std::declval<index_type>())))
    {
        BOOST_STATIC_VIEWS_EXPECT(0 <= i && i < static_cast<index_type>(size()),
            "boost::static_views::through_impl::map: Precondition "
            "`0 <= i < size()` is not satisfied.");
        return proxy().unsafe_at(i);
    }
};

struct through_impl {
  private:
    template <class Wrapper, class Proxy>
    constexpr auto call_impl(Wrapper xs, Proxy ys) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            through_view_impl<Wrapper, Proxy>{std::move(xs), std::move(ys)});

  public:
    // TODO; Should through_view be checking that operator[] of P actually
    // returns something convertible to V::index_type?
    // clang-format off
    template <class V, class P
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>
         && View<std::remove_cv_t<std::remove_reference_t<P>>>)
    constexpr
    auto operator()(V&& xs, P&& proxy) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
            noexcept(std::declval<through_impl const&>().call_impl(
                make_wrapper(std::forward<V>(xs)),
                make_wrapper(std::forward<P>(proxy)))))
    {
        return call_impl(make_wrapper(std::forward<V>(xs)),
            make_wrapper(std::forward<P>(proxy)));
    }
};
#line 190 "through.hpp"
} // end namespace detail

/// \brief A functor for creating "through views"

/// \f[
/// \text{through} : \text{View} \to \text{View} \to \text{View}
/// \f]
///
/// \verbatim embed:rst:leading-slashes
/// Given a view ``proxy`` and a view ``xs``, creates a view of ``xs``
/// through
/// ``proxy``. Type of ``xs`` may be anything as long as it models the
/// :ref:`view <view-concept>` concept. ``proxy`` must be a view of
/// elements of type ``std::size_t``. The exact type of the returned
/// view is an implementation detail. What's important is that it also
/// models the :ref:`view <view-concept>` concept.
///
/// .. note::
///   Haskell notation is used here, i.e. the function is curried and
///   :math:`\text{through}(\text{proxy}) : \text{View} \to
///   \text{View}` models the :ref:`algorithm <algorithm-concept>`
///   concept.
///
/// \endverbatim
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::through_impl, through)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_THROUGH_HPP
#line 17 "static_views/hashed.hpp"
#include <type_traits>
#include <iostream>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
#line 70 "static_views/hashed.hpp"
#if defined(BOOST_STATIC_VIEWS_CONCEPTS)
template <class H, class T>
concept bool               Hasher =
    MoveConstructible<H>&& is_invocable_r<std::size_t, H, T>::value;
#else
template <class H, class T>
constexpr bool             Hasher =
    MoveConstructible<H>&& is_invocable_r<std::size_t, H, T>::value;
#endif
#line 180 "static_views/hashed.hpp"
template <std::size_t BucketCount, std::size_t BucketSize, class Wrapper,
    class Hasher>
struct hashed_view_impl
    : view_adaptor_base<
          hashed_view_impl<BucketCount, BucketSize, Wrapper, Hasher>, Wrapper> {

  private:
    using view_type = typename Wrapper::value_type;

    static_assert(
        is_wrapper<Hasher>(), "[INTERNAL] Invalid use of hashed_impl.");

    using base = view_adaptor_base<
        hashed_view_impl<BucketCount, BucketSize, Wrapper, Hasher>, Wrapper>;
    using base_index_type = typename base::index_type;

  public:
    using hasher_type = typename Hasher::value_type;
    using index_type  = std::size_t;
    using typename base::size_type;

    using base::parent;

    /// \brief Returns the number buckets.
    static constexpr auto bucket_count() noexcept
    {
        static_assert(BucketCount > 0,
            "Zero-sized boost::static_views::hashed_view is not supported.");
        return static_cast<size_type>(BucketCount);
    }

    /// \brief Returns the capacity of each bucket.
    static constexpr auto bucket_size() noexcept
    {
        static_assert(BucketSize > 0, "Zero-sized buckets are not supported by "
                                      "boost::static_views::hashed_view.");
        return static_cast<size_type>(BucketSize);
    }

  private:
    using storage_type = base_index_type[bucket_count() * bucket_size()];
    struct array_wrapper {
        storage_type unpack;
    };

    std::tuple<array_wrapper, Hasher> _data;

    // Returns whether a bucket at given index is empty.
    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr static auto empty(base_index_type const x) noexcept -> bool
    {
        return x
               == static_cast<base_index_type>(bucket_count() * bucket_size());
    }

    struct is_empty {
        constexpr auto operator()(base_index_type const x) const noexcept
            -> bool
        {
            return empty(x);
        }
    };

    constexpr auto storage() const & noexcept -> storage_type const&
    {
        return std::get<0>(_data).unpack;
    }

    constexpr auto storage() & noexcept -> storage_type&
    {
        return std::get<0>(_data).unpack;
    }

    constexpr auto bucket(base_index_type const i) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    {
        // Note that i absolutely must not exceed the size of storage.
        return take_exactly(drop_exactly(raw_view(storage()), i),
            std::integral_constant<base_index_type, bucket_size()>{});
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto insert(typename base::index_type const i,
        typename base::index_type const                   guess) -> void
    {
        auto const p = find_first_i(bucket(guess), is_empty{});
        if (p != bucket_size()) { storage()[guess + p] = i; }
        else {
            make_full_bucket_error("Bucket is full.");
        }
    }

    /// \brief Finds an element in a bucket at the given index which satisfies
    /// the given predicate.
    // clang-format off
    template <class Predicate
        BOOST_STATIC_VIEWS_REQUIRES(
            is_invocable_r<bool, Predicate, typename base::reference>::value)
    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr
    auto find_in_bucket(typename base::index_type const h, Predicate&& p)
              // clang-format on
              BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                  noexcept(invoke(p, std::declval<typename base::reference>())))
                  ->typename base::value_type*
    {
        auto const xs = bucket(h);
        for (typename base::index_type i = 0;
             i < bucket_size() && !empty(xs.unsafe_at(i)); ++i) {
            // i -- index in the bucket
            // xs.unsafe_at(i) -- index in the parent view
            // parent().unsafe_at(xs.unsafe_at(i)) -- element in the parent
            // view.
            auto& x = parent().unsafe_at(xs.unsafe_at(i));
            if (invoke(p, x)) return &x;
        }
        return nullptr;
    }

    // clang-format off
    template <class Equal>
    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr
    auto insert(typename base::index_type const i,
        typename base::index_type const guess, Equal& equal) -> void
    // clang-format on
    {
        struct Predicate {
            Equal&                                     _equal;
            std::add_const_t<typename base::reference> _value;

            constexpr auto operator()(typename base::reference x)
            {
                return invoke(_equal, _value, x);
            }
        };

        if (find_in_bucket(guess, Predicate{equal, parent().unsafe_at(i)})
            != nullptr) {
            // We already have this element.
            return;
        }
        auto const p = find_first_i(bucket(guess), is_empty{});
        if (p != bucket_size()) { storage()[guess + p] = i; }
        else {
            make_full_bucket_error("Bucket is full.");
        }
    }

    constexpr auto initialise_storage()
    {
        using I = typename base::index_type;
        for (std::size_t i = 0; i < bucket_count() * bucket_size(); ++i)
            storage()[i] = bucket_count() * bucket_size();

        for (I i = 0, size = static_cast<I>(parent().size()); i < size; ++i) {
            auto const hash = invoke(hash_function(), parent().unsafe_at(i));
            insert(i, static_cast<I>(bucket_size() * (hash % bucket_count())));
        }
    }

    template <class Equal>
    constexpr auto initialise_storage(Equal&& equal)
    {
        using I = typename base::index_type;
        for (std::size_t i = 0; i < bucket_count() * bucket_size(); ++i)
            storage()[i] = bucket_count() * bucket_size();

        for (I i = 0, size = static_cast<I>(parent().size()); i < size; ++i) {
            auto const hash = invoke(hash_function(), parent().unsafe_at(i));
            insert(i, static_cast<I>(bucket_size() * (hash % bucket_count())),
                equal);
        }
    }

  public:
    /// \brief Constructs a hashed view of \p xs using \p hf as a hash
    /// function.

    /// \tparam BucketCount
    /// Number of buckets.
    /// \tparam BucketSize
    /// Capacity of each bucket.
    /// \tparam View
    /// \verbatim embed:rst:leading-slashes
    /// Wrapper around a view, i.e. ``typename View::type`` is a view
    /// and
    /// must model the :ref:`view <view-concept>` concept.
    /// \endverbatim
    /// \param xs      Rvalue reference to a wrapper around a view.
    /// \param hf      Hash function to use.
    /// \param storage Array of indices initialised by
    /// #hashed_init_impl.
    ///
    ///
    /// \verbatim embed:rst:leading-slashes
    /// .. note::
    ///   This function requires an initialiser storage as an argument
    ///   which
    ///   is an implementation detail. This constructor is thus not
    ///   meant to
    ///   be used explicitly, use the :cpp:var:`hashed` factory
    ///   function instead.
    /// \endverbatim
    constexpr hashed_view_impl(Wrapper&& xs, Hasher&& hf)
        : base{std::move(xs)}, _data{array_wrapper{}, std::move(hf)}
    {
        initialise_storage();
    }

    template <class Equal>
    constexpr hashed_view_impl(Wrapper&& xs, Hasher&& hf, Equal&& equal)
        : base{std::move(xs)}, _data{array_wrapper{}, std::move(hf)}
    {
        initialise_storage(std::forward<Equal>(equal));
    }

    /// \brief Returns the capacity of the view.

    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. Hashed view is a view of buckets rather than
    /// individual elements. This function is thus equivalent to
    /// :cpp:func:`bucket_count()
    /// <detail::hashed_impl::bucket_count()>`.
    /// \endverbatim
    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return static_cast<std::ptrdiff_t>(bucket_count());
    }

    /// \brief Returns a reference to the hash function.
    constexpr auto const& hash_function() const& noexcept
    {
        return std::get<1>(_data).get();
    }

    /// \brief Returns the bucket corresponding to \p hash.

    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. It returns a view of elements that have hash
    /// ``hash``. If there are no such elements, the returned view
    /// will have size zero. \endverbatim
    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto operator[](index_type const hash)
        const& BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    {
        auto const i = static_cast<typename base::index_type>(
            bucket_size() * (hash % bucket_count()));
        auto const n = find_first_i(bucket(i), is_empty{});
        return through(parent(), slice(raw_view(storage()), i, i + n));
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto unsafe_at(index_type const hash)
        const& BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    {
        return this->operator[](hash);
    }

    using reference = decltype(
        std::declval<hashed_view_impl const&>()[std::declval<index_type>()]);
    using value_type = std::remove_reference_t<reference>;

    template <class Predicate>
    BOOST_STATIC_VIEWS_FORCEINLINE constexpr auto lookup(index_type h,
        Predicate&& p) const noexcept -> typename value_type::value_type*
    {
        auto const xs = bucket(h);
        for (typename base::index_type i = 0;
             i < bucket_size() && !empty(xs.unsafe_at(i)); ++i) {

            auto* const x = &parent().unsafe_at(xs.unsafe_at(i));
            if (invoke(p, *x)) return x;
        }
        return nullptr;
    }
};

template <std::size_t BucketCount, std::size_t BucketSize = 4u>
struct hashed_impl {

  private:
    template <class Wrapper, class Hasher>
    constexpr auto call_impl(Wrapper xs, Hasher hf) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            hashed_view_impl<BucketCount, BucketSize, Wrapper, Hasher>{
                std::move(xs), std::move(hf)});

    template <class Wrapper, class Hasher, class Equal>
    constexpr auto call_impl(Wrapper xs, Hasher hf, Equal&& equal) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            hashed_view_impl<BucketCount, BucketSize, Wrapper, Hasher>{
                std::move(xs), std::move(hf), std::forward<Equal>(equal)});

    static_assert(BucketCount > 0,
        "`boost::static_views::hashed<BucketCount, BucketSize>` "
        "requires BucketCount to be greater than zero.");
    static_assert(BucketCount > 0,
        "`boost::static_views::hashed<BucketCount, BucketSize>` "
        "requires BucketSize to be greater than zero.");

  public:
    // clang-format off
    template <class V, class H
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>
         && Hasher<H, typename std::remove_cv_t<
                          std::remove_reference_t<V>>::reference>)
    constexpr
    auto operator()(V&& xs, H&& hf) const
    // clang-format on
    {
        return call_impl(make_wrapper(std::forward<V>(xs)),
            make_wrapper(std::forward<H>(hf)));
    }

    // clang-format off
    template <class V, class H, class Equal
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>
         && Hasher<H, typename std::remove_cv_t<
                          std::remove_reference_t<V>>::reference>)
    constexpr
    auto operator()(V&& xs, H&& hf, Equal&& equal) const
    // clang-format on
    {
        return call_impl(make_wrapper(std::forward<V>(xs)),
            make_wrapper(std::forward<H>(hf)), std::forward<Equal>(equal));
    }
};
#line 549 "static_views/hashed.hpp"
} // end namespace detail

/// \brief A functor for creating "hashed views"

/// \f[
/// \text{hashed} : (\text{T} \to \mathbb{N}) \to \text{View} \to
/// \text{View} \f]
///
/// Given a hash function `hf` and a view `xs`, creates a view of `xs`
/// which uses hashes as indices to access elements.
///
/// So, say, \f$ \text{hf} : \text{T} \to \mathbb{N} \f$ is a hash
/// function, `xs` is a view of elements of type `T`, and `ys =`
/// #hashed `(hf)(xs)`. Then
///
/// \f[
/// \{ x \in \text{xs} \;\;|\;\;
///     \text{hf}(x) = h \,\%\, \text{ys.bucket_count}() \}
///     = \text{ys}[h] \;, \forall h \in \mathbb{N}\;.
/// \f]
///
/// \verbatim embed:rst:leading-slashes
/// .. note::
///   Haskell notation is used here, i.e. the function is curried and
///   :math:`\text{hashed}(\text{hf}) : \text{View} \to \text{View}`
///   models the :ref:`algorithm <algorithm-concept>` concept.
/// \endverbatim
#if defined(DOXYGEN_IN_HOUSE)
template <std::size_t BucketCount, std::size_t BucketSize>
constexpr auto hashed = implementation detail;
#else
inline namespace {
template <std::size_t BucketCount, std::size_t BucketSize = 4u>
constexpr auto const& hashed = ::boost::static_views::_static_const<
    detail::hashed_impl<BucketCount, BucketSize>>;
} // anonymous namespace
#endif

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_HASHED_HPP
#line 1 "static_views/pipe.hpp"
//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_PIPE_HPP
#define BOOST_STATIC_VIEWS_PIPE_HPP

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
constexpr
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
#line 1 "static_views/static_map.hpp"
//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_STATIC_MAP_HPP
#define BOOST_STATIC_VIEWS_STATIC_MAP_HPP

#include <exception>
#include <type_traits>
#line 1 "detail/compose.hpp"
//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_COMPOSE_HPP
#define BOOST_STATIC_VIEWS_COMPOSE_HPP

#include <algorithm>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
template <class Second, class First>
struct compose_impl
    : private Second
    , private First {

    using first_fn  = typename First::value_type;
    using second_fn = typename Second::value_type;

    constexpr compose_impl(Second&& second, First&& first)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<First>::value&&
                std::is_nothrow_move_constructible<Second>::value)
        : Second{std::move(second)}, First{std::move(first)}
    {
    }

    compose_impl(compose_impl const&) = default;
    compose_impl(compose_impl&&)      = default;
    compose_impl& operator=(compose_impl const&) = default;
    compose_impl& operator=(compose_impl&&) = default;

    constexpr auto const& first() const& noexcept
    {
        return static_cast<First const&>(*this).get();
    }

    constexpr auto const& second() const& noexcept
    {
        return static_cast<Second const&>(*this).get();
    }

    // clang-format off
    template <class... Args
        BOOST_STATIC_VIEWS_REQUIRES(
            is_invocable<first_fn const&, Args...>::value
         && is_invocable<second_fn const&,
                invoke_t<first_fn const&, Args...>>::value)
    constexpr
    decltype(auto) operator()(Args&&... args) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(invoke(std::declval<second_fn const&>(),
                invoke(std::declval<first_fn const&>(),
                    std::forward<Args>(args)...))))
    {
        return invoke(second(), invoke(first(), std::forward<Args>(args)...));
    }
};

struct make_compose {
  private:
    template <class Second, class First>
    constexpr auto call_impl(Second second, First first) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            compose_impl<Second, First>{std::move(second), std::move(first)});

  public:
    // clang-format off
    template <class Second, class First
        BOOST_STATIC_VIEWS_REQUIRES(
            !Same<std::remove_cv_t<std::remove_reference_t<Second>>,
                  std::remove_cv_t<std::remove_reference_t<First>>>)
    constexpr
    auto operator()(Second&& second, First&& first) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<make_compose const&>().call_impl(
                make_wrapper(std::forward<Second>(second)),
                make_wrapper(std::forward<First>(first)))))
    {
        return call_impl(make_wrapper(std::forward<Second>(second)),
            make_wrapper(std::forward<First>(first)));
    }
};

} // end namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_compose, compose)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DROP_HPP
#line 16 "static_views/static_map.hpp"
BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

struct key_not_found_error : public virtual std::exception {
    auto what() const noexcept -> char const* override { return _msg; }

  private:
    static constexpr char const* _msg = "Key not found.";
};
constexpr char const* key_not_found_error::_msg;

namespace detail {

template <class Pred, class GetKey, class GetMapped>
class map_config
    // TODO: This will probably break if get_key and get_mapped functions have
    // the same type...
    : private Pred
    , private GetKey
    , private GetMapped {

  public:
    using key_equal     = typename Pred::value_type;
    using key_getter    = typename GetKey::value_type;
    using mapped_getter = typename GetMapped::value_type;

    constexpr map_config(Pred&& equal, GetKey&& get_key, GetMapped&& get_mapped)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<Pred>::value&&
                    std::is_nothrow_move_constructible<GetKey>::value&&
                    std::is_nothrow_move_constructible<GetMapped>::value)
        : Pred{std::move(equal)}
        , GetKey{std::move(get_key)}
        , GetMapped{std::move(get_mapped)}
    {
    }

    map_config(map_config const&) = default;
    map_config(map_config&&)      = default;
    map_config& operator=(map_config const&) = default;
    map_config& operator=(map_config&&) = default;

    constexpr auto const& equal() const& noexcept
    {
        return static_cast<Pred const&>(*this).get();
    }

    constexpr auto equal()
        && BOOST_STATIC_VIEWS_NOEXCEPT_IF(
               std::is_nothrow_move_constructible<key_equal>::value)
    {
        return static_cast<Pred&&>(*this).get();
    }

    constexpr auto const& key() const& noexcept
    {
        return static_cast<GetKey const&>(*this).get();
    }

    constexpr auto key()
        && BOOST_STATIC_VIEWS_NOEXCEPT_IF(
               std::is_nothrow_move_constructible<key_getter>::value)
    {
        return static_cast<GetKey&&>(*this).get();
    }

    constexpr auto const& mapped() const& noexcept
    {
        return static_cast<GetMapped const&>(*this).get();
    }

    constexpr auto mapped()
        && BOOST_STATIC_VIEWS_NOEXCEPT_IF(
               std::is_nothrow_move_constructible<mapped_getter>::value)
    {
        return static_cast<GetMapped&&>(*this).get();
    }
};

// clang-format off
template <class Pred, class GetKey, class GetMapped>
BOOST_STATIC_VIEWS_FORCEINLINE
constexpr
auto make_map_config_impl(Pred p, GetKey k, GetMapped m)
    BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN
(
    map_config<Pred, GetKey, GetMapped>{
        std::move(p), std::move(k), std::move(m)}
);
// clang-format on

// clang-format off
template <class Pred, class GetKey, class GetMapped>
BOOST_STATIC_VIEWS_FORCEINLINE
constexpr
auto make_map_config(Pred&& p, GetKey&& k, GetMapped&& m)
    BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN
(
    make_map_config_impl(
        make_wrapper(std::forward<Pred>(p)),
        make_wrapper(std::forward<GetKey>(k)),
        make_wrapper(std::forward<GetMapped>(m)))
);
// clang-format on

template <class HashedView, class MapConfig>
class static_map
    : private HashedView
    , private MapConfig {

    using type = static_map<HashedView, MapConfig>;

  public:
    using bucket_type   = typename HashedView::value_type;
    using value_type    = typename bucket_type::value_type;
    using reference     = typename bucket_type::reference;
    using key_equal     = typename MapConfig::key_equal;
    using key_getter    = typename MapConfig::key_getter;
    using mapped_getter = typename MapConfig::mapped_getter;

    static_assert(is_invocable<key_getter, reference>::value,
        "`key_getter` in boost::static_views::static_map must be invocable "
        "with `reference`.");

    using key_type = std::remove_reference_t<decltype(
        invoke(std::declval<key_getter const&>(), std::declval<reference>()))>;

    static_assert(is_invocable<key_getter, reference>::value,
        "`mapped_getter` in boost::static_views::static_map must be "
        "invocable with `reference`.");

    using mapped_type = std::remove_reference_t<decltype(invoke(
        std::declval<mapped_getter const&>(), std::declval<reference>()))>;

    static_assert(is_invocable_r<bool, key_equal, key_type&, key_type&>::value,
        "`key_equal` in boost::static_views::static_map must be invocable "
        "with two `key_type&` and, when invoked, return something "
        "convertible to `bool`..");

    using typename HashedView::size_type;
    using key_hasher = typename HashedView::hasher_type::first_fn;
    using index_type = key_type;

  private:
    using view_type   = HashedView;
    using config_type = MapConfig;

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto const& _hash_function() const noexcept
    {
        // TODO: This is ugly!
        return this->hash_function().second();
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto* _lookup(key_type const& k) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(invoke(std::declval<key_equal const&>(),
                std::declval<key_type const&>(),
                invoke(std::declval<key_getter const&>(),
                    std::declval<reference>())))
            && noexcept(invoke(
                   std::declval<static_map const&>()._hash_function(), k)))
    {
        struct pred_equal {
            key_getter const& get_key;
            key_equal const&  equal;
            key_type const&   key;

            BOOST_STATIC_VIEWS_FORCEINLINE
            constexpr auto operator()(value_type const& y)
                BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                    noexcept(invoke(std::declval<key_equal const&>(),
                        std::declval<key_type const&>(),
                        invoke(std::declval<key_getter const&>(), y)))) -> bool
            {
                return invoke(equal, key, invoke(get_key, y));
            }
        };

        auto const hash =
            view_type::bucket_size()
            // TODO: For optimal performance, I need a way to mark a hash
            // function "trustworthy". Such functions are then assumed to never
            // return values larger that bucker_count() which would avoid a
            // costly `mod` operation.
            * (invoke(_hash_function(), k) % view_type::bucket_count());
        return static_cast<HashedView const&>(*this).lookup(
            hash, pred_equal{this->key(), this->equal(), k});
    }

  public:
    constexpr static_map(view_type&& xs, config_type&& conf)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<view_type>::value&&
                std::is_nothrow_move_constructible<config_type>::value)
        : view_type{std::move(xs)}, config_type{std::move(conf)}
    {
    }

    static_map(static_map const&) = default;
    static_map(static_map&&)      = default;
    static_map& operator=(static_map const&) = default;
    static_map& operator=(static_map&&) = default;

    constexpr auto size() const noexcept -> size_type
    {
        return static_cast<view_type const&>(*this).size();
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto find(key_type const& k) const BOOST_STATIC_VIEWS_NOEXCEPT_IF(
        noexcept(std::declval<static_map const&>()._lookup(k))) -> value_type*
    {
        return _lookup(k);
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr auto count(key_type const& k) const -> size_type
    {
        return _lookup(k) == nullptr ? 0 : 1;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr decltype(auto) at(key_type const& k) const
    {
        auto* x = _lookup(k);
        if (x == nullptr) { throw key_not_found_error{}; }
        return invoke(this->mapped(), *x);
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    constexpr decltype(auto) operator[](key_type const& k) const
    {
        return at(k);
    }

    using view_type::begin;
    using view_type::bucket_count;
    using view_type::bucket_size;
    using view_type::end;
};

} // namespace detail

template <std::size_t BucketCount = 0, std::size_t BucketSize = 0>
struct make_static_map_impl {
  private:
    template <class HashedView, class MapConfig>
    constexpr auto call_impl(HashedView view, MapConfig config) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            detail::static_map<HashedView, MapConfig>{
                std::move(view), std::move(config)});

  public:
    // clang-format off
    template <class View, class GetKey, class GetMapped,
        class KeyEqual = std::equal_to<void>, class Hasher = hash_c>
    constexpr
    auto operator()(View&& xs, GetKey get_key, GetMapped&& get_mapped,
        KeyEqual&& key_equal = KeyEqual{}, Hasher&& hasher = Hasher{})
    // clang-format on
    {
        using view_type = std::remove_cv_t<std::remove_reference_t<View>>;
        static_assert(view_type::extent() != dynamic_extent,
            "For views of unknown size you need to manually specify a non-zero "
            "BucketCount.");

        constexpr auto bucket_count =
            (BucketCount == 0) ? 2 * view_type::extent() : BucketCount;
        constexpr auto bucket_size = (BucketSize == 0) ? 2 : BucketSize;

        static_assert(std::is_copy_constructible<GetKey>::value,
            "Current implemetation of boost::static_views::static_map requires "
            "GetKey to be copy constructible. If you absolutely cannot live "
            "with this, please submit a bug report.");
        auto view = hashed<bucket_count, bucket_size>(std::forward<View>(xs),
            compose(std::forward<Hasher>(hasher), GetKey{get_key}), key_equal);
        auto conf = detail::make_map_config(std::forward<KeyEqual>(key_equal),
            std::move(get_key), std::forward<GetMapped>(get_mapped));

        return call_impl(std::move(view), std::move(conf));
    }
};
#line 356 "static_views/static_map.hpp"
BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_STATIC_MAP_HPP
#line 1 "static_views/transform.hpp"
//          Copyright Tom Westerhout 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/transform.hpp
///
/// \brief Implementation of #transform.

#ifndef BOOST_STATIC_VIEWS_TRANSFORM_HPP
#define BOOST_STATIC_VIEWS_TRANSFORM_HPP

#include <algorithm>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class Wrapper, class Function>
struct transform_view_impl
    : view_adaptor_base<transform_view_impl<Wrapper, Function>, Wrapper>
    , private Function {

  private:
    using wrapper_type = Wrapper;
    using base =
        view_adaptor_base<transform_view_impl<Wrapper, Function>, Wrapper>;
    using function_type = typename Function::value_type;

  public:
    using typename base::index_type;
    using typename base::size_type;
    using reference  = decltype(invoke(std::declval<function_type const&>(),
        std::declval<typename base::reference>()));
    using value_type = std::remove_reference_t<reference>;
    using base::parent;
    using base::size;

    constexpr transform_view_impl(Wrapper&& xs, Function&& func)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_constructible<base, Wrapper&&>::value&&
                std::is_nothrow_move_constructible<Function>::value)
        : base{std::move(xs)}, Function{std::move(func)}
    {
    }

    transform_view_impl(transform_view_impl const&) = default;
    transform_view_impl(transform_view_impl&&)      = default;
    transform_view_impl& operator=(transform_view_impl const&) = default;
    transform_view_impl& operator=(transform_view_impl&&) = default;

    constexpr decltype(auto) unsafe_at(index_type const i) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    {
        BOOST_STATIC_VIEWS_EXPECT(0 <= i && i < static_cast<index_type>(size()),
            "boost::static_views::transform_view_impl::map: Precondition "
            "`0 <= i < size()` is not satisfied.");
        return invoke(
            static_cast<Function const&>(*this).get(), parent().unsafe_at(i));
    }
};

struct transform_impl {
  private:
    template <class Wrapper, class Function>
    constexpr auto call_impl(Wrapper xs, Function func) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            transform_view_impl<Wrapper, Function>{
                std::move(xs), std::move(func)});

    template <class T>
    using index_t =
        typename std::remove_cv_t<std::remove_reference_t<T>>::index_type;

    // clang-format off
    template <class V, class F
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>
        && is_invocable<F, typename std::remove_cv_t<
                                    std::remove_reference_t<
                                        V>>::reference>::value)
    constexpr auto call(V&& xs, F&& f, int /*unused*/) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<transform_impl const&>().call_impl(
                make_wrapper(std::forward<V>(xs)),
                make_wrapper(std::forward<F>(f)))))
    {
        return call_impl(make_wrapper(std::forward<V>(xs)),
            make_wrapper(std::forward<F>(f)));
    }

#if !defined(BOOST_STATIC_VIEWS_SFINAE)
    template <class V, class F>
    constexpr auto call(V&& /*unused*/, F&& /*unused*/, ...) const noexcept
    {
        static_assert(View<std::remove_cv_t<std::remove_reference_t<V>>>,
            "`boost::static_views::transform(xs, func)` requires xs to model "
            "the View concept.");
        static_assert(
            is_invocable<F, typename std::remove_cv_t<
                                std::remove_reference_t<V>>::reference>::value,
            "`boost::static_views::transform(xs, func)` requires func to be "
            "invocable with an element of xs.");
    }
#endif

  public:
    template <class V, class F>
    constexpr auto operator()(V&& xs, F&& f) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<transform_impl const&>().call(
                std::forward<V>(xs), std::forward<F>(f))))
    {
        return call(std::forward<V>(xs), std::forward<F>(f), int{});
    }

    template <class F>
    constexpr auto operator()(F&& func) const noexcept
    {
        return lazy_adaptor(transform_impl{}, std::forward<F>(func));
    }
};

} // end namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::transform_impl, transform)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_TRANSFORM_HPP
#line 30 "include/boost/static_views.hpp"
#endif // BOOST_STATIC_VIEWS_STATIC_VIEWS_HPP
