//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP
#define BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP


#include <tuple>
#include <type_traits>
#include <utility>

#include <boost/config.hpp>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/detail/utils.hpp>
#include <boost/static_views/detail/wrapper.hpp>
#include <boost/static_views/view_base.hpp>


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE


struct algorithm_base {};


template <class V>
struct is_algorithm : std::is_base_of<algorithm_base, V> {};


namespace detail {
    template <class Function, class... Args>
    struct algorithm_impl : algorithm_base {

        explicit BOOST_STATIC_VIEWS_CONSTEXPR algorithm_impl(Function&& f,
            std::tuple<wrapper<Args>...>&& args)
#           if defined(NEGLECT_STD_TUPLE)
                // std::tuple's constructor is not marked noexcept in most of
                // the implementations. We "know", however, that the only thing
                // std::tuple's move constructor needs is for each Args to be
                // nothrow move constructible. Hence, we can "neglect" the fact
                // that std::tuple's move constructor is not noexcept.
                BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
                    std::is_nothrow_move_constructible<wrapper<Args>>::value...,
                    std::is_nothrow_move_constructible<Function>::value
                ))
#           else
                BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
                    std::is_nothrow_move_constructible<std::tuple<wrapper<
                    Args>...> >::value, std::is_nothrow_move_constructible<
                        Function>::value
                ))
#           endif
            : _func{ std::move(f) }
            , _args{ std::move(args) }
        {
        }

#       define FAKE_CALL_IMPL(qualifiers)                                     \
            std::declval<algorithm_impl qualifiers>().call_impl(              \
                std::forward<View>(xs),                                       \
                std::make_index_sequence<sizeof...(Args)>{}                   \
            )                                                                 \
            /**/

        template <class View> 
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator()(View&& xs) const&
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(FAKE_CALL_IMPL(const&)))
        {
            static_assert(is_view<std::decay_t<View>>::value, 
                "`View` must model the View concept.");
            return call_impl(std::forward<View>(xs),
                std::make_index_sequence<sizeof...(Args)>{});
        }

        template <class View> 
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator()(View&& xs) &
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(FAKE_CALL_IMPL(&)))
        {
            static_assert(is_view<std::decay_t<View>>::value, 
                "`View` must model the View concept.");
            return call_impl(std::forward<View>(xs),
                std::make_index_sequence<sizeof...(Args)>{});
        }

        template <class View> 
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator()(View&& xs) &&
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(FAKE_CALL_IMPL(&&)))
        {
            static_assert(is_view<std::decay_t<View>>::value, 
                "`View` must model the View concept.");
            return call_impl(std::forward<View>(xs),
                std::make_index_sequence<sizeof...(Args)>{});
        }

#       undef FAKE_CALL_IMPL

    private:
        Function                      _func;
        std::tuple<wrapper<Args>...>  _args;


        template <class View>
        static constexpr auto is_noexcept_call_cref() noexcept -> bool
        {
            return noexcept(invoke(
                std::declval<Function const&>(),
                make_wrapper(std::declval<View&&>()),
                std::declval<Args const&>()...));
        }

        template <class View>
        static constexpr auto is_noexcept_call_move() noexcept -> bool
        {
            return noexcept(invoke(
                std::declval<Function &&>(),
                make_wrapper(std::declval<View&&>()),
                std::declval<Args &&>()...));
        }


        template <class View, std::size_t... Is>
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        BOOST_STATIC_VIEWS_DECLTYPE_AUTO call_impl(View&& xs,
            std::index_sequence<Is...>) const&
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(is_noexcept_call_cref<View&&>())
        {
            return invoke(_func, make_wrapper(std::forward<View>(xs)),
                std::get<Is>(_args).get()...);
        }

        /* // I think that this overload makes little sense
        template <class View, std::size_t... Is>
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR BOOST_STATIC_VIEWS_DECLTYPE_AUTO call_impl(View&& xs,
            std::index_sequence<Is...>) &
        {
            return invoke(_func, make_wrapper(std::forward<View>(xs)),
                std::get<Is>(_args).get()...);
        }
        */

        template <class View, std::size_t... Is>
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        BOOST_STATIC_VIEWS_DECLTYPE_AUTO call_impl(View&& xs,
            std::index_sequence<Is...>) && 
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(is_noexcept_call_move<View&&>())
        {
            return invoke(std::move(_func),
                make_wrapper(std::forward<View>(xs)),
                std::get<Is>(std::move(_args)).get()...);
        }
    };


    template <class Function>
    struct make_algorithm_impl {
        template <class... Args>
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(Args&&... args) const
        BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
        (
            algorithm_impl<Function, Args&&...>(Function{},
                std::make_tuple(make_wrapper(std::forward<Args>(args))...))
        )
    };
} // end namespace detail


#define BOOST_STATIC_VIEWS_INLINE_ALGO_VARIABLE(type, name)                   \
    inline namespace {                                                        \
        BOOST_STATIC_VIEWS_CONSTEXPR auto const& name =                       \
            ::BOOST_STATIC_VIEWS_NAMESPACE::_static_const<                    \
            ::BOOST_STATIC_VIEWS_NAMESPACE::detail::make_algorithm_impl<      \
                type>>;                                                       \
    }                                                                         \
    /**/


BOOST_STATIC_VIEWS_END_NAMESPACE


#endif // BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP
