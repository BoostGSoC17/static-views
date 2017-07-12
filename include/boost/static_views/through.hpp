//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_STATIC_VIEWS_THROUGH_HPP
#define BOOST_STATIC_VIEWS_THROUGH_HPP


#include <type_traits>

#include <boost/config.hpp>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/view_base.hpp>
#include <boost/static_views/algorithm_base.hpp>


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
    template <class View, class Proxy>
    struct through_impl : view_adaptor_base<through_impl<View, Proxy>, View> {
        BOOST_STATIC_VIEWS_CONSTEXPR through_impl(View&& xs, Proxy&& proxy)
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                std::is_nothrow_constructible<typename
                    through_impl::view_adaptor_base_type, View&&>::value
             && std::is_nothrow_move_constructible<Proxy>::value )
            : through_impl::view_adaptor_base_type{ std::move(xs) }
            , _proxy{ std::move(proxy) }
        {
        }

        static BOOST_STATIC_VIEWS_CONSTEXPR auto capacity() noexcept
        {
            return typename Proxy::type::capacity();
        }

        BOOST_STATIC_VIEWS_CONSTEXPR auto size() const
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
                std::declval<Proxy const&>().get().size() ))
        {
            return _proxy.get().size();
        }


    private:
        friend struct BOOST_STATIC_VIEWS_NAMESPACE::view_adaptor_core_access;
        Proxy _proxy;

        BOOST_STATIC_VIEWS_CONSTEXPR auto map(std::size_t const i) const
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
                std::declval<Proxy const&>().get()[std::declval<std::size_t>()]
            ))
            -> std::size_t
        {
            return _proxy.get()[i];
        }
    };

    struct make_through_impl {
        template <class View, class Proxy>
        BOOST_STATIC_VIEWS_CONSTEXPR
        auto operator()(View&& xs, Proxy&& proxy) const
        BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
        (
            through_impl<std::decay_t<View>, std::decay_t<
                decltype(make_wrapper(std::forward<Proxy>(proxy)))>>{
                std::forward<View>(xs),
                make_wrapper(std::forward<Proxy>(proxy))}
        )
    };
} // end namespace detail


BOOST_STATIC_VIEWS_INLINE_ALGO_VARIABLE(detail::make_through_impl, through)


BOOST_STATIC_VIEWS_END_NAMESPACE


#endif // BOOST_STATIC_VIEWS_THROUGH_HPP

