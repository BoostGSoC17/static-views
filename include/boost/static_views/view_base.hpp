//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Implementation of boost::static_views::view_base,
// boost::static_views::is_view, boost::static_views::view_adaptor_core_access
// and boost::static_views::view_adaptor_base.

#ifndef BOOST_STATIC_VIEWS_VIEW_BASE_HPP
#define BOOST_STATIC_VIEWS_VIEW_BASE_HPP


#include <type_traits>

#include <boost/config.hpp>
#include <boost/static_views/detail/config.hpp>


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE


/// \brief Base class for all the views.
struct view_base {};


/// \brief Check whether `V` models the View concept.
template <class V> struct is_view : std::is_base_of<view_base, V> {};


struct view_adaptor_core_access {
    template <class T, class V> friend struct view_adaptor_base;

private:

    template <class View>
    static constexpr auto map(View const& xs, std::size_t const i) 
    BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
    (
        xs.map(i)
    )
};


template <class Derived, class View>
struct view_adaptor_base : view_base {

private:
    using derived_type = Derived;
    using wrapper_type = View;
    using view_type    = typename View::type;

protected:
    using view_adaptor_base_type = view_adaptor_base<Derived, View>;

public:
    explicit constexpr view_adaptor_base(View&& view) 
        noexcept(std::is_nothrow_move_constructible<View>::value)
        : _xs{ std::move(view) }
    {
    }

    constexpr view_adaptor_base(view_adaptor_base const&)            = default;
    constexpr view_adaptor_base(view_adaptor_base &&)                = default;
    constexpr view_adaptor_base& operator=(view_adaptor_base const&) = default;
    constexpr view_adaptor_base& operator=(view_adaptor_base &&)     = default;


    static constexpr auto capacity() noexcept 
    { return view_type::capacity(); }


    BOOST_FORCEINLINE
    constexpr auto size() const
        noexcept(noexcept(
            std::declval<view_adaptor_base const&>().parent().size()
        ))
    {
        return parent().size(); 
    }


    BOOST_FORCEINLINE
    constexpr decltype(auto) parent() const&
        noexcept(noexcept(
            std::declval<wrapper_type const&>().get()
        ))
    {
        return _xs.get();
    }

    BOOST_FORCEINLINE
    constexpr decltype(auto) parent() &
        noexcept(noexcept(
            std::declval<wrapper_type &>().get()
        ))
    {
        return _xs.get();
    }

    constexpr decltype(auto) parent() &&
        noexcept(noexcept(
            std::declval<wrapper_type &&>().get()
        ))
    {
        return std::move(_xs).get();
    }


    BOOST_FORCEINLINE
    constexpr decltype(auto) operator[](std::size_t const i) const&
        noexcept(noexcept(
            std::declval<view_adaptor_base const&>().parent()[
                view_adaptor_core_access::map(
                    std::declval<derived_type const&>(),
                    std::declval<std::size_t>() )]
        ))
    {
        return parent()[view_adaptor_core_access::map(derived(), i)];
    }

    BOOST_FORCEINLINE
    constexpr decltype(auto) operator[](std::size_t const i) &
        noexcept(noexcept(
            std::declval<view_adaptor_base &>().parent()[
                view_adaptor_core_access::map(
                    std::declval<derived_type const&>(),
                    std::declval<std::size_t>() )]
        ))
    {
        return parent()[view_adaptor_core_access::map(derived(), i)];
    }

    BOOST_FORCEINLINE
    constexpr decltype(auto) operator[](std::size_t const i) &&
        noexcept(noexcept(
            std::declval<view_adaptor_base &&>().parent()[
                view_adaptor_core_access::map(
                    std::declval<derived_type const&>(),
                    std::declval<std::size_t>() )]
        ))
    {
        return parent()[view_adaptor_core_access::map(derived(), i)];
    }
    

private:
    View _xs;


    BOOST_FORCEINLINE
    constexpr auto derived() const noexcept -> derived_type const&
    {
        return *static_cast<derived_type const*>(this);
    }
};



BOOST_STATIC_VIEWS_END_NAMESPACE


#endif // BOOST_STATIC_VIEWS_VIEW_BASE_HPP
