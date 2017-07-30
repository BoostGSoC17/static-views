//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP
#define BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP

#include <tuple>
#include <type_traits>
#include <utility>

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
    Function                     _func;
    std::tuple<wrapper<Args>...> _args;

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
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            concepts::and_<Is_cref_callable, Is_noexcept_cref_call>
                ::template test<View&&>())
    {
        return invoke(_func, make_wrapper(std::forward<View>(xs)),
            std::get<Is>(_args).get()...);
    }
    // clang-format on

    // clang-format off
    template <class View, std::size_t... Is>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO call_impl(View&& xs,
        std::index_sequence<Is...> /*unused*/) &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            concepts::and_<Is_move_callable, Is_noexcept_move_call>
                ::template test<View&&>())
    {
        return invoke(std::move(_func),
            make_wrapper(std::forward<View>(xs)),
            std::get<Is>(std::move(_args)).get()...);
    }
    // clang-format on

  public:

    explicit BOOST_STATIC_VIEWS_CONSTEXPR algorithm_impl(
        Function&& f, wrapper<Args>&&... args)
#if defined(BOOST_STATIC_VIEWS_NEGLECT_STD_TUPLE)
        // std::tuple's constructor is not marked noexcept in most of
        // the implementations. We "know", however, that the only
        // thing std::tuple's move constructor needs is for each Args
        // to be nothrow move constructible. Hence, we can "neglect"
        // the fact that std::tuple's move constructor is not
        // noexcept.
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            utils::all(std::is_nothrow_move_constructible<
                           wrapper<Args>>::value...,
                std::is_nothrow_move_constructible<Function>::value))
#else
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            utils::all(std::is_nothrow_move_constructible<
                           std::tuple<wrapper<Args>...>>::value,
                std::is_nothrow_move_constructible<Function>::value))
#endif
        : _func{std::move(f)}, _args{std::move(args)...}
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
        static_assert(concepts::View::template test<
            std::remove_cv_t<std::remove_reference_t<View>>>(),
            "I'm sorry, but `View` must model the View concept!");
        concepts::View::template
            check<std::remove_cv_t<std::remove_reference_t<View>>>();
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    algorithm_impl(algorithm_impl const& other)
#if defined(BOOST_STATIC_VIEWS_NEGLECT_STD_TUPLE)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_copy_constructible<Function>::value,
            std::is_nothrow_copy_constructible<
                wrapper<Args>>::value...))
#else
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_copy_constructible<Function>::value,
            std::is_nothrow_copy_constructible<
                std::tuple<wrapper<Args>...>>::value))
#endif
        : _func{other._func}, _args{other._args}
    {
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    algorithm_impl(algorithm_impl&& other)
#if defined(BOOST_STATIC_VIEWS_NEGLECT_STD_TUPLE)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_move_constructible<Function>::value,
            std::is_nothrow_move_constructible<
                wrapper<Args>>::value...))
#else
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_move_constructible<Function>::value,
            std::is_nothrow_move_constructible<
                std::tuple<wrapper<Args>...>>::value))
#endif
        : _func{std::move(other._func)}, _args{std::move(other._args)}
    {
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    algorithm_impl& operator=(algorithm_impl const& other)
#if defined(BOOST_STATIC_VIEWS_NEGLECT_STD_TUPLE)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_copy_assignable<Function>::value,
            std::is_nothrow_copy_assignable<wrapper<Args>>::value...))
#else
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_copy_assignable<Function>::value,
            std::is_nothrow_copy_assignable<
                std::tuple<wrapper<Args>...>>::value))
#endif
    {
        _func = other._func;
        _args = other._args;
        return *this;
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    algorithm_impl& operator=(algorithm_impl&& other)
#if defined(BOOST_STATIC_VIEWS_NEGLECT_STD_TUPLE)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_move_assignable<Function>::value,
            std::is_nothrow_move_assignable<wrapper<Args>>::value...))
#else
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_move_assignable<Function>::value,
            std::is_nothrow_move_assignable<
                std::tuple<wrapper<Args>...>>::value))
#endif
    {
        _func = std::move(other._func);
        _args = std::move(other._args);
        return *this;
    }

    ~algorithm_impl() = default;

};

template <class Function>
struct make_algorithm_impl {
    // clang-format off
    template <class... Args>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Args&&... args) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        algorithm_impl<Function, Args&&...>(
            Function{}, make_wrapper(std::forward<Args>(args))...)
    );
    // clang-format on
};

} // namespace detail

#define BOOST_STATIC_VIEWS_INLINE_ALGO_VARIABLE(type, name)          \
    inline namespace {                                               \
        BOOST_STATIC_VIEWS_CONSTEXPR auto const& name =              \
            ::BOOST_STATIC_VIEWS_NAMESPACE::_static_const<           \
                ::BOOST_STATIC_VIEWS_NAMESPACE::detail::             \
                    make_algorithm_impl<type>>;                      \
    }                                                                \
    /**/

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP
