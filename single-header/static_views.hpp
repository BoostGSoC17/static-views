//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_ALL_IN_ONE_HPP
#define BOOST_STATIC_VIEWS_ALL_IN_ONE_HPP

#include <limits>
#include <iterator>
#include <cassert>
#include <type_traits>
#include <tuple>
#include <array>
#include <algorithm>
#include <functional>
#include <exception>
#include <utility>



//////////////////////////////////////////////////////////////////////
// Expanded from 'static_views.hpp'
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Expanded from 'algorithm_base.hpp'
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Expanded from 'config.hpp'
//////////////////////////////////////////////////////////////////////


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
    ((!!(cond)) ? static_cast<void>(0) : __builtin_unreachable())

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


//////////////////////////////////////////////////////////////////////
// Expanded from 'utils.hpp'
//////////////////////////////////////////////////////////////////////


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

namespace utils {

    constexpr auto all() noexcept -> bool { return true; }

    template <class... Bools>
    constexpr auto all(bool const x, Bools... xs) noexcept -> bool
    {
        return x && all(xs...);
    }

} // namespace utils

#if defined(__cpp_lib_void_t) && __cpp_lib_void_t >= 201411
using std::void_t;
#else
template <class...>
using void_t = void;
#endif

struct nonesuch {
    nonesuch()                = delete;
    nonesuch(nonesuch const&) = delete;
    nonesuch(nonesuch&&)      = delete;
    nonesuch& operator=(nonesuch const&) = delete;
    nonesuch& operator=(nonesuch&&) = delete;
};

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

template <template <class...> class Op, class... Args>
using is_detected =
    typename detector<nonesuch, void, Op, Args...>::value_t;

template <template <class...> class Op, class... Args>
using detected_t =
    typename detector<nonesuch, void, Op, Args...>::type;

} // namespace detail

namespace concepts {

using ::BOOST_STATIC_VIEWS_NAMESPACE::detail::detected_t;
using ::BOOST_STATIC_VIEWS_NAMESPACE::detail::is_detected;

template <class...>
struct and_;

template <class...>
struct all_;

template <>
struct and_<> {
    template <class T>
    static constexpr auto test() noexcept -> bool
    {
        return true;
    }

    template <class T>
    static constexpr auto check() noexcept -> bool
    {
        return true;
    }
};

template <class C1, class... C2>
struct and_<C1, C2...> {

  private:
    template <class T,
        class = std::enable_if_t<C1::template test<T>()>,
        class = void>
    static constexpr auto test_impl() noexcept -> bool
    {
        return and_<C2...>::template test<T>();
    }

    template <class T,
        class = std::enable_if_t<!C1::template test<T>()>>
    static constexpr auto test_impl() noexcept -> bool
    {
        return false;
    }

    template <class T,
        class = std::enable_if_t<C1::template test<T>()>,
        class = void>
    static constexpr auto check_impl() noexcept -> bool
    {
        return and_<C2...>::template check<T>();
    }

    template <class T,
        class = std::enable_if_t<!C1::template test<T>()>>
    static constexpr auto check_impl() noexcept -> bool
    {
        C1::template check<T>();
        return false;
    }

  public:
    template <class T>
    static constexpr auto test() noexcept -> bool
    {
        return test_impl<T>();
    }

    template <class T>
    static constexpr auto check() noexcept -> bool
    {
        return check_impl<T>();
    }
};

template <>
struct all_<> {
    template <class T>
    static constexpr auto test() noexcept -> bool
    {
        return true;
    }

    template <class T>
    static constexpr auto check() noexcept -> bool
    {
        return true;
    }
};

template <class C1, class... C2>
struct all_<C1, C2...> {
    template <class T>
    static constexpr auto test() noexcept -> bool
    {
        return C1::template test<T>()
               && all_<C2...>::template test<T>();
    }

    template <class T>
    static constexpr auto check() noexcept -> bool
    {
        return C1::template check<T>()
               && all_<C2...>::template check<T>();
    }
};

#define BOOST_STATIC_VIEWS_DEFINE_CHECK(name, T, expr, msg)          \
    struct name {                                                    \
        template <class T>                                           \
        static constexpr auto test() noexcept -> bool                \
        {                                                            \
            return expr;                                             \
        }                                                            \
                                                                     \
        template <class T>                                           \
        static constexpr auto check() noexcept -> bool               \
        {                                                            \
            constexpr bool x = test<T>();                            \
            static_assert(x, msg);                                   \
            return x;                                                \
        }                                                            \
    } /**/

} // namespace concepts

BOOST_STATIC_VIEWS_END_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Expanded from 'wrapper.hpp'
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Expanded from 'invoke.hpp'
//////////////////////////////////////////////////////////////////////


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

/// \brief Trait that determines whether `T` is a reference_wrapper.
template <class T>
struct is_reference_wrapper : std::false_type {
};

/// \cond
template <class T>
struct is_reference_wrapper<std::reference_wrapper<T>>
    : std::true_type {
};
/// \endcond

#if 0 // Current implementations of invoke are not constexpr
.
// #if defined(__cpp_lib_invoke) && (__cpp_lib_invoke >= 201411)
// We have access to std::invoke, let's use it.

struct invoke_impl {
    template <class Function, class... Args>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) operator()(
        Function&& f, Args&&... args) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::invoke(std::forward<Function>(f),
                std::forward<Args>(args)...)))
    {
        return std::invoke(std::forward<Function>(f),
            std::forward<Args>(args)...);
    }
};

#else // no C++17 invoke available, let's implement our own.
// clang-format off


template <class Any, class Pointer, class Object, class... Args>
BOOST_STATIC_VIEWS_FORCEINLINE
BOOST_STATIC_VIEWS_CONSTEXPR
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
BOOST_STATIC_VIEWS_CONSTEXPR
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
BOOST_STATIC_VIEWS_CONSTEXPR
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
BOOST_STATIC_VIEWS_CONSTEXPR
auto invoke_member_func
    ( Function (T::*f)
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
BOOST_STATIC_VIEWS_CONSTEXPR
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
BOOST_STATIC_VIEWS_CONSTEXPR
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
BOOST_STATIC_VIEWS_CONSTEXPR
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
BOOST_STATIC_VIEWS_CONSTEXPR
auto invoke_member_data
    ( Function (T::*f)
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
BOOST_STATIC_VIEWS_CONSTEXPR
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
            std::is_member_object_pointer<Function(T::*)>::value>>
    BOOST_STATIC_VIEWS_FORCEINLINE
    static BOOST_STATIC_VIEWS_CONSTEXPR
    auto call_impl(Function(T::*f), Object&& obj)
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        invoke_member_data(f, std::forward<Object>(obj))
    );

    template <class Function, class T, class Object, class... Args,
        class = std::enable_if_t<
            std::is_member_function_pointer<Function(T::*)>::value>>
    BOOST_STATIC_VIEWS_FORCEINLINE
    static BOOST_STATIC_VIEWS_CONSTEXPR
    auto call_impl(Function(T::*f), Object&& obj, Args&&... args)
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        invoke_member_func(f, std::forward<Object>(obj),
            std::forward<Args>(args)...)
    );

    template <class Function, class... Args,
        class = std::enable_if_t<
            !std::is_member_pointer<std::decay_t<Function>>::value>>
    BOOST_STATIC_VIEWS_FORCEINLINE
    static BOOST_STATIC_VIEWS_CONSTEXPR
    auto call_impl(Function&& f, Args&&... args)
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        invoke_nonmember(
            std::forward<Function>(f), std::forward<Args>(args)...)
    );

  public:
    template <class Function, class... Args>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Function&& f, Args&&... args) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        call_impl(std::forward<Function>(f), std::forward<Args>(args)...)
    );
};

#endif // if C++17
// clang-format on

} // end namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::invoke_impl, invoke)

BOOST_STATIC_VIEWS_END_NAMESPACE


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

/// \brief Wraps an lvalue or an rvalue reference.

// clang-format off
/// \verbatim embed:rst:leading-slashes
/// This is basically a ``constexpr`` version of
/// |reference_wrapper|_. The difference is that
/// :cpp:class:`detail::wrapper` can wrap both lvalue and rvalue
/// references. This is just syntactic sugar.
///
/// .. |reference_wrapper| replace:: ``std::reference_wrapper``
/// .. _reference_wrapper: http://en.cppreference.com/w/cpp/utility/functional/reference_wrapper
///
/// .. code-block:: cpp
///
///   template <class T>
///   struct wrapper {
///       ... // copy and move constructors/assignment operators
///
///       constexpr auto get() const& noexcept(whenever possible);
///       constexpr auto get() &      noexcept(whenever possible);
///       constexpr auto get() &&     noexcept(whenever possible);
///
///       template <class... Args>
///       constexpr auto operator() (Args&&...) const
///           noexcept(whenever possible);
///   };
///
/// Constructors are intentionally not listed. Use
/// :cpp:var:`make_wrapper(T&&) <make_wrapper>` to create wrappers.
/// \endverbatim
// clang-format on
template <class T>
struct wrapper;

template <class T>
struct is_wrapper : std::false_type {
};

template <class T>
struct is_wrapper<wrapper<T>> : std::true_type {
};

/// \cond
template <class T>
struct wrapper<T&> {
    using type = T;

    explicit BOOST_STATIC_VIEWS_CONSTEXPR wrapper(type& x) noexcept
        : _payload{&x}
    {
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    wrapper(wrapper const& other) noexcept = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    wrapper(wrapper&&) noexcept = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    wrapper& operator=(wrapper const& other) noexcept = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    wrapper& operator=(wrapper&&) noexcept = default;

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto get() const noexcept
        -> type const&
    {
        return *_payload;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto get() noexcept -> type&
    {
        return *_payload;
    }

    /*
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR operator type&() const noexcept
    {
        return get();
    }
    */

    /*
    template <class... Args>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) operator()(Args&&... args) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(invoke(
            std::declval<T const&>(), std::declval<Args&&>()...)))
    {
        return invoke(*_payload, std::forward<Args>(args)...);
    }
    */

  private:
    T* _payload;
};
/// \endcond

/// \cond
template <class T>
struct wrapper<T&&> {
    using type = T;

    static_assert(std::is_move_constructible<T>::value,
        "wrapper<T&&>: I'm refusing to work with non-movable types. "
        "If you have problems with that, submit an issue here "
        "https://github.com/BoostGSoC17/static-views/issues.");

    explicit BOOST_STATIC_VIEWS_CONSTEXPR wrapper(type&& x)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<type>::value)
        : _payload{std::move(x)}
    {
    }

    // clang-format off
    BOOST_STATIC_VIEWS_CONSTEXPR wrapper(wrapper const&)            = default;
    BOOST_STATIC_VIEWS_CONSTEXPR wrapper(wrapper &&)                = default;
    BOOST_STATIC_VIEWS_CONSTEXPR wrapper& operator=(wrapper const&) = default;
    BOOST_STATIC_VIEWS_CONSTEXPR wrapper& operator=(wrapper &&)     = default;
    // clang-format on

#if 0 // defined(BOOST_STATIC_VIEWS_GCC) // Doesn't help anymore...
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto get() const & BOOST_STATIC_VIEWS_NOEXCEPT_IF(
        std::is_nothrow_copy_constructible<type>::value) -> type
    {
        return _payload;
    }
#else
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto get() & noexcept -> type& { return _payload; }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto get() const & noexcept -> type const& { return _payload; }
#endif

    // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto get() &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<T>::value) -> type
    {
        return std::move(_payload);
    }
    // clang-format on

    /*
    BOOST_STATIC_VIEWS_CONSTEXPR operator type() const noexcept
    {
        return get();
    }

    BOOST_STATIC_VIEWS_CONSTEXPR operator type() noexcept
    {
        return get();
    }
    */

    // clang-format off
    /*
    template <class... Args>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) operator()(Args&&... args) &
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(invoke(std::declval<wrapper&>().get(),
                std::declval<Args&&>()...)))
    {
        return invoke(std::forward<wrapper>(*this).get(),
            std::forward<Args>(args)...);
    }
    */
    // clang-format on

    // clang-format off
    /*
    template <class... Args>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) operator()(Args&&... args) const&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(invoke(std::declval<wrapper const&>().get(),
                std::declval<Args&&>()...)))
    {
        return invoke(std::forward<wrapper>(*this).get(),
            std::forward<Args>(args)...);
    }
    */
    // clang-format on

    // clang-format off
    /*
    template <class... Args>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) operator()(Args&&... args) &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(invoke(std::declval<wrapper&&>().get(),
                std::declval<Args&&>()...)))
    {
        return invoke(std::forward<wrapper>(*this).get(),
            std::forward<Args>(args)...);
    }
    */
    // clang-format on

  private:
    T _payload;
};
/// \endcond

struct make_wrapper_impl {

