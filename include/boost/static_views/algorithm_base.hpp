//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP
#define BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP


#include <tuple>
#include <type_traits>
#include <utility>

#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/detail/wrapper.hpp>
#include <boost/static_views/view_base.hpp>


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE


struct algorithm_base {};


template <class V>
struct is_algorithm : std::is_base_of<algorithm_base, V> {};


namespace detail {
    template <class Function, class... Args>
    struct algorithm_impl : algorithm_base {

        constexpr algorithm_impl(Function&& f,
            std::tuple<wrapper<Args>...>&& args)
            noexcept(
                std::is_nothrow_move_constructible<
                    std::tuple<wrapper<Args>...>>::value
             && std::is_nothrow_move_constructible<
                    Function>::value
            )
            : _func{ std::move(f) }
            , _args{ std::move(args) }
        {
        }

        template <class View> 
        constexpr decltype(auto) operator()(View&& xs) const
            noexcept /* TODO: add specification */
        {
            static_assert(is_view<std::decay_t<View>>::value, 
                "`View` must model the View concept.");
            return call_impl(std::forward<View>(xs),
                std::make_index_sequence<sizeof...(Args)>{});
        }

        template <class View> 
        constexpr decltype(auto) operator()(View&& xs)
            noexcept /* TODO: add specification */
        {
            static_assert(is_view<std::decay_t<View>>::value, 
                "`View` must model the View concept.");
            return call_impl(std::forward<View>(xs),
                std::make_index_sequence<sizeof...(Args)>{});
        }

    private:
        Function                      _func;
        std::tuple<wrapper<Args>...>  _args;

        template <class View, std::size_t... Is>
        constexpr decltype(auto) call_impl(View&& xs,
            std::index_sequence<Is...>) const&
            noexcept /* TODO: add specification */
        {
            return invoke(_func, make_wrapper(std::forward<View>(xs)),
                std::get<Is>(_args).get()...);
        }

        template <class View, std::size_t... Is>
        constexpr decltype(auto) call_impl(View&& xs,
            std::index_sequence<Is...>) &
            noexcept /* TODO: add specification */
        {
            return invoke(_func, make_wrapper(std::forward<View>(xs)),
                std::get<Is>(_args).get()...);
        }

        template <class View, std::size_t... Is>
        constexpr decltype(auto) call_impl(View&& xs,
            std::index_sequence<Is...>) && 
            noexcept /* TODO: add specification */
        {
            return invoke(std::move(_func), 
                make_wrapper(std::forward<View>(xs)),
                std::get<Is>(std::move(_args)).get()...);
        }

    };


    template <class Function>
    struct make_algorithm_impl {
        template <class... Args>
        constexpr auto operator()(Args&&... args) const
            noexcept /* TODO: add specification */
        {
            return algorithm_impl<Function, Args&&...>(Function{},
                std::make_tuple(make_wrapper(std::forward<Args>(args))...));
        }
    };
} // end namespace detail


#define BOOST_STATIC_VIEWS_INLINE_ALGO_VARIABLE(type, name)                   \
    inline namespace {                                                        \
        constexpr auto const& name =                                          \
            ::BOOST_STATIC_VIEWS_NAMESPACE::_static_const<                    \
            ::BOOST_STATIC_VIEWS_NAMESPACE::detail::make_algorithm_impl<      \
                type>>;                                                       \
    }                                                                         \
    /**/


BOOST_STATIC_VIEWS_END_NAMESPACE


#endif // BOOST_STATIC_VIEWS_ALGORITHM_BASE_HPP
