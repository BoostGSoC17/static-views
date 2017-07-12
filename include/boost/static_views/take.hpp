//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_STATIC_VIEWS_TAKE_HPP
#define BOOST_STATIC_VIEWS_TAKE_HPP


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
    struct take_impl : view_adaptor_base<take_impl<View>, View> {

        explicit BOOST_STATIC_VIEWS_CONSTEXPR take_impl(View&& xs,
            std::size_t const n) 
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
                std::is_nothrow_constructible<typename 
                    take_impl::view_adaptor_base_type, View&&>::value,
                noexcept(std::min(std::declval<std::size_t>(),
                    std::declval<std::size_t>())),
                take_impl::is_noexcept_parent_size()
            ))
            : take_impl::view_adaptor_base_type{ std::move(xs) }
            , _n{ std::min(this->parent().size(), n) }
        {
        }

        BOOST_STATIC_VIEWS_CONSTEXPR auto size() const noexcept
        {
            return _n; 
        }

        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR auto map(std::size_t const i) const
        {
            return i < size()
                ? i
                : ((void)make_out_of_bound_error(
                       "`i < size()` not satisfied."), i);
        }

    private:
        friend struct BOOST_STATIC_VIEWS_NAMESPACE::view_adaptor_core_access;
        std::size_t _n;

        static constexpr auto is_noexcept_parent_size() noexcept
        {
            return noexcept(std::declval<take_impl const&>().parent().size());
        }
    };

    struct make_take_impl {
        template <class View>
        constexpr auto operator()(View&& xs, std::size_t const n) const 
        BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
        (
            take_impl<std::decay_t<View>>{std::forward<View>(xs), n}
        )
    };
} // end namespace detail


/// \brief Given a source range and an integral count, returns a range
/// consisting of the first count elements, or the complete range if
/// it has fewer elements.
#if defined(DOXYGEN_IN_HOUSE)
constexpr auto take = [](auto const n) { return [n](auto&& view) { ... }; };
#else
BOOST_STATIC_VIEWS_INLINE_ALGO_VARIABLE(detail::make_take_impl, take)
#endif


BOOST_STATIC_VIEWS_END_NAMESPACE


#endif // BOOST_STATIC_VIEWS_TAKE_HPP