  private:
    // If an rvalue of type T is passed to make_wrapper, require T to
    // be move-constructible.
    template <class T>
    using reference_or_move_constructible = std::integral_constant<
        bool,
        std::is_lvalue_reference<T>::value
            || std::is_move_constructible<std::decay_t<T>>::value>;

    // clang-format off
    template <class T>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(T&& /*unused*/,
        std::false_type /*T is not move constructible*/) const noexcept
    {
        static_assert(std::is_rvalue_reference<T&&>::value
            && !std::is_move_constructible<T>::value,
            "Congratulations, you've found a bug in the StaticViews "
            "library. Please, be so kind to submit it here "
            "https://github.com/BoostGSoC17/static-views/issues.");
        static_assert(std::is_move_constructible<T>::value,
            "`boost::static_views::make_wrapper(T&&)` requires `T` "
            "to be move constructible.");
    }
    // clang-format on

    // clang-format off
    template <class T>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(T&& x,
        std::true_type /*all good*/) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(detail::wrapper<decltype(x)>{std::forward<T>(x)}))
    {
        return detail::wrapper<decltype(x)>{std::forward<T>(x)};
    }
    // clang-format on

  public:
    // clang-format off
    template <class T>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(T&& x) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<make_wrapper_impl const&>().call_impl(
                std::forward<T>(x),
                reference_or_move_constructible<decltype(x)>{})
       ))
    {
        return call_impl(std::forward<T>(x),
            reference_or_move_constructible<decltype(x)>{});
    }
    // clang-format on
};

} // end namespace detail

// clang-format off
/// \verbatim embed:rst:leading-slashes
/// .. math::
///
///   \mathtt{make\_wrapper}: \mathtt{U} \to \mathtt{Wrapper}
///
/// Creates a :cpp:class:`detail::wrapper` of an lvalue reference when
/// ``U = T&`` for some type ``T`` or an rvalue reference when ``U ==
/// T&&``. If ``U = T&&``, ``T`` is required to be
/// `MoveConstructible <http://en.cppreference.com/w/cpp/concept/MoveConstructible>`_.
/// \endverbatim
// clang-format on
BOOST_STATIC_VIEWS_INLINE_VARIABLE(
    detail::make_wrapper_impl, make_wrapper)

BOOST_STATIC_VIEWS_END_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Expanded from 'view_base.hpp'
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Expanded from 'errors.hpp'
//////////////////////////////////////////////////////////////////////


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \brief Exception that is thrown when an "index out of bounds
/// error" is encountered.
class out_of_bound : std::exception {
    char const* _msg;

  public:
    out_of_bound() noexcept : _msg{"Index out of bounds."} {}
    explicit out_of_bound(char const* msg) noexcept : _msg{msg} {}

    auto what() const noexcept -> char const* override
    {
        return _msg;
    }
};

/*
class invalid_range : std::exception {
    char const* _msg;
public:
    invalid_range() noexcept : _msg{ "Invalid range bounds." } {}
    invalid_range(char const* msg) noexcept : _msg{ msg } {}

    auto what() const noexcept -> char const* override
    { return _msg; }
};
*/

/// \brief Exception that is thrown when an insert into a full bucket
/// is attempted.
class full_bucket : std::exception {
    char const* _msg;

  public:
    full_bucket() noexcept : _msg{"Bucket is full."} {}
    explicit full_bucket(char const* msg) noexcept : _msg{msg} {}

    auto what() const noexcept -> char const* override
    {
        return _msg;
    }
};

namespace detail {
BOOST_STATIC_VIEWS_NORETURN
auto make_out_of_bound(char const* msg) -> void
{
    throw out_of_bound{msg};
}

/*
BOOST_STATIC_VIEWS_NORETURN
auto make_invalid_range(char const* msg) -> void
{
    throw invalid_range{msg};
}
*/

BOOST_STATIC_VIEWS_NORETURN
auto make_full_bucket(char const* msg) -> void
{
    throw full_bucket{msg};
}
} // end namespace detail

// clang-format off
void (*make_out_of_bound_error)(char const*) = &detail::make_out_of_bound;
// void (*make_invalid_range_error)(char const*) = &detail::make_invalid_range;
void (*make_full_bucket_error)(char const*) = &detail::make_full_bucket;
// clang-format on

BOOST_STATIC_VIEWS_END_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Expanded from 'iterator.hpp'
//////////////////////////////////////////////////////////////////////


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class View>
struct view_iterator {

    using size_type         = std::size_t;
    using difference_type   = std::ptrdiff_t;
    using reference         = decltype(std::declval<
        std::decay_t<View>>()[std::declval<size_type>()]);
    using value_type        = std::remove_reference_t<reference>;
    using pointer           = std::add_pointer_t<reference>;
    using iterator_category = std::random_access_iterator_tag;

  private:
    View*          _view;
    std::ptrdiff_t _i;

    BOOST_STATIC_VIEWS_CONSTEXPR auto is_dereferenceable() const
        noexcept -> bool
    {
        return _view != nullptr && _i >= 0
               && static_cast<size_type>(_i) <= _view->size();
    }

    struct Impl {

      private:
        template <class T>
        using has_unsafe_at_t = decltype(
            std::declval<T>().unsafe_at(std::declval<std::size_t>()));

        BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_unsafe_at, T,
            (is_detected<has_unsafe_at_t, T>::value), "");

        BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_noexcept_unsafe_at, T,
            (noexcept(std::declval<T>().unsafe_at(
                std::declval<std::size_t>()))),
            "");

        // clang-format off
        template <class T,
            class = std::enable_if_t<concepts::and_<Has_unsafe_at,
                Is_noexcept_unsafe_at>::template test<T>()>,
            class = void>
        BOOST_STATIC_VIEWS_FORCEINLINE
        static BOOST_STATIC_VIEWS_CONSTEXPR
        BOOST_STATIC_VIEWS_DECLTYPE_AUTO unsafe_at_impl(T&& xs,
            std::size_t const i) noexcept
        {
            return std::forward<T>(xs).unsafe_at(i);
        }
        // clang-format on

        // clang-format off
        template <class T,
            class = std::enable_if_t<!concepts::and_<Has_unsafe_at,
                Is_noexcept_unsafe_at>::template test<T>()>>
        BOOST_STATIC_VIEWS_FORCEINLINE
        static BOOST_STATIC_VIEWS_CONSTEXPR
        auto unsafe_at_impl(T&& xs, std::size_t const i)
        BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
        (
            std::forward<T>(xs)[i]
        );
        // clang-format on

      public:
        // clang-format off
        template <class T>
        static BOOST_STATIC_VIEWS_CONSTEXPR
        auto unsafe_at(T&& xs, std::size_t const i)
        BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
        (
            unsafe_at_impl(std::forward<T>(xs), i)
        );
    };


  public:
    BOOST_STATIC_VIEWS_CONSTEXPR view_iterator(
        View* view = nullptr, size_type const i = 0)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
        : _view{view}, _i{static_cast<difference_type>(i)}
    {
        BOOST_STATIC_VIEWS_EXPECT(
            i < static_cast<size_type>(
                    std::numeric_limits<difference_type>::max()),
            "Overflow detected.");
        BOOST_STATIC_VIEWS_EXPECT(
            view == nullptr && i == 0
                || view != nullptr && i <= view->size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    view_iterator(view_iterator const&) noexcept = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    view_iterator(view_iterator&&) noexcept = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    view_iterator& operator=(view_iterator const&) noexcept = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    view_iterator& operator=(view_iterator&&) noexcept = default;

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto* operator-> () const BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
        noexcept(std::declval<View&>().unsafe_at(
            std::declval<size_type>())))
    {
        BOOST_STATIC_VIEWS_EXPECT(is_dereferenceable(),
            "Nah, this iterator is not dereferenceable.");
        return &Impl::unsafe_at(*_view, static_cast<size_type>(_i));
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO
    operator*() const BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(noexcept(
        std::declval<View&>().unsafe_at(std::declval<size_type>())))
    {
        BOOST_STATIC_VIEWS_EXPECT(is_dereferenceable(),
            "Nah, this iterator is not dereferenceable.");
        return Impl::unsafe_at(*_view, static_cast<size_type>(_i));
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO
    operator[](difference_type const n) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<view_iterator const&>().operator*()))
    {
        return *(*this + n);
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator++() noexcept -> view_iterator&
    {
        ++_i;
        return *this;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator--() noexcept -> view_iterator&
    {
        --_i;
        return *this;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator++(int) noexcept -> view_iterator
    {
        view_iterator temp{*this};
        ++(*this);
        return temp;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator--(int) noexcept -> view_iterator
    {
        view_iterator temp{*this};
        --(*this);
        return temp;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator+=(difference_type const n) noexcept
        -> view_iterator&
    {
        _i += n;
        return *this;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator-=(difference_type const n) noexcept
        -> view_iterator&
    {
        _i -= n;
        return *this;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator+(view_iterator const& x,
        difference_type const n) noexcept -> view_iterator
    {
        view_iterator temp{x};
        temp += n;
        return temp;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator-(view_iterator const& x,
        difference_type const n) noexcept -> view_iterator
    {
        view_iterator temp{x};
        temp -= n;
        return temp;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator+(difference_type const n,
        view_iterator const& x) noexcept -> view_iterator
    {
        return x + n;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator-(view_iterator const& x,
        view_iterator const& y) noexcept -> difference_type
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to subtract iterators to different "
            "objects.");
        return x._i - y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator==(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare iterators to different "
            "objects.");
        return x._i == y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator!=(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare iterators to different "
            "objects.");
        return x._i != y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator<(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare iterators to different "
            "objects.");
        return x._i < y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator>(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare iterators to different "
            "objects.");
        return x._i > y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator<=(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare iterators to different "
            "objects.");
        return x._i <= y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator>=(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare iterators to different "
            "objects.");
        return x._i >= y._i;
    }
};

} // namespace detail

BOOST_STATIC_VIEWS_END_NAMESPACE


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \brief Base class for all the views.

/// \verbatim embed:rst:leading-slashes
/// By deriving from :cpp:class:`view_base`, you tell StaticView that
/// the derived class models the :ref:`view <view-concept>` concept.
/// \endverbatim
struct view_base {
};

/// \brief Checks whether `V` models the View concept.

/// \verbatim embed:rst:leading-slashes
/// Metafunction that returns whether ``V`` models the :ref:`View
/// <view-concept>` concept:
///
/// .. code-block:: cpp
///
///   template <class View>
///   struct is_view : std::is_base_of<view_base, View>
///   {};
///
/// \endverbatim
// template <class V>
// struct is_view : std::is_base_of<view_base, V> {
// };

/// \brief Helper class one can befriend to give StaticViews access to
/// the private `map` function.

/// Defines a single private member function
/// \code
/// template <class View>
/// static constexpr auto map(View const& xs, std::size_t const i)
///     noexcept( whenever possible );
/// \endcode
/// that calls `xs.map(i)`. By befriending this class, you give
/// #boost::static_views::view_adaptor_base access to private `map`
/// function,
/// but nothing else. The very same technique is used in
/// [Boost.Iterator](http://www.boost.org/doc/libs/1_64_0/libs/iterator/doc/iterator_facade.html#iterator-core-access).
struct view_adaptor_core_access {
    template <class T, class V>
    friend struct view_adaptor_base;

    //  private: This is unfortunate and is to be fixed later!

    // clang-format off
    template <class View>
    static BOOST_STATIC_VIEWS_CONSTEXPR
    auto map(View const& xs, std::size_t const i)
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        xs.map(i)
    );
    // clang-format on
};

namespace concepts {
namespace view {

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Derives_from_view_base, T,
        (std::is_base_of<view_base, T>::value),
        "`T` doesn't derive from `boost::static_views::view_base`, "
        "which is, unfortunately, required by the View concept. "
        "Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_move_constructible, T,
        (std::is_move_constructible<T>::value),
        "`T` doesn't model the MoveConstructible concept, which "
        "it should, to have a chance at modeling the View concept. "
        "Hence, the error.");

    template <class T>
    using has_static_extent_t = decltype(T::extent());

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_static_extent, T,
        (is_detected<has_static_extent_t, T>::value),
        "`T` has no static member function `extent()`. "
        "Availability of it is, unfortunately, required by the "
        "View concept. Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_correct_return_type_extent, T,
        (std::is_convertible<detected_t<has_static_extent_t, T>,
            std::size_t>::value),
        "The View concept requires the return type of "
        "`T::extent()` to be convertible to `size_t`. This "
        "condition is, I'm afraid, not satisfied. Hence, the "
        "error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_noexcept_extent, T,
        (noexcept(T::extent())),
        "`T::extent()` is not noexcept while it should be "
        "to have a chance at modeling the View concept. Hence, "
        "the error.");

    template <class T>
    using has_member_size_t =
        decltype(std::declval<T const&>().size());

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_member_size, T,
        (is_detected<has_member_size_t, T>::value),
        "`T` has no member function `size()`. Availability of it "
        "is, unfortunately, required by the View concept. Hence, "
        "the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_correct_return_type_size, T,
        (std::is_convertible<detected_t<has_member_size_t, T>,
            std::size_t>::value),
        "The View concept requires the return type of the "
        "`size()` member function to be convertible to `size_t`. "
        "This condition is, I'm afraid, not satisfied. Hence, the "
        "error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_noexcept_size, T,
        (noexcept(std::declval<T const&>().size())),
        "`T`'s `size()` member function is not noexcept while it "
        "should be for `T` to have a chance at modeling the View "
        "concept. Hence, the error.");

    template <class T>
    using has_operator_access_t =
        decltype(std::declval<T>()[std::declval<std::size_t>()]);

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_operator_access, T,
        (is_detected<has_operator_access_t, T>::value),
        "`T` has no `operator[](size_t)`. Availability of it "
        "is, unfortunately, required by the View concept. Hence, "
        "the error.");

    template <class T>
    using has_member_map_t = decltype(view_adaptor_core_access::map(
        std::declval<T>(), std::declval<std::size_t>()));

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_member_map, T,
        (is_detected<has_member_map_t, T>::value),
        "`T` is required to have a member function `map(size_t)` "
        "accessible through "
        "`boost::static_views::view_adaptor_core_access`. Either "
        "define a it publicly or befriend the "
        "`view_adaptor_core_access` struct.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_correct_return_type_map, T,
        (std::is_convertible<detected_t<has_member_map_t, T>,
            std::size_t>::value),
        "`T`'s `map(size_t)` should have return type convertible to "
        "size_t.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_noexcept_map, T,
        (noexcept(view_adaptor_core_access::map(
            std::declval<T>(), std::declval<std::size_t>()))),
        "`T`'s `map(size_t)` member function is not noexcept while "
        "it "
        "should be. Hence, the error.");

    template <class T>
    using has_unsafe_at_t = decltype(
        std::declval<T>().unsafe_at(std::declval<std::size_t>()));

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_unsafe_at, T,
        (is_detected<has_unsafe_at_t, T>::value),
        "`T` is required to have a member function "
        "`unsafe_at(size_t)`.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_noexcept_unsafe_at, T,
        (noexcept(std::declval<T>().unsafe_at(
            std::declval<std::size_t>()))),
        "`T`'s `unsafe_at(size_t)` member function is not noexcept "
        "while it should be. Hence, the error.");

    // clang-format off
    using View_impl =
        all_<
            Derives_from_view_base,
            Is_move_constructible,
            and_<
                Has_static_extent,
                Has_correct_return_type_extent,
                Is_noexcept_extent
            >,
            and_<
                Has_member_size,
                Has_correct_return_type_size,
                Is_noexcept_size
            >,
            Has_operator_access
        >;
    // clang-format on

    struct View : View_impl {

        using View_impl::test;

        template <class T>
        static constexpr auto check() noexcept -> bool
        {
            constexpr bool x = test<T>();
            static_assert(
                x, "I'm sorry, but `T` must model the View concept!");
            return View_impl::check<T>();
        }

      private:
        // clang-format off
        template <class T,
            class = std::enable_if_t<and_<Has_unsafe_at,
                Is_noexcept_unsafe_at>::test<T>()>,
            class = void>
        BOOST_STATIC_VIEWS_FORCEINLINE
        static BOOST_STATIC_VIEWS_CONSTEXPR
        BOOST_STATIC_VIEWS_DECLTYPE_AUTO unsafe_at_impl(T&& xs,
            std::size_t const i) noexcept
        {
            return std::forward<T>(xs).unsafe_at(i);
        }
        // clang-format on

        // clang-format off
        template <class T,
            class = std::enable_if_t<!and_<Has_unsafe_at,
                Is_noexcept_unsafe_at>::test<T>()>>
        BOOST_STATIC_VIEWS_FORCEINLINE
        static BOOST_STATIC_VIEWS_CONSTEXPR
        auto unsafe_at_impl(T&& xs, std::size_t const i)
        BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
        (
            std::forward<T>(xs)[i]
        );
        // clang-format on

      public:
        // clang-format off
        template <class T,
            class = std::enable_if_t<
                test<std::remove_cv_t<std::remove_reference_t<T>>>()>>
        static BOOST_STATIC_VIEWS_CONSTEXPR
        auto unsafe_at(T&& xs, std::size_t const i)
        BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
        (
            unsafe_at_impl(std::forward<T>(xs), i)
        );
        // clang-format on
    };
    // clang-format on

} // namespace view

/// \verbatim embed:rst:leading-slashes
/// Defines the View concept. Here's a synopsis:
///
/// .. code-block:: cpp
///
///   struct View {
///       template <class T>
///       static constexpr auto test() noexcept -> bool;
///
///       template <class T>
///       static constexpr auto check() noexcept -> bool;
///   };
///
/// ``test()`` returns whether the requirements are satisfied, while
/// ``check()`` *checks* that they are satisfied and triggers a
/// ``static_assert`` failure if they aren't.
/// \endverbatim
#if defined(DOXYGEN_IN_HOUSE)
// let Doxygen think it's just a struct.
struct View {
};
#else
using view::View;
#endif

} // namespace concepts

// clang-format off
template <class View,
    class = std::enable_if_t<concepts::View::template test<View>()>>
BOOST_STATIC_VIEWS_CONSTEXPR
auto begin(View const& xs)
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    detail::view_iterator<View const>{&xs, 0}
);

template <class View,
    class = std::enable_if_t<concepts::View::template test<View>()>>
BOOST_STATIC_VIEWS_CONSTEXPR
auto end(View const& xs)
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    detail::view_iterator<View const>{&xs, xs.size()}
);
// clang-format on

/// \brief
/// \verbatim embed:rst:leading-slashes
/// Base class to that helps with modeling the :ref:`View
/// <view-concept>` concept. \endverbatim
template <class Derived, class Wrapper>
struct view_adaptor_base : view_base {

  private:
    using derived_type = Derived;
    using wrapper_type = Wrapper;
    using view_type    = typename Wrapper::type;

    wrapper_type _xs;

    template <class Dummy>
    struct traits {
        template <class T>
        static constexpr auto has_map() noexcept -> bool
        {
            using namespace concepts;
            return and_<view::Has_member_map,
                view::Has_correct_return_type_map,
                view::Is_noexcept_map>::test<T>();
        }
    };

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto derived() const noexcept
        -> derived_type const&
    {
        return *static_cast<derived_type const*>(this);
    }

  protected:
    using view_adaptor_base_type =
        view_adaptor_base<Derived, Wrapper>;

