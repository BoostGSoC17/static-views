//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_CONCAT_HPP
#define BOOST_STATIC_VIEWS_CONCAT_HPP

#include <type_traits>

#include <boost/config.hpp>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/view_base.hpp>
#include <boost/static_views/algorithm_base.hpp>


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

    namespace {
        constexpr auto all() noexcept -> bool
        {
            return true;
        }

        template <class... Bools>
        constexpr auto all(bool x, Bools... xs) noexcept -> bool
        {
            return x && all(xs...);
        }
    } // end unnamed namespace


    template <class... Views>
    struct concat_impl : view_base {

        BOOST_STATIC_VIEWS_CONSTEXPR concat_impl(Views&&... xs) BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            all(std::is_nothrow_move_constructible<Views>::value...))
            : _views{std::move(xs)...}
        {
        }

        static BOOST_STATIC_VIEWS_CONSTEXPR auto capacity() noexcept -> std::size_t
        {
            BOOST_STATIC_VIEWS_CONSTEXPR std::size_t capacities[] = { 
                typename Views::type::capacity()... };
            return lfold(raw_view(capacities), std::size_t{});
        }

        BOOST_STATIC_VIEWS_CONSTEXPR auto size() 
            noexcept /* TODO add specifiers */ 
            -> std::size_t
        {
            std::size_t const capacities[] = { 
                typename Views::type::capacity()... };
            return lfold(raw_view(capacities), std::size_t{});
        }
        
        
    private:
        std::tuple<Views...> _views;
    };

} // end namespace detail





BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_CONCAT_HPP
