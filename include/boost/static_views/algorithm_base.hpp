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
    template <class V, std::size_t... Is>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
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
    BOOST_STATIC_VIEWS_CONSTEXPR
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
    BOOST_STATIC_VIEWS_CONSTEXPR
    lazy_adaptor_impl(Function f, Args... args)
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
    BOOST_STATIC_VIEWS_CONSTEXPR
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
    BOOST_STATIC_VIEWS_CONSTEXPR
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
    BOOST_STATIC_VIEWS_CONSTEXPR
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
    BOOST_STATIC_VIEWS_CONSTEXPR
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
    BOOST_STATIC_VIEWS_CONSTEXPR
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
    BOOST_STATIC_VIEWS_CONSTEXPR
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
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Function&& fn, Args&&... args) const
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        lazy_adaptor_impl<std::decay_t<Function>,
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

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_lazy_adaptor_impl, lazy_adaptor)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP
