//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_STATIC_VIEWS_DROP_HPP
#define BOOST_STATIC_VIEWS_DROP_HPP


#include <type_traits>

#include <boost/config.hpp>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/detail/utils.hpp>
#include <boost/static_views/errors.hpp>
#include <boost/static_views/view_base.hpp>
#include <boost/static_views/algorithm_base.hpp>


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
    template <class View>
    struct drop_impl : view_adaptor_base<drop_impl<View>, View> {

        explicit BOOST_STATIC_VIEWS_CONSTEXPR drop_impl(View&& xs,
            std::size_t const b)
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
                std::is_nothrow_constructible<typename 
                    drop_impl::view_adaptor_base_type, View&&>::value,
                noexcept(std::min(std::declval<std::size_t>(),
                    std::declval<std::size_t>())),
                drop_impl::is_noexcept_parent_size()
            ))
            : drop_impl::view_adaptor_base_type{ std::move(xs) }
            , _b{ std::min(this->parent().size(), b) }
        {
        }

        BOOST_STATIC_VIEWS_CONSTEXPR auto size() const 
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(drop_impl::is_noexcept_parent_size())
        {
            return this->parent().size() - _b;
        }

        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR auto map(std::size_t const i) const
        {
            return i < size()
                ? (_b + i)
                : ((void)make_out_of_bound_error(
                       "`i < size()` not satisfied."), _b + i);
        }

    private:
        friend struct BOOST_STATIC_VIEWS_NAMESPACE::view_adaptor_core_access;
        std::size_t _b;

        static constexpr auto is_noexcept_parent_size()
        {
            return noexcept(std::declval<drop_impl const&>().parent().size());
        }
    };

    struct make_drop_impl {
        template <class View>
        BOOST_STATIC_VIEWS_CONSTEXPR
        auto operator()(View&& xs, std::size_t const b) const 
        BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
        (
            drop_impl<std::decay_t<View>>{std::forward<View>(xs), b}
        )
    };
} // end namespace detail


/// \brief Given an integral count, return a range consisting of all but the 
/// first count elements from the source range, or an empty range if it has 
/// fewer elements.
BOOST_STATIC_VIEWS_INLINE_ALGO_VARIABLE(detail::make_drop_impl, drop)


BOOST_STATIC_VIEWS_END_NAMESPACE


#endif // BOOST_STATIC_VIEWS_DROP_HPP
