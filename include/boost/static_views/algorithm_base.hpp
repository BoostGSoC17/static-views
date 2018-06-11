//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP
#define BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP

#include "detail/config.hpp"
#include "detail/wrapper.hpp"
#include "view_base.hpp"
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
struct algorithm_impl : private std::tuple<Function, Args...> {

    using base_type = std::tuple<Function, Args...>;
    using self_type = algorithm_impl;
#if 0
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
#endif

    template <class Dummy>
    struct traits {
        using base_type = base_type;
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

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto const& base() const& noexcept
    {
        return static_cast<base_type const&>(*this);
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto& base() & noexcept { return static_cast<base_type&>(*this); }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto base()
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
    template <class View, std::size_t... Is>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) call_impl(View&& xs, std::index_sequence<Is...> /*unused*/)
        // clang-format on
        const& BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            is_call_noexcept<base_type const&, View>())
    {
        return invoke(std::get<0>(base()), std::forward<View>(xs),
            std::get<Is + 1>(base())...);
    }

    // clang-format off
    template <class View, std::size_t... Is>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) call_impl(View&& xs, std::index_sequence<Is...> /*unused*/) &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            is_call_noexcept<base_type&&, View>())
    // clang-format on
    {
        return invoke(std::get<0>(static_cast<algorithm_impl&&>(*this).base()),
            std::forward<View>(xs),
            std::get<Is + 1>(
                std::move(static_cast<algorithm_impl&&>(*this)))...);
    }

  public:
    BOOST_STATIC_VIEWS_CONSTEXPR
    algorithm_impl(Function f, Args... args)
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
    BOOST_STATIC_VIEWS_CONSTEXPR
    algorithm_impl(algorithm_impl const& other)
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            is_base_nothrow_copy_constructible()) : base_type{other.base()}
    {
    }

    // clang-format off
    template <class Dummy
        BOOST_STATIC_VIEWS_REQUIRES(
            std::is_move_constructible<
                typename traits<Dummy>::base_type>::value)
    BOOST_STATIC_VIEWS_CONSTEXPR
    algorithm_impl(algorithm_impl&& other)
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
    BOOST_STATIC_VIEWS_CONSTEXPR
    algorithm_impl& operator=(algorithm_impl const& other)
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
    BOOST_STATIC_VIEWS_CONSTEXPR
    algorithm_impl& operator=(algorithm_impl&& other)
    // clang-format on
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                is_base_nothrow_move_assignable())
    {
        base() = std::move(other).base();
        return *this;
    }

    // clang-format off
    template <class V
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) operator()(V&& xs) const&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<algorithm_impl const&>().call_impl(
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
    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) operator()(V&& xs) &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<self_type&&>().call_impl(
                std::forward<V>(xs),
                std::make_index_sequence<sizeof...(Args)>{})))
    // clang-format on
    {
        return static_cast<self_type&&>(*this).call_impl(
            std::forward<V>(xs), std::make_index_sequence<sizeof...(Args)>{});
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
            std::decay_t<Args>...>(std::forward<Function>(fn),
                std::forward<Args>(args)...)
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

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_algorithm_impl, adaptor)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP
