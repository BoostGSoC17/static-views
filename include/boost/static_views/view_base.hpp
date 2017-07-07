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

/// By deriving from #view_base, you tell StaticView that the derived class
/// models the View concept.
struct view_base {};


/// \brief Checks whether `V` models the View concept.

/// \verbatim embed:rst:leading-slashes
/// Metafunction that returns whether ``V`` models the :ref:`View
/// <view-concept>` concept:
/// 
/// .. code-block:: cpp
/// 
///   template <class View> 
///   struct is_view : std::is_base_of<view_base, View> 
///   {};
///
/// \endverbatim
template <class V> struct is_view : std::is_base_of<view_base, V> {};


/// \brief Helper class one can befriend to give StaticViews access to the
/// private `map` function.

/// Defines a single private member function
/// \code
/// template <class View>
/// static constexpr auto map(View const& xs, std::size_t const i)
///     noexcept( whenever possible );
/// \endcode
/// that calls `xs.map(i)`. By befriending this class, you give
/// #boost::static_views::view_adaptor_base access to private `map` function,
/// but nothing else. The very same technique is used in
/// [Boost.Iterator](http://www.boost.org/doc/libs/1_64_0/libs/iterator/doc/iterator_facade.html#iterator-core-access).
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


/// \brief
/// \verbatim embed:rst:leading-slashes
/// Base class to that helps with modeling the :ref:`View <view-concept>`
/// concept.
/// \endverbatim
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

    /// \name Copy and move constructors/assignments
    /// 
    /// Defines default copy and move constructors and assignments, i.e. is
    /// copy/move-constructible/assignable if \p View is.
    /// \{
    constexpr view_adaptor_base(view_adaptor_base const&)            = default;
    constexpr view_adaptor_base(view_adaptor_base &&)                = default;
    constexpr view_adaptor_base& operator=(view_adaptor_base const&) = default;
    constexpr view_adaptor_base& operator=(view_adaptor_base &&)     = default;
    /// \}


    /// \brief 
    /// \verbatim embed:rst:leading-slashes
    /// Default implementation of the ``capacity`` function required by the
    /// :ref:`view concept <view-concept>`.
    /// \endverbatim

    /// Just calls `capacity()` on the underlying view. 
    static constexpr auto capacity() noexcept 
    { return view_type::capacity(); }


    /// \brief 
    /// \verbatim embed:rst:leading-slashes
    /// Default implementation of the ``size`` function required by the
    /// :ref:`view concept <view-concept>`.
    /// \endverbatim

    /// Just calls `size()` on the underlying view. 
    constexpr auto size() const
    	BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<view_adaptor_base const&>().parent().size()
        ))
    {
        return parent().size(); 
    }

    
    /// \brief Returns the underlying view.
    /// \{
#if defined(DOXYGEN_IN_HOUSE)
    constexpr const underlying_view_type& parent() const&
#else
    constexpr decltype(auto) parent() const&
#endif
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<wrapper_type const&>().get()
        ))
    {
        return _xs.get();
    }

/*
#if defined(DOXYGEN_IN_HOUSE)
    constexpr underlying_view_type& parent() &
#else
    constexpr decltype(auto) parent() &
#endif
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<wrapper_type &>().get()
        ))
    {
        return _xs.get();
    }
*/

#if defined(DOXYGEN_IN_HOUSE)
    constexpr underlying_view_type&& parent() &&
#else
    constexpr decltype(auto) parent() &&
#endif
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<wrapper_type &&>().get()
        ))
    {
        return std::move(_xs).get();
    }
    /// \}


    /// \name Element access
    /// \{
#if defined(DOXYGEN_IN_HOUSE)
    constexpr const element_type& operator[](std::size_t const i) const&
#else
    constexpr decltype(auto) operator[](std::size_t const i) const&
#endif
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<view_adaptor_base const&>().parent()[
                view_adaptor_core_access::map(
                    std::declval<derived_type const&>(),
                    std::declval<std::size_t>() )]
        ))
    {
        return parent()[view_adaptor_core_access::map(derived(), i)];
    }

#if defined(DOXYGEN_IN_HOUSE)
    constexpr element_type& operator[](std::size_t const i) &
#else
    constexpr decltype(auto) operator[](std::size_t const i) &
#endif
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<view_adaptor_base &>().parent()[
                view_adaptor_core_access::map(
                    std::declval<derived_type const&>(),
                    std::declval<std::size_t>() )]
        ))
    {
        return parent()[view_adaptor_core_access::map(derived(), i)];
    }

#if defined(DOXYGEN_IN_HOUSE)
    constexpr element_type&& operator[](std::size_t const i) &&
#else
    constexpr decltype(auto) operator[](std::size_t const i) &&
#endif
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<view_adaptor_base &&>().parent()[
                view_adaptor_core_access::map(
                    std::declval<derived_type const&>(),
                    std::declval<std::size_t>() )]
        ))
    {
        return parent()[view_adaptor_core_access::map(derived(), i)];
    }
    /// \} 

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