  public:
    /// \name Constructors
    ///
    /// \brief Constructs an adaptor of \p view.
    /// \details Here, \p view is a wrapper around the actual view
    /// created with #make_wrapper(T&&) function.
    explicit BOOST_STATIC_VIEWS_CONSTEXPR view_adaptor_base(
        wrapper_type&& view)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<wrapper_type>::value)
        : _xs{std::move(view)}
    {
    }

    /// \name Copy and move constructors/assignments
    ///
    /// Defines default copy and move constructors and assignments,
    /// i.e. is copy/move-constructible/assignable if \p View is.
    /// \{
    BOOST_STATIC_VIEWS_CONSTEXPR
    view_adaptor_base(view_adaptor_base const&) = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    view_adaptor_base(view_adaptor_base&&) = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    view_adaptor_base& operator=(view_adaptor_base const&) = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    view_adaptor_base& operator=(view_adaptor_base&&) = default;
    /// \}

    /// \brief Returns the underlying view.
    /// \{
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO parent() const & noexcept
    {
        static_assert(noexcept(_xs.get()),
            "[INTERNAL] view_adaptor_base<> assumes that "
            "`wrapper::get() const&` is noexcept.");
        return _xs.get();
    }

    // clang-format off
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO parent() &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<wrapper_type&&>().get()))
    {
        return std::move(_xs).get();
    }
    // clang-format on
    /// \}

    /// \brief
    /// \verbatim embed:rst:leading-slashes
    /// Default implementation of the ``capacity`` function required
    /// by the :ref:`view concept <view-concept>`. \endverbatim

    /// Just calls `capacity()` on the underlying view.
    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return view_type::extent();
    }

    /// \brief
    /// \verbatim embed:rst:leading-slashes
    /// Default implementation of the ``size`` function required by
    /// the :ref:`view concept <view-concept>`. \endverbatim

    /// Just calls `size()` on the underlying view.
    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const noexcept
    {
        static_assert(noexcept(parent()),
            "[INTERNAL] view_adaptor_base<> assumes that its "
            "own `parent() const&` member function does not throw.");
        return parent().size();
    }

    /// \name Element access
    /// \{

    // clang-format off
    template <class Dummy = void,
        class = std::enable_if_t<traits<Dummy>::template
            has_map<derived_type>()>>
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO unsafe_at(std::size_t const i) const&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            concepts::View::unsafe_at(
                std::declval<view_type const&>(),
                std::declval<std::size_t>())))
    {
        static_assert(noexcept(parent()),
            "[INTERNAL] view_adaptor_base<> assumes that its "
            "own `parent() const&` member function does not throw.");
        BOOST_STATIC_VIEWS_EXPECT(i < derived().size(),
            "You're trying to cause undefined behavior, aren't you?");
        auto const x = view_adaptor_core_access::map(derived(), i);
        return concepts::View::unsafe_at(parent(), x);
    }
    // clang-format on

    // clang-format off
    template <class Dummy = void,
        class = std::enable_if_t<traits<Dummy>::template
            has_map<derived_type>()>>
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO unsafe_at(std::size_t const i) &
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            concepts::View::unsafe_at(
                std::declval<view_type &>(),
                std::declval<std::size_t>())))
    {
        static_assert(noexcept(parent()),
            "[INTERNAL] view_adaptor_base<> assumes that its "
            "own `parent() const&` member function does not throw.");
        BOOST_STATIC_VIEWS_EXPECT(i < derived().size(),
            "You're trying to cause undefined behavior, aren't you?");
        auto const x = view_adaptor_core_access::map(derived(), i);
        return concepts::View::unsafe_at(parent(), x);
    }
    // clang-format on

    // clang-format off
    template <class Dummy = void,
        class = std::enable_if_t<traits<Dummy>::template
            has_map<derived_type>()>>
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO unsafe_at(std::size_t const i) &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            concepts::View::unsafe_at(
                std::declval<view_type &&>(),
                std::declval<std::size_t>())))
    {
        static_assert(noexcept(parent()),
            "[INTERNAL] view_adaptor_base<> assumes that its "
            "own `parent() const&` member function does not throw.");
        BOOST_STATIC_VIEWS_EXPECT(i < derived().size(),
            "You're trying to cause undefined behavior, aren't you?");
        auto const x = view_adaptor_core_access::map(derived(), i);
        return concepts::View::unsafe_at(
            std::forward<view_adaptor_base>(*this).parent(), x);
    }
    // clang-format on

    // clang-format off
    template <class Dummy = void,
        class = std::enable_if_t<traits<Dummy>::template
            has_map<derived_type>()>>
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](std::size_t const i) const&
    {
        if (BOOST_STATIC_VIEWS_UNLIKELY(i >= derived().size())) {
            make_out_of_bound_error("`i < size()` not satisfied.");
            BOOST_STATIC_VIEWS_UNREACHABLE;
        }
        auto const x = view_adaptor_core_access::map(derived(), i);
        return parent()[x];
    }
    // clang-format on

    // clang-format off
    template <class Dummy = void,
        class = std::enable_if_t<traits<Dummy>::template
            has_map<derived_type>()>>
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](std::size_t const i) &
    {
        if (BOOST_STATIC_VIEWS_UNLIKELY(i >= derived().size())) {
            make_out_of_bound_error("`i < size()` not satisfied.");
            BOOST_STATIC_VIEWS_UNREACHABLE;
        }
        auto const x = view_adaptor_core_access::map(derived(), i);
        return parent()[x];
    }
    // clang-format on

    // clang-format off
    template <class Dummy = void,
        class = std::enable_if_t<traits<Dummy>::template
            has_map<derived_type>()>>
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](std::size_t const i) &&
    {
        if (BOOST_STATIC_VIEWS_UNLIKELY(i >= derived().size())) {
            make_out_of_bound_error("`i < size()` not satisfied.");
            BOOST_STATIC_VIEWS_UNREACHABLE;
        }
        auto const x = view_adaptor_core_access::map(derived(), i);
        return std::forward<view_adaptor_base>(*this).parent()[x];
    }
    // clang-format on
    /// \}

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto begin() const BOOST_STATIC_VIEWS_NOEXCEPT_IF(
        noexcept(::BOOST_STATIC_VIEWS_NAMESPACE::begin(
            std::declval<derived_type const&>())))
    {
        return ::BOOST_STATIC_VIEWS_NAMESPACE::begin(derived());
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto end() const BOOST_STATIC_VIEWS_NOEXCEPT_IF(
        noexcept(::BOOST_STATIC_VIEWS_NAMESPACE::end(
            std::declval<derived_type const&>())))
    {
        return ::BOOST_STATIC_VIEWS_NAMESPACE::end(derived());
    }
};

BOOST_STATIC_VIEWS_END_NAMESPACE


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \brief Base class for all the algorithms.

/// \verbatim embed:rst:leading-slashes
/// By deriving from :cpp:class:`algorithm_base`, you tell StaticView
/// that the derived class models the :ref:`view <algorithm-concept>`
/// concept. \endverbatim
struct algorithm_base {
};

/// \brief Checks whether `T` models the Algorithm concept.

/// \verbatim embed:rst:leading-slashes
/// Metafunction that returns whether ``T`` models the :ref:`algorithm
/// <algorithm-concept>` concept:
///
/// .. code-block:: cpp
///
///   template <class T>
///   struct is_algorithm : std::is_base_of<algorithm_base, T>
///   {};
///
/// \endverbatim
template <class V>
struct is_algorithm : std::is_base_of<algorithm_base, V> {
};

namespace detail {
template <class Function, class... Args>
struct algorithm_impl : algorithm_base {

  private:
    std::tuple<Function, Args...> _storage;

    template <class T>
    using compile_call_cref_t =
        decltype(invoke(std::declval<Function const&>(),
            make_wrapper(std::declval<T>()),
            std::declval<Args const&>()...));

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_cref_callable, T,
        (is_detected<compile_call_cref_t, T>::value), "");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_noexcept_cref_call, T,
        (noexcept(invoke(std::declval<Function const&>(),
            make_wrapper(std::declval<T>()),
            std::declval<Args const&>()...))),
        "");

    template <class T>
    using compile_call_move_t = decltype(invoke(
        std::declval<Function&&>(), make_wrapper(std::declval<T>()),
        std::declval<Args&&>()...));

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_move_callable, T,
        (is_detected<compile_call_move_t, T>::value), "");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_noexcept_move_call, T,
        (noexcept(invoke(std::declval<Function&&>(),
            make_wrapper(std::declval<T>()),
            std::declval<Args&&>()...))),
        "");

    // clang-format off
    template <class View, std::size_t... Is>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO call_impl(View&& xs,
        std::index_sequence<Is...> /*unused*/) const&
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            concepts::and_<Is_cref_callable,
                Is_noexcept_cref_call>::template test<View&&>())
    {
        return invoke(std::get<0>(_storage),
            make_wrapper(std::forward<View>(xs)),
            std::get<Is + 1>(_storage)...);
    }

    // clang-format off
    template <class View, std::size_t... Is>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO call_impl(View&& xs,
        std::index_sequence<Is...> /*unused*/) &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            concepts::and_<Is_move_callable, Is_noexcept_move_call>
                ::template test<View&&>())
    // clang-format on
    {
        return invoke(std::get<0>(std::move(_storage)),
            make_wrapper(std::forward<View>(xs)),
            std::get<Is + 1>(std::move(_storage))...);
    }

  public:
    explicit BOOST_STATIC_VIEWS_CONSTEXPR algorithm_impl(
        Function&& f, Args... args)
#if defined(BOOST_STATIC_VIEWS_NEGLECT_STD_TUPLE)
        // std::tuple's constructor is not marked noexcept in most of
        // the implementations. We "know", however, that the only
        // thing std::tuple's move constructor needs is for each Args
        // to be nothrow move constructible. Hence, we can "neglect"
        // the fact that std::tuple's move constructor is not
        // noexcept.
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            utils::all(std::is_nothrow_move_constructible<
                           std::decay_t<Args>>::value...,
                std::is_nothrow_move_constructible<Function>::value))
#else
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<
                std::tuple<Function, Args...>>::value)
#endif
        : _storage{std::move(f), std::forward<Args>(args)...}
    {
    }

    // clang-format off
    template <class View,
        class = std::enable_if_t<concepts::View::template test<
            std::remove_cv_t<std::remove_reference_t<View>>>()>>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs) const&
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        call_impl(std::forward<View>(xs),
            std::make_index_sequence<sizeof...(Args)>{})
    );
    // clang-format on

    // clang-format off
    template <class View,
        class = std::enable_if_t<concepts::View::template test<
            std::remove_cv_t<std::remove_reference_t<View>>>()>>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator()(View&& xs) &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            concepts::and_<Is_move_callable, Is_noexcept_move_call>
                ::template test<View&&>())
    {
        return std::forward<algorithm_impl>(*this).call_impl(
            std::forward<View>(xs),
            std::make_index_sequence<sizeof...(Args)>{});
    }
    // clang-format on

    template <class View,
        class = std::enable_if_t<!concepts::View::template test<
            std::remove_cv_t<std::remove_reference_t<View>>>()>,
        class = void>
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(
        View&& /*unused*/) const noexcept
    {
        static_assert(
            concepts::View::template test<
                std::remove_cv_t<std::remove_reference_t<View>>>(),
            "I'm sorry, but `View` must model the View concept!");
        concepts::View::template check<
            std::remove_cv_t<std::remove_reference_t<View>>>();
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    algorithm_impl(algorithm_impl const& other)
#if defined(BOOST_STATIC_VIEWS_NEGLECT_STD_TUPLE)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_copy_constructible<Function>::value,
            std::is_nothrow_copy_constructible<
                std::decay_t<Args>>::value...))
#else
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_copy_constructible<
                std::tuple<Function, Args...>>::value)
#endif
        : _storage{other._storage}
    {
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    algorithm_impl(algorithm_impl&& other)
#if defined(BOOST_STATIC_VIEWS_NEGLECT_STD_TUPLE)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_move_constructible<Function>::value,
            std::is_nothrow_move_constructible<
                std::decay_t<Args>>::value...))
#else
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<
                std::tuple<Function, Args...>>::value)
#endif
        : _storage{std::move(other._storage)}
    {
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    algorithm_impl& operator=(algorithm_impl const& other)
#if defined(BOOST_STATIC_VIEWS_NEGLECT_STD_TUPLE)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_copy_assignable<Function>::value,
            std::is_nothrow_copy_assignable<
                std::decay_t<Args>>::value...))
