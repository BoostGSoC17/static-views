//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_DETAIL_UTILS_HPP
#define BOOST_STATIC_VIEWS_DETAIL_UTILS_HPP

#include "config.hpp"
#include <tuple>
#include <type_traits>

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

#endif // BOOST_STATIC_VIEWS_DETAIL_UTILS_HPP
