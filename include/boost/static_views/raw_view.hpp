//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Implementation of boost::static_views::raw_view.


#ifndef BOOST_STATIC_VIEWS_RAW_VIEW_HPP
#define BOOST_STATIC_VIEWS_RAW_VIEW_HPP

#include <type_traits>
#include <boost/static_views/view_base.hpp>
#include <boost/static_views/sequence_traits.hpp>


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
    template <class Sequence>
    struct raw_view_impl : view_base {

        explicit constexpr raw_view_impl(Sequence& xs) noexcept
            : _xs{ xs }
        {
        }

        constexpr raw_view_impl(raw_view_impl const&)            = default;
        constexpr raw_view_impl(raw_view_impl &&)                = default;
        constexpr raw_view_impl& operator=(raw_view_impl const&) = default;
        constexpr raw_view_impl& operator=(raw_view_impl &&)     = default;


        static constexpr auto capacity() noexcept
        {
            return sequence_traits<sequence_type>::size(); 
        }


        constexpr auto size() const noexcept
        { 
            return sequence_traits<sequence_type>::size(); 
        }


        constexpr decltype(auto) operator[](std::size_t const i) const
        {
            if (i >= size()) {
                throw std::out_of_range{"detail::raw_view_impl::operator[]: "
                    "index `i` out of bounds."};
            }
            return sequence_traits<sequence_type>::at(_xs, i);
        }

        constexpr decltype(auto) operator[](std::size_t const i)
        {
            if (i >= size()) {
                throw std::out_of_range{"detail::raw_view_impl::operator[]: "
                    "index `i` out of bounds."};
            }
            return sequence_traits<sequence_type>::at(_xs, i);
        }

    private:
        using sequence_type = std::remove_const_t<
                              std::remove_reference_t<
                                  Sequence >>;
        Sequence& _xs;
    };

    struct make_raw_view {
        template <class Sequence>
        constexpr auto operator()(Sequence& sequence) const noexcept
        {
            return raw_view_impl<Sequence>(sequence);
        }
    };
} // end namespace detail


BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_raw_view, raw_view)  


BOOST_STATIC_VIEWS_END_NAMESPACE


#endif // BOOST_STATIC_VIEWS_RAW_VIEW_HPP
