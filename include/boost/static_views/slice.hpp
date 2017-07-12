//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_STATIC_VIEWS_SLICE_HPP
#define BOOST_STATIC_VIEWS_SLICE_HPP


#include <boost/config.hpp>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/drop.hpp>
#include <boost/static_views/take.hpp>


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

    template <class DropTake>
    struct slice_impl : DropTake {
  
        template <class View>
        BOOST_STATIC_VIEWS_CONSTEXPR slice_impl(View&& xs)
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                std::is_nothrow_constructible<DropTake, View&&>::value)
            : DropTake{ std::forward<View>(xs) }
        {
        }

        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR auto map(std::size_t const i) const 
            -> std::size_t
        {
            auto const* p = static_cast<DropTake const*>(this);
            return p->parent().map(p->map(i));
        }

        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        BOOST_STATIC_VIEWS_DECLTYPE_AUTO parent() const
        {
            return static_cast<DropTake const*>(this)->parent().parent();
        }
    };

    struct make_slice_impl {
        template <class View>
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        auto operator()(View&& xs, std::size_t const b,
            std::size_t const e) const

        {
            auto v = drop(b)(take(e)(std::forward<View>(xs).get()));
            return slice_impl<std::decay_t<decltype(v)>>{ std::move(v) };
        }
    };
} // end namespace detail


BOOST_STATIC_VIEWS_INLINE_ALGO_VARIABLE(detail::make_slice_impl, slice)


BOOST_STATIC_VIEWS_END_NAMESPACE


#endif // BOOST_STATIC_VIEWS_SLICE_HPP