#else
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_copy_assignable<
                std::tuple<Function, Args...>>::value)
#endif
    {
        _storage = other._storage;
        return *this;
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    algorithm_impl& operator=(algorithm_impl&& other)
#if defined(BOOST_STATIC_VIEWS_NEGLECT_STD_TUPLE)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_move_assignable<Function>::value,
            std::is_nothrow_move_assignable<
                std::decay_t<Args>>::value...))
#else
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_assignable<
                std::tuple<Function, Args...>>::value)
#endif
    {
        _storage = std::move(other._storage);
        return *this;
    }
};

struct make_algorithm_impl {
    // clang-format off
    template <class Function, class... Args>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Function&& fn, Args&&... args) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm_impl<std::decay_t<Function>,
            std::conditional_t<std::is_lvalue_reference<Args>::value,
                Args,
                std::remove_reference_t<Args>
            >...>(
            std::forward<Function>(fn), std::forward<Args>(args)...)
    );
    // clang-format on
};

} // namespace detail

/*
#define BOOST_STATIC_VIEWS_INLINE_ALGO_VARIABLE(type, name)          \
    inline namespace {                                               \
        BOOST_STATIC_VIEWS_CONSTEXPR auto const& name =              \
            ::BOOST_STATIC_VIEWS_NAMESPACE::_static_const<           \
                ::BOOST_STATIC_VIEWS_NAMESPACE::detail::             \
                    make_algorithm_impl<type>>;                      \
    }
*/

BOOST_STATIC_VIEWS_INLINE_VARIABLE(
    detail::make_algorithm_impl, algorithm)

BOOST_STATIC_VIEWS_END_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Expanded from 'drop.hpp'
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Expanded from 'sequence_traits.hpp'
//////////////////////////////////////////////////////////////////////


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

/// \brief Special value of that indicates that the size of a sequence
/// is unknown at compile-time.
constexpr std::ptrdiff_t dynamic_extent = -1;

namespace concepts {
namespace sequence {

    /// \brief Helper trait to determine whether a specialisation
    /// of #sequence_traits for `T` exists.
    template <class T>
    using has_sequence_traits_spec_t = decltype(sequence_traits<T>{});

    BOOST_STATIC_VIEWS_DEFINE_CHECK(HasSequenceTraitsSpecialisation,
        T, (is_detected<has_sequence_traits_spec_t, T>::value),
        "There exists no specialisation of "
        "`boost::static_views::sequence_traits` for type `T`. "
        "Availability of it is, unfortunately, required "
        "by the Sequence concept. Hence, the error.");

    /*
    /// \brief Helper trait to determine whether a specialisation
    /// of #sequence_traits for `T` has a `type` typedef.
    template <class T>
    using has_type_typedef_t = typename sequence_traits<T>::type;

    BOOST_STATIC_VIEWS_DEFINE_CHECK(
        Has_type_typedef, T,
        (is_detected<has_type_typedef_t, T>::value),
        "`boost::static_views::sequence_traits<T>` has no "
        "`type` typedef. Availability of it is, unfortunately, "
        "required by the Sequence concept. Hence, the error.");
    */

    template <class T>
    using has_extent_t = decltype(sequence_traits<T>::extent());

    BOOST_STATIC_VIEWS_DEFINE_CHECK(HasExtent, T,
        (is_detected<has_extent_t, T>::value),
        "`boost::static_views::sequence_traits<T>` has no "
        "static member function `extent()`. Availability of "
        "it is, unfortunately, required by the Sequence concept. "
        "Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(ExtentHasCorrectReturnType, T,
        (std::is_convertible<detected_t<has_extent_t, T>,
            std::ptrdiff_t>::value),
        "Return type of "
        "`boost::static_views::sequence_traits<T>::extent()` is "
        "not convertible to ptrdiff_t. According to the "
        "Sequence concept, it should be. Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(ExtentIsNoexcept, T,
        (noexcept(sequence_traits<T>::extent())),
        "`boost::static_views::sequence_traits<T>::extent()` is "
        "not noexcept. It should be though, according to the "
        "Sequence concept. Hence, the error.");

    struct ExtentIsConstexpr {
      private:
        template <class T, int = (T::extent(), 0)>
        static constexpr auto test_impl(int) -> std::true_type
        {
            return {};
        }

        template <class T>
        static constexpr auto test_impl(...) -> std::false_type
        {
            return {};
        }

      public:
        template <class T>
        static constexpr auto test() noexcept -> bool
        {
            return test_impl<T>(int{});
        }

        template <class T>
        static constexpr auto check() noexcept -> bool
        {
            constexpr auto x = test<T>();
            static_assert(x,
                "`boost::static_views::sequence_traits<T>::extent()` "
                "is not constexpr. It should be though for `T` to "
                "satisfy the Sequence concept. Hence, the error.");
            return x;
        }
    };

    BOOST_STATIC_VIEWS_DEFINE_CHECK(ExtentHasCorrectReturnValue, T,
        (sequence_traits<T>::extent() == dynamic_extent
            || sequence_traits<T>::extent() >= 0),
        "`boost::static_views::sequence_traits<T>::extent()` returns "
        "an invald extent. Either a non-negative number or "
        "`boost::static_views::dynamic_extent` should be returned. "
        "Hence, the error.");

    template <class T>
    using has_size_t =
        decltype(sequence_traits<T>::size(std::declval<T const&>()));

    BOOST_STATIC_VIEWS_DEFINE_CHECK(HasSize, T,
        (is_detected<has_size_t, T>::value),
        "`boost::static_views::sequence_traits<T>` has no "
        "static member function `size(T const&)`. Availability of "
        "it is, unfortunately, required by the Sequence concept. "
        "Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(SizeHasCorrectReturnType, T,
        (std::is_convertible<detected_t<has_size_t, T>,
            std::size_t>::value),
        "Return type of "
        "`boost::static_views::sequence_traits<T>::size(T const&)` "
        "is not convertible to size_t. According to the "
        "Sequence concept, it should be. Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(SizeIsNoexcept, T,
        (noexcept(
            sequence_traits<T>::size(std::declval<T const&>()))),
        "`boost::static_views::sequence_traits<T>::size(T const&)` "
        "is not noexcept. It should be though, according to the "
        "Sequence concept. Hence, the error.");

    template <class T>
    using has_at_t = decltype(sequence_traits<T>::at(
        std::declval<T>(), std::declval<std::size_t>()));

    BOOST_STATIC_VIEWS_DEFINE_CHECK(HasAt, T,
        (is_detected<has_at_t, T>::value),
        "`boost::static_views::sequence_traits<T>` has no "
        "static member function `at(T, size_t)`. "
        "Availability of it is, unfortunately, required by "
        "the Sequence concept. Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(AtIsNoexcept, T,
        (noexcept(sequence_traits<T>::at(
            std::declval<T>(), std::declval<std::size_t>()))),
        "`boost::static_views::sequence_traits<T>::at(T, size_t)` "
        "is not noexcept. It should be though, according to the "
        "Sequence concept. Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(HasFixedSize, T,
        (sequence_traits<T>::extent() >= 0),
        "`boost::static_views::sequence_traits<T>::extent() should "
        "return a non-negative number. It does not. Hence, the "
        "error.");

    // clang-format off
    using Sequence =
        and_<
            HasSequenceTraitsSpecialisation,
            all_<
                and_<
                    HasExtent,
                    ExtentHasCorrectReturnType,
                    ExtentIsNoexcept,
                    ExtentHasCorrectReturnValue
                >,
                and_<
                    HasAt,
                    AtIsNoexcept
                >,
                and_<
                    HasSize,
                    SizeHasCorrectReturnType,
                    SizeIsNoexcept
                >
            >
        >;

    using StaticSequence =
        and_<
            Sequence,
            HasFixedSize
        >;
    // clang-format on

} // namespace sequence

using sequence::Sequence;
using sequence::StaticSequence;

} // namespace concepts

template <class Derived, class T>
struct sequence_traits_default {

  private:
    template <class Dummy>
    struct Impl {
      private:
        template <class U>
        using call_size_t = decltype(std::declval<U const&>().size());

        template <class U>
        using call_size_impl_t =
            decltype(U::size_impl(std::declval<T const&>()));

      public:
        struct HasExtent {
          private:
            template <class U, int = (U::extent(), 0)>
            static constexpr auto has_extent(int) noexcept
                -> std::true_type
            {
                return {};
            }

            template <class U>
            static constexpr auto has_extent(...) noexcept
                -> std::false_type
            {
                return {};
            }

          public:
            template <class U>
            static constexpr auto test() noexcept -> bool
            {
                return has_extent<U>(int{});
            }
        };

        BOOST_STATIC_VIEWS_DEFINE_CHECK(
            ExtentIsNonNegative, U, (U::extent() >= 0), "");

        BOOST_STATIC_VIEWS_DEFINE_CHECK(HasSize, U,
            (detail::is_detected<call_size_t, U>::value), "");

        BOOST_STATIC_VIEWS_DEFINE_CHECK(HasSizeImpl, U,
            (detail::is_detected<call_size_impl_t, U>::value), "");
    };

  public: // This is really unfortunate
    // Derived::extent() exists and is non-negative
    template <class Dummy = void,
        class             = std::enable_if_t<
            concepts::and_<typename Impl<Dummy>::HasExtent,
                typename Impl<Dummy>::ExtentIsNonNegative>::
                template test<Derived>()>,
        class = void>
    static constexpr auto size_impl(T const& /*unused*/) noexcept
        -> std::size_t
    {
        static_assert(std::is_convertible<decltype(Derived::extent()),
                          std::ptrdiff_t>::value,
            "If you provide your own `extent()` function, do you "
            "mind making it return something convertible to "
            "`std::ptrdiff_t`?");
        return static_cast<std::size_t>(Derived::extent());
    }

    // Derived::extent() either doesn't exist or returns a negative
    // number, but T::size() does exist.
    template <class Dummy = void,
        class             = std::enable_if_t<
            !concepts::and_<typename Impl<Dummy>::HasExtent,
                typename Impl<Dummy>::ExtentIsNonNegative>::
                template test<Derived>()
            && Impl<Dummy>::HasSize::template test<T>()>>
    static constexpr auto size_impl(T const& xs) noexcept
        -> std::size_t
    {
        static_assert(
            std::is_convertible<decltype(
                                    std::declval<T const&>().size()),
                std::size_t>::value,
            "If you provide your own `size()` function, do you "
            "mind making it return something convertible to "
            "`std::size_t`?");
        return xs.size();
    }

  public:
    template <class S,
        class = std::enable_if_t<std::is_same<T,
            std::remove_cv_t<std::remove_reference_t<S>>>::value>>
    static constexpr auto at(S&& xs, std::size_t i) noexcept
        -> decltype(std::forward<S>(xs)[i])
    {
        static_assert(
            noexcept(std::forward<S>(xs)[i]), "Why not noexcept?");
        return std::forward<S>(xs)[i];
    }

    // Only enable if size_impl is available
    template <class Dummy = void,
        class = std::enable_if_t<Impl<Dummy>::HasSizeImpl::
                template test<sequence_traits_default>()>>
    static constexpr auto size(T const& xs) noexcept
    {
        return sequence_traits_default::size_impl(xs);
    }

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return dynamic_extent;
    }
};

/// \brief Specialisation of #sequence_traits for C-arrays.
template <class T, std::size_t N>
struct sequence_traits<T[N]>
    : sequence_traits_default<sequence_traits<T[N]>, T[N]> {

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        constexpr std::size_t max_size = static_cast<std::size_t>(
            std::numeric_limits<std::ptrdiff_t>::max());
        static_assert(N <= max_size, "Overflow detected.");
        return static_cast<std::ptrdiff_t>(N);
    }
};

/// \brief Specialisation of #sequence_traits for std::array.
template <class T, std::size_t N>
struct sequence_traits<std::array<T, N>>
    : sequence_traits_default<sequence_traits<std::array<T, N>>,
          std::array<T, N>> {

    template <class S,
        class = std::enable_if_t<std::is_same<std::array<T, N>,
            std::remove_cv_t<std::remove_reference_t<S>>>::value>>
    static constexpr auto at(S&& xs, std::size_t i) noexcept
        -> decltype(std::forward<S>(xs)[i])
    {
        return std::forward<S>(xs).data()[i];
    }

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        constexpr std::size_t max_size = static_cast<std::size_t>(
            std::numeric_limits<std::ptrdiff_t>::max());
        static_assert(N <= max_size, "Overflow detected.");
        return static_cast<std::ptrdiff_t>(N);
    }
};

namespace detail {

template <class Tuple, class = void>
struct sequence_traits_tuple;

template <>
struct sequence_traits_tuple<std::tuple<>> {
    static constexpr auto at(
        std::tuple<> xs, std::size_t const i) noexcept -> nonesuch;
};

template <class T, class... Ts>
struct sequence_traits_tuple<std::tuple<T, Ts...>,
    std::enable_if_t<utils::all(std::is_same<T, Ts>::value...)>> {

  private:
    using tuple_type = std::tuple<T, Ts...>;
    using value_type = T;

    template <class U>
    static constexpr decltype(auto) dummy_get(U&& tpl) noexcept
    {
        return std::get<0>(std::forward<U>(tpl));
    }

    // This is just wonderful!
    template <class U>
    using get_type =
        std::decay_t<auto(U)->decltype(dummy_get(std::declval<U>()))>;

    template <class Tuple, std::size_t... Is>
    static constexpr decltype(auto) at_impl(Tuple&& xs,
        std::size_t const                           i,
        std::index_sequence<Is...> /*unused*/) noexcept
    {
        constexpr get_type<Tuple&&> getters[] = {&std::get<Is>...};
        return (*getters[i])(std::forward<Tuple>(xs));
    }

  public:
    template <class Tuple,
        class = std::enable_if_t<std::is_same<tuple_type,
            std::remove_cv_t<std::remove_reference_t<Tuple>>>::value>>
    static constexpr decltype(auto) at(
        Tuple&& xs, std::size_t const i) noexcept
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

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        constexpr std::size_t max_size = static_cast<std::size_t>(
            std::numeric_limits<std::ptrdiff_t>::max());
        static_assert(
            sizeof...(Ts) <= max_size, "Overflow detected.");

        return static_cast<std::ptrdiff_t>(sizeof...(Ts));
    }
};

BOOST_STATIC_VIEWS_END_NAMESPACE


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class Wrapper, std::ptrdiff_t Extent>
struct drop_impl
    : view_adaptor_base<drop_impl<Wrapper, Extent>, Wrapper> {

    /// \brief Constructs a view of \p xs with the first \p b elements
    /// dropped.

    /// \param xs **Rvalue** reference to a wrapper around a view.
    /// \param b  Number of elements to drop. The resulting view will
    ///           contain `max(xs.get().size() - b, 0)` elements.
    ///
    /// \exception Never throws, unless the initialisation of
    /// #view_adaptor_base with `Wrapper&&` throws.
    explicit BOOST_STATIC_VIEWS_CONSTEXPR drop_impl(
        Wrapper&& xs, std::size_t const b)
#if !defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)
        // This is formally wrong, but come on,
        // std::min(size_t, size_t) _should_ be noexcept.
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(std::is_nothrow_constructible<
            typename drop_impl::view_adaptor_base_type,
            Wrapper&&>::value)
#endif
        : drop_impl::view_adaptor_base_type{std::move(xs)}, _b{b}
    {
        BOOST_STATIC_VIEWS_EXPECT(b <= this->parent().size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        BOOST_STATIC_VIEWS_EXPECT(
            extent() == dynamic_extent || extent() >= size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
    }

    /// \brief Returns the number of elements viewed.
    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const noexcept
        -> std::size_t
    {
        static_assert(noexcept(this->parent().size()),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        // Constructor makes sure this never causes underflow
        BOOST_STATIC_VIEWS_EXPECT(_b <= this->parent().size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return this->parent().size() - _b;
    }

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return Extent;
    }

    /// \brief "Maps" index \p i to the corresponding index in the
    /// parent view.

    /// For \f$ i \geq \text{xs.size}()\f$ behavior of this function
    /// is undefined
    BOOST_STATIC_VIEWS_CONSTEXPR auto map(std::size_t const i) const
        noexcept
    {
        BOOST_STATIC_VIEWS_EXPECT(i < size(),
            "Which part of `map: [0, size()) --> N` you didn't "
            "understand?");
        return _b + i;
    }

  private:
    std::size_t _b;
};

struct make_drop_exactly_impl {
    // clang-format off
    template <class Wrapper>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Wrapper&& xs, std::size_t const b) const
    // clang-format on
#if !defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            drop_impl<std::decay_t<Wrapper>, Wrapper::type::extent()>{
                std::forward<Wrapper>(xs), b}))
#endif
    {
        BOOST_STATIC_VIEWS_EXPECT(b <= xs.get().size(),
            "drop_exactly(n)(xs) expects `n <= xs.size()`.");
        BOOST_STATIC_VIEWS_EXPECT(
            Wrapper::type::extent() == dynamic_extent
                || Wrapper::type::extent() >= xs.get().size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return drop_impl<std::decay_t<Wrapper>,
            Wrapper::type::extent()>{std::forward<Wrapper>(xs), b};
    }

    // clang-format off
    template <class Wrapper, std::size_t N>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Wrapper&& xs,
        std::integral_constant<std::size_t, N> /*unused*/) const
    // clang-format on
#if !defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            drop_impl<std::decay_t<Wrapper>, Wrapper::type::extent()>{
                std::forward<Wrapper>(xs), N}))
#endif
    {
        BOOST_STATIC_VIEWS_EXPECT(N <= xs.get().size(),
            "drop_exactly(n)(xs) expects `n <= xs.size()`.");
        BOOST_STATIC_VIEWS_EXPECT(
            Wrapper::type::extent() == dynamic_extent
                || Wrapper::type::extent() >= xs.get().size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return drop_impl<std::decay_t<Wrapper>,
            ((Wrapper::type::extent() > N)
                    ? (Wrapper::type::extent() - N)
                    : 0)>{std::forward<Wrapper>(xs), N};
    }
};

struct make_drop_exactly_algo_impl {
    // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::size_t const n) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_drop_exactly_impl{}, std::size_t{n})
    );
    // clang-format on

    // clang-format off
    template <std::size_t N>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::integral_constant<std::size_t, N> /*unused*/) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_drop_exactly_impl{},
            std::integral_constant<std::size_t, N>{})
    );
    // clang-format on
};

struct make_drop_impl {
    // clang-format off
    template <class Wrapper>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Wrapper&& xs, std::size_t const b) const
    BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN
    (
        drop_impl<std::decay_t<Wrapper>,
            Wrapper::type::extent()>{std::forward<Wrapper>(xs), 
            std::min(xs.get().size(), b)}
    );
    // clang-format on

    // clang-format off
    template <class Wrapper, std::size_t N>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Wrapper&& xs,
        std::integral_constant<std::size_t, N> /*unused*/) const
    BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN
    (
        drop_impl<std::decay_t<Wrapper>,
            ((Wrapper::type::extent() > N)
                    ? (Wrapper::type::extent() - N)
                    : 0)>{std::forward<Wrapper>(xs), std::min(xs.get().size(), N)}
    );
    // clang-format on
};

struct make_drop_algo_impl {
    // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::size_t const n) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_drop_impl{}, std::size_t{n})
    );
    // clang-format on

    // clang-format off
    template <std::size_t N>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::integral_constant<std::size_t, N> /*unused*/) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_drop_impl{},
            std::integral_constant<std::size_t, N>{})
    );
    // clang-format on
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
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_drop_algo_impl, drop)

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
BOOST_STATIC_VIEWS_INLINE_VARIABLE(
    detail::make_drop_exactly_algo_impl, drop_exactly)

BOOST_STATIC_VIEWS_END_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Expanded from 'hashed.hpp'
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Expanded from 'find_first.hpp'
//////////////////////////////////////////////////////////////////////


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

struct find_first_i_impl {

  private:
    template <class V, class P>
    struct Constrains {

      private:
        template <class Predicate>
        struct Impl {
            template <class T>
            using element_type_t = decltype(
                concepts::View::unsafe_at(std::declval<T const&>(),
                    std::declval<std::size_t>()));

            template <class T>
            using has_call_t =
                decltype(invoke(std::declval<Predicate>(),
                    std::declval<element_type_t<T>>()));

            BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_callable, T,
                (is_detected<has_call_t, T>::value),
                "`boost::static_views::detail::find_first_i(xs, p)` "
                "requires `p` to be callable with `xs`' element "
                "type. "
                "This condition has, I'm afraid, been violated. "
                "Hence, "
                "the error.");

            BOOST_STATIC_VIEWS_DEFINE_CHECK(
                Call_has_correct_return_type, T,
                (std::is_convertible<detected_t<has_call_t, T>,
                    bool>::value),
                "`boost::static_views::detail::find_first_i(xs, p)` "
                "requires `p(xs[i])` to return something convertible "
                "to "
                "`bool`. That's, unfortunately, not the case here. "
                "Hence, the error.");

            template <class T>
            static constexpr auto test() noexcept -> bool
            {
                return concepts::and_<concepts::View, Is_callable,
                    Call_has_correct_return_type>::template test<T>();
            }
            template <class T>
            static constexpr auto check() noexcept -> bool
            {
                return concepts::and_<concepts::View, Is_callable,
                    Call_has_correct_return_type>::
                    template check<T>();
            }
        };

      public:
        static constexpr auto test() noexcept -> bool
        {
            return Impl<P>::template test<V>();
        }

        static constexpr auto check() noexcept -> bool
        {
            return Impl<P>::template check<V>();
        }
    };

  public:
    template <class View, class Predicate,
        class =
            std::enable_if_t<!Constrains<View, Predicate>::test()>,
        class = void>
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(
        View const& /*unused*/, Predicate&& /*unused*/) const noexcept
    {
        Constrains<View, Predicate>::check();
    }

    // clang-format off
    template <class View, class Predicate,
        class = std::enable_if_t<Constrains<View, Predicate>::test()>>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View const& xs, Predicate&& p) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            noexcept(xs.size()),
            noexcept(p(xs[std::declval<std::size_t>()]))
        ))
        -> std::size_t
    {
        std::size_t const n = xs.size();
        std::size_t       i = 0;

        while (i < n && !invoke(p, concepts::View::unsafe_at(xs, i))) {
            ++i;
        }
        return i;
    }
    // clang-format on
};

} // namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(
    detail::find_first_i_impl, find_first_i)

BOOST_STATIC_VIEWS_END_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Expanded from 'raw_view.hpp'
//////////////////////////////////////////////////////////////////////


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
/// :math:`\mathtt{raw\_view} : \mathtt{Sequence} \to \mathtt{View}`
/// (i.e.  takes a :ref:`sequence <sequence-concept>` and returns a
/// :ref:`view <view-concept>`). It will trigger ``static_assert``
/// failures if passed a type not modeling the :ref:`sequence
/// <sequence-concept>` concept.
/// \endverbatim
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_raw_view, raw_view)

BOOST_STATIC_VIEWS_END_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Expanded from 'slice.hpp'
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Expanded from 'take.hpp'
//////////////////////////////////////////////////////////////////////


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
#if defined(BOOST_STATIC_VIEWS_DISABLE_CHECKS)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(std::is_nothrow_constructible<
            typename take_impl::view_adaptor_base_type, View&&>::value
            // This is formally wrong, but come on,
            // std::min(size_t, size_t) _should_ be noexcept.
            )
#endif
        : take_impl::view_adaptor_base_type{std::move(xs)}, _n{n}
    {
        BOOST_STATIC_VIEWS_EXPECT(n <= this->parent().size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        BOOST_STATIC_VIEWS_EXPECT(
            extent() == dynamic_extent || extent() >= size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
    }

    /// \brief Returns the number of elements viewed.
    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const noexcept
        -> std::size_t
    {
        return _n;
    }

    BOOST_STATIC_VIEWS_CONSTEXPR static auto extent() noexcept
        -> std::ptrdiff_t
    {
        return Extent;
    }

    /// \brief "Maps" index \p i to the corresponding index in the
    /// parent view.

    /// For \f$ i \geq \text{xs.size}()\f$ behavior of this function
    /// is undefined
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto map(std::size_t const i) const
        noexcept
    {
        BOOST_STATIC_VIEWS_EXPECT(i < size(),
            "Which part of `map: [0, size()) --> N` you didn't "
            "understand?");
        return i;
    }

  private:
    std::size_t _n;
};

struct make_take_exactly_impl {
    // clang-format off
    template <class View, std::size_t N>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs,
        std::integral_constant<std::size_t, N> /*unused*/) const
    // clang-format on
#if defined(BOOST_STATIC_VIEWS_DISABLE_CHECKS)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            take_impl<std::decay_t<View>, View::type::extent()>{
                std::forward<View>(xs), N}))
#endif
    {
        BOOST_STATIC_VIEWS_EXPECT(N <= xs.get().size(),
            "take_exactly(n)(xs) expects `n <= xs.size()`.");
        BOOST_STATIC_VIEWS_EXPECT(
            View::type::extent() == dynamic_extent
                || View::type::extent() >= xs.get().size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return take_impl<std::decay_t<View>,
            ((View::type::extent() > static_cast<std::ptrdiff_t>(N))
                    ? static_cast<std::ptrdiff_t>(N)
                    : View::type::extent())>{
            std::forward<View>(xs), N};
    }

    // clang-format off
    template <class View>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs, std::size_t const n) const
#if defined(BOOST_STATIC_VIEWS_DISABLE_CHECKS)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            take_impl<std::decay_t<View>, View::type::extent()>{
                std::forward<View>(xs), n}))
#endif
    {
        BOOST_STATIC_VIEWS_EXPECT(n <= xs.get().size(),
            "take_exactly(n)(xs) expects `n <= xs.size()`.");
        BOOST_STATIC_VIEWS_EXPECT(
            View::type::extent() == dynamic_extent
                || View::type::extent() >= xs.get().size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return take_impl<std::decay_t<View>,
            View::type::extent()>{std::forward<View>(xs), n};
    }
    // clang-format on
};

struct make_take_exactly_algo_impl {
    // clang-format off
    template <std::size_t N>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(
        std::integral_constant<std::size_t, N> /*unused*/) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_take_exactly_impl{},
            std::integral_constant<std::size_t, N>{})
    );
    // clang-format on

    // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::size_t const n) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_take_exactly_impl{}, std::size_t{n})
    );
    // clang-format on
};

struct make_take_impl {
    // clang-format off
    template <class View, std::size_t N>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs,
        std::integral_constant<std::size_t, N> /*unused*/) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        take_impl<std::decay_t<View>,
            ((View::type::extent() == dynamic_extent
                 || static_cast<std::size_t>(View::type::extent()) > N)
                    ? static_cast<std::ptrdiff_t>(N)
                    : View::type::extent())>{
            std::forward<View>(xs), std::min(xs.get().size(), N)}
    );
    // clang-format on

    // clang-format off
    template <class View>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs, std::size_t const n) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        take_impl<std::decay_t<View>,
            View::type::extent()>{std::forward<View>(xs),
                std::min(n, xs.get().size())}
    );
    // clang-format on
};

struct make_take_algo_impl {
    // clang-format off
    template <std::size_t N>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(
        std::integral_constant<std::size_t, N> /*unused*/) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_take_impl{},
            std::integral_constant<std::size_t, N>{})
    );
    // clang-format on

    // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::size_t const n) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_take_impl{}, std::size_t{n})
    );
    // clang-format on
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
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_take_algo_impl, take)

BOOST_STATIC_VIEWS_INLINE_VARIABLE(
    detail::make_take_exactly_algo_impl, take_exactly)

BOOST_STATIC_VIEWS_END_NAMESPACE


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class DropTake>
struct slice_impl : DropTake {

  public:
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
    template <class View,
        // avoid clashes with copy/move constructors
        class = std::enable_if_t<!std::is_same<std::decay_t<View>,
            slice_impl<DropTake>>::value>>
    explicit BOOST_STATIC_VIEWS_CONSTEXPR slice_impl(View&& xs)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_constructible<DropTake, View&&>::value)
        : DropTake{std::forward<View>(xs)}
    {
    }

    /// \brief "Maps" index \p i to the corresponding index in the
    /// parent view.

    /// Let `xs` be of type #slice_impl. The following relation then
    /// holds
    ///
    /// `xs[i] = xs.` #parent `()[xs.` #map `(i)]`
    /// \f$ \forall i \in \{0, 1, \dots, \text{xs.size}()-1\}\f$.
    ///
    /// This function and the #parent() member function are what
    /// distinguish slice view from the "drop view of take view of
    /// view". Although slice is implemented in terms of the latter,
    /// this function goes two steps up and thus makes the rest of the
    /// library think that slice is a single view.
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto map(std::size_t const i) const
        noexcept
    {
        static_assert(
            noexcept(std::declval<DropTake const&>().parent()),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        static_assert(noexcept(std::declval<DropTake const&>().map(
                          std::declval<std::size_t>())),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        static_assert(
            noexcept(std::declval<DropTake const&>().parent().map(
                std::declval<std::size_t>())),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        auto const* p = static_cast<DropTake const*>(this);
        return p->parent().map(p->map(i));
    }

    /// \brief Returns the "parent" view.

    /// Let `xs_ptr` be a pointer to some view. The following
    /// relation then holds
    ///
    /// `xs_ptr = &` #slice `(b, e)(*xs_ptr).` #parent `(),`
    ///     \f$\forall \text{b},\text{e} \in \mathbb{N}\f$.
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO parent() const noexcept
    {
        static_assert(
            noexcept(
                std::declval<DropTake const&>().parent().parent()),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return static_cast<DropTake const*>(this)->parent().parent();
    }
};

struct make_slice_impl {
    // clang-format off
    template <class View>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs, std::size_t const b, std::size_t const e) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        slice_impl<std::decay_t<decltype(
            drop_exactly(b)(take_exactly(e)(std::forward<View>(xs).get())))>>{
            drop_exactly(b)(take_exactly(e)(std::forward<View>(xs).get()))}
    );
    // clang-format on
};

struct make_slice_algo_impl {
    // clang-format off
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(std::size_t const b, std::size_t const e) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_slice_impl{}, std::size_t{b}, std::size_t{e})
        /*algorithm_impl<make_slice_impl, std::size_t, std::size_t>(
            make_slice_impl{}, b, e)*/
    );
    // clang-format on
};

} // end namespace detail

/// \verbatim embed:rst:leading-slashes
/// :math:`\mathtt{slice} : \mathbb{N} \times \mathbb{N} \to
/// \text{View} \to \text{View}` is a functor that let's you create
/// slices of views. Given a index :math:`b \in \mathbb{N}`, an index
/// :math:`e \in \mathbb{N}` and a :ref:`view <view-concept>`
/// :math:`xs`, creates a view of elements of :math:`xs` with indices
/// :math:`i \in \mathbb{N} \cap [b, e)`. \endverbatim
BOOST_STATIC_VIEWS_INLINE_VARIABLE(
    detail::make_slice_algo_impl, slice)

BOOST_STATIC_VIEWS_END_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Expanded from 'through.hpp'
//////////////////////////////////////////////////////////////////////


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
template <class View, class Proxy>
struct through_impl
    : view_adaptor_base<through_impl<View, Proxy>, View> {

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
    explicit BOOST_STATIC_VIEWS_CONSTEXPR through_impl(
        View&& xs, Proxy&& proxy)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_constructible<
                typename through_impl::view_adaptor_base_type,
                View&&>::value,
            std::is_nothrow_move_constructible<Proxy>::value))
        : through_impl::view_adaptor_base_type{std::move(xs)}
        , _proxy{std::move(proxy)}
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
        return Proxy::type::extent();
    }

    /// \brief Returns the number of elements viewed.

    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. It just calls ``size()`` on the proxy view. This
    /// function never fails unless a call to ``proxy.size()``
    /// fails.
    /// \endverbatim
    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<Proxy const&>().get().size()))
    {
        return _proxy.get().size();
    }

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
    BOOST_STATIC_VIEWS_CONSTEXPR auto map(std::size_t const i) const
        noexcept
    /*
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(concepts::View::unsafe_at(
                std::declval<Proxy const&>().get(),
                std::declval<std::size_t>()))) -> std::size_t
    */
    {
        static_assert(noexcept(_proxy.get().unsafe_at(i)), "");
        return _proxy.get().unsafe_at(
            i); // concepts::View::unsafe_at(_proxy.get(), i);
    }

  private:
    // friend struct
    // BOOST_STATIC_VIEWS_NAMESPACE::view_adaptor_core_access;
    Proxy _proxy;
};

struct make_through_impl {
    // clang-format off
    template <class View, class Proxy>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs, Proxy&& proxy) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        through_impl<std::decay_t<View>,
            decltype(make_wrapper(std::forward<Proxy>(proxy).get()))>{
                std::forward<View>(xs),
                make_wrapper(std::forward<Proxy>(proxy).get())}
    );
    // clang-format on
};

struct make_through_algo_impl {
    // clang-format off
    template <class Proxy>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Proxy&& proxy) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm(make_through_impl{},
            make_wrapper(std::forward<Proxy>(proxy)))
        /*
        algorithm_impl<make_through_impl,
            decltype(make_wrapper(std::forward<Proxy>(proxy)))>{
            make_through_impl{}, make_wrapper(std::forward<Proxy>(proxy))}*/
    );
    // clang-format on
};

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
BOOST_STATIC_VIEWS_INLINE_VARIABLE(
    detail::make_through_algo_impl, through)

BOOST_STATIC_VIEWS_END_NAMESPACE


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class View, class Hasher>
struct Constrains {

  private:
    template <class H>
    struct Impl {
        template <class T>
        using call_t = decltype(invoke(std::declval<H>(),
            concepts::View::unsafe_at(
                std::declval<T>(), std::declval<std::size_t>())));

        BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_callable, T,
            (is_detected<call_t, T>::value),
            "`boost::static_views::hashed(hf)(xs)` requires hash "
            "function `hf` to be callable with `xs[i]` as an "
            "argument. This seems to be not the case here. Hence, "
            "the error.");

        BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_correct_return_type, T,
            (std::is_convertible<detected_t<call_t, T>,
                std::size_t>::value),
            "`boost::static_views::hashed(hf)(xs)` requires the "
            "return type of the hash function to be convertible to "
            "`size_t`, i.e. `size_t hash = invoke(hf, xs[i]);` "
            "should compile cleanly. This condition has, I'm afraid, "
            "been violated. Hence, the error.");
    };

    using constrains = concepts::and_<concepts::View,
        typename Impl<Hasher>::Is_callable,
        typename Impl<Hasher>::Has_correct_return_type>;

  public:
    static constexpr auto test() noexcept -> bool
    {
        return constrains::template test<View>();
    }

    static constexpr auto check() noexcept -> bool
    {
        return constrains::template check<View>();
    }
};

template <std::size_t BucketSize, std::size_t... Is>
struct hashed_init_impl {
    std::size_t storage[sizeof...(Is)];

    // clang-format off
    template <class View, class Hasher,
        class = std::enable_if_t<Constrains<View, Hasher>::test()>>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    explicit hashed_init_impl(View const& xs, Hasher const& h)
        : storage{((void)Is, hashed_init_impl::capacity())...}
    {
        auto const size = xs.size();
        for (std::size_t i = 0; i < size; ++i) {
            auto const hash = invoke(h, concepts::View::unsafe_at(xs, i));
            insert(i, bucket_size() * (hash % bucket_count()));
        }
    }
    // clang-format on

  private:
    static constexpr auto capacity() noexcept -> std::size_t
    {
        static_assert(sizeof...(Is) > 0,
            "[INTERNAL] Invalid use of hashed_init_impl.");
        return sizeof...(Is);
    }

    static constexpr auto bucket_size() noexcept -> std::size_t
    {
        static_assert(BucketSize > 0,
            "[INTERNAL] Invalid use of hashed_init_impl.");
        return BucketSize;
    }

    static constexpr auto bucket_count() noexcept -> std::size_t
    {
        static_assert(sizeof...(Is) % bucket_size() == 0,
            "[INTERNAL] Invalid use of hashed_init_impl.");
        return capacity() / bucket_size();
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto insert(std::size_t const i, std::size_t const guess) -> void
    {
#if defined(__cpp_constexpr) && __cpp_constexpr >= 201603
        // We have C++17 constexpr lambdas
        constexpr auto is_empty = [](auto const x) noexcept
        {
            return x == hashed_init_impl::capacity();
        };
#else
        struct is_empty_impl {
            BOOST_STATIC_VIEWS_CONSTEXPR
            auto operator()(std::size_t const x) const noexcept
            {
                return x == hashed_init_impl::capacity();
            }
        };
        constexpr is_empty_impl is_empty{};
#endif

        auto const bucket =
            slice(guess, guess + bucket_size())(raw_view(storage));
        auto const p = find_first_i(bucket, is_empty);

        if (p != bucket_size()) {
            storage[guess + p] = i;
        }
        else {
            make_full_bucket_error("Bucket is full.");
        }
    }
};

template <std::size_t BucketSize, class V, class H, std::size_t... Is>
BOOST_STATIC_VIEWS_CONSTEXPR auto make_hashed_init_impl(
    V const& xs, H const& h, std::index_sequence<Is...> /*unused*/)
{
    return hashed_init_impl<BucketSize, Is...>{xs, h};
}

template <std::size_t BucketCount, std::size_t BucketSize, class V,
    class H>
BOOST_STATIC_VIEWS_CONSTEXPR auto make_hashed_init_impl(
    V const& xs, H const& h)
{
    static_assert(
        std::numeric_limits<std::size_t>::max() / BucketCount
            >= BucketSize,
        "[INTERNAL] Overflow detected.");
    return make_hashed_init_impl<BucketSize>(
        xs, h, std::make_index_sequence<BucketCount * BucketSize>{});
}

template <std::size_t BucketCount, std::size_t BucketSize, class View,
    class Hasher>
struct hashed_impl
    : view_adaptor_base<
          hashed_impl<BucketCount, BucketSize, View, Hasher>, View> {

  private:
    using view_type = typename View::type;

    static_assert(is_wrapper<Hasher>(),
        "[INTERNAL] Invalid use of hashed_impl.");

    using hasher_type = typename Hasher::type;

    using value_type = std::remove_reference_t<decltype(
        std::declval<view_type const&>().unsafe_at(
            std::declval<std::size_t>()))>;

  public:
    /// \brief Returns the number buckets.
    static constexpr auto bucket_count() noexcept
    {
        static_assert(BucketCount > 0,
            "[INTERNAL] Invalid use of hashed_impl.");
        return BucketCount;
    }

    /// \brief Returns the capacity of each bucket.
    static constexpr auto bucket_size() noexcept
    {
        static_assert(
            BucketSize > 0, "[INTERNAL] Invalid use of hashed_impl.");
        return BucketSize;
    }

  private:
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    static auto empty(std::size_t const x) noexcept -> bool
    {
        return x == bucket_count() * bucket_size();
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
    template <std::size_t... Is>
    BOOST_STATIC_VIEWS_CONSTEXPR hashed_impl(View&& xs, Hasher&& hf,
        std::size_t (&storage)[hashed_impl::bucket_count()
                               * hashed_impl::bucket_size()],
        std::index_sequence<Is...> /*unused*/)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_constructible<
                typename hashed_impl::view_adaptor_base_type,
                View&&>::value,
            std::is_nothrow_move_constructible<Hasher>::value))
        : hashed_impl::view_adaptor_base_type{std::move(xs)}
        , _hf{std::move(hf)}
        , _storage{storage[Is]...}
    {
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
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO hash_function() const noexcept
    {
        static_assert(noexcept(_hf.get()),
            "[INTERNAL] hashed_impl assumes that _hf has a noexcept "
            "get().");
        return _hf.get();
    }

    /// \brief Returns the bucket corresponding to \p hash.

    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. It returns a view of elements that have hash
    /// ``hash``. If there are no such elements, the returned view
    /// will have size zero. \endverbatim
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator[](std::size_t const hash) const
    // noexcept /* TODO add specifiers */
    {
#if defined(__cpp_constexpr) && __cpp_constexpr >= 201603
        constexpr auto is_empty = [](auto const x) noexcept
        {
            return x == bucket_count() * bucket_size();
        };
#else
        struct is_empty_impl {
            BOOST_STATIC_VIEWS_CONSTEXPR
            auto operator()(std::size_t const x) const noexcept
            {
                return x == bucket_count() * bucket_size();
            }
        };
        constexpr is_empty_impl is_empty{};
#endif
        auto const i = bucket_size() * (hash % bucket_count());
        auto const n = find_first_i(
            slice(i, i + bucket_size())(raw_view(_storage)),
            is_empty);
        return through(slice(i, i + n)(raw_view(_storage)))(
            this->parent());
    }

    template <class Predicate>
    BOOST_STATIC_VIEWS_FORCEINLINE   //
        BOOST_STATIC_VIEWS_CONSTEXPR //
        auto
        lookup(std::size_t h, Predicate&& p) const noexcept
        -> value_type*
    {
        auto const bucket =
            slice(h, h + bucket_size())(raw_view(_storage));

        for (std::size_t i = 0;
             (i < bucket_size()) && !empty(bucket.unsafe_at(i));
             ++i) {

            auto* const x =
                &this->parent().unsafe_at(bucket.unsafe_at(i));
            if (invoke(p, *x)) {
                return x;
            }
        }
        return nullptr;
    }

  private:
    Hasher      _hf;
    std::size_t _storage[bucket_count() * bucket_size()];
};

template <std::size_t BucketCount, std::size_t BucketSize>
struct make_hashed_impl {

    static_assert(BucketCount > 0,
        "`boost::static_views::hashed<BucketCount, BucketSize>` "
        "requires BucketCount to be greater than zero.");
    static_assert(BucketCount > 0,
        "`boost::static_views::hashed<BucketCount, BucketSize>` "
        "requires BucketSize to be greater than zero.");

    // clang-format off
    template <class View, class Hasher>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs, Hasher&& hf) const
    {
        static_assert(is_wrapper<std::decay_t<View>>::value,
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        static_assert(is_wrapper<std::decay_t<Hasher>>::value,
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        using view_type = typename std::decay_t<View>::type;
        using hasher_type = typename std::decay_t<Hasher>::type;
        concepts::View::check<view_type>();
        Constrains<view_type, hasher_type>::check();

        auto const& xs_ref = xs.get();
        auto const& hf_ref = hf.get();
        auto init = detail::make_hashed_init_impl<
            BucketCount, BucketSize>(xs_ref, hf_ref);
        return detail::hashed_impl<BucketCount, BucketSize,
            std::decay_t<View>,
            decltype(make_wrapper(std::forward<Hasher>(hf).get()))>{
            std::forward<View>(xs),
            make_wrapper(std::forward<Hasher>(hf).get()),
            init.storage,
            std::make_index_sequence<BucketCount * BucketSize>{}};
    }
    // clang-format on
};

template <std::size_t BucketCount, std::size_t BucketSize>
struct make_hashed_algo_impl {
    template <class Hasher>
    BOOST_STATIC_VIEWS_FORCEINLINE BOOST_STATIC_VIEWS_CONSTEXPR auto
                                   operator()(Hasher&& hf) const
    {
        return algorithm(make_hashed_impl<BucketCount, BucketSize>{},
            make_wrapper(std::forward<Hasher>(hf)));
        /*
            decltype(make_wrapper(std::forward<Proxy>(proxy)))>{
            make_through_impl{},
           make_wrapper(std::forward<Proxy>(proxy))}*/
    }
};

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
template <std::size_t BucketCount, std::size_t BucketSize>
BOOST_STATIC_VIEWS_CONSTEXPR auto const& hashed =
    ::BOOST_STATIC_VIEWS_NAMESPACE::_static_const<
        detail::make_hashed_algo_impl<BucketCount, BucketSize>>;
} // anonymous namespace
#endif

BOOST_STATIC_VIEWS_END_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Expanded from 'pipe.hpp'
//////////////////////////////////////////////////////////////////////


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




#endif // BOOST_STATIC_VIEWS_ALL_IN_ONE_HPP
