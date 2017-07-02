//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


// Implementation of boost::static_views::invoke. It is a constexpr equivalent
// of std::invoke.


#ifndef BOOST_STATIC_VIEWS_DETAIL_INVOKE_HPP
#define BOOST_STATIC_VIEWS_DETAIL_INVOKE_HPP

#include <type_traits>

#include <boost/config.hpp>
#include <boost/static_views/detail/config.hpp>


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE


namespace detail {

    template <class T>
    struct is_reference_wrapper : std::false_type {};

    /// \cond
    template <class T>
    struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type {};
    /// \endcond

    template <class Any, class Pointer, class Object, class... Args>
    BOOST_FORCEINLINE
    constexpr decltype(auto) invoke_member_func_impl
        ( std::true_type  /*is_base_of*/
        , Any             /*is_reference_wrapper*/
        , Pointer f
        , Object&& obj
        , Args&&... args )
    BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
    (
        (std::forward<Object>(obj).*f)(std::forward<Args>(args)...)
    )

    template <class Pointer, class Object, class... Args>
    BOOST_FORCEINLINE
    constexpr decltype(auto) invoke_member_func_impl
        ( std::false_type /*is_base_of*/
        , std::true_type  /*is_reference_wrapper*/
        , Pointer f
        , Object&& obj
        , Args&&... args )
    BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
    (
        (std::forward<Object>(obj).get().*f)(std::forward<Args>(args)...)
    )

    template <class Pointer, class Object, class... Args>
    BOOST_FORCEINLINE
    constexpr decltype(auto) invoke_member_func_impl
        ( std::false_type /*is_base_of*/
        , std::false_type /*is_reference_wrapper*/
        , Pointer f
        , Object&& obj
        , Args&&... args )
    BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
    (
        ((*std::forward<Object>(obj)).*f)(std::forward<Args>(args)...)
    )

    template <class Function, class T, class Object, class... Args>
    BOOST_FORCEINLINE
    constexpr decltype(auto) invoke_member_func
        ( Function (T::*f)
        , Object&& obj
        , Args&&... args ) 
    BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
    (
        invoke_member_func_impl
            ( std::is_base_of<T, std::decay_t<Object>>{}
            , is_reference_wrapper<std::decay_t<Object>>{}
            , f
            , std::forward<Object>(obj)
            , std::forward<Args>(args)... )
    )


    template <class Any, class Pointer, class Object>
    BOOST_FORCEINLINE
    constexpr decltype(auto) invoke_member_data_impl
        ( std::true_type  /*is_base_of*/
        , Any             /*is_reference_wrapper*/
        , Pointer f
        , Object&& obj )
    BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
    (
        std::forward<Object>(obj).*f
    )

    template <class Pointer, class Object>
    BOOST_FORCEINLINE
    constexpr decltype(auto) invoke_member_data_impl
        ( std::false_type /*is_base_of*/
        , std::true_type  /*is_reference_wrapper*/
        , Pointer f
        , Object&& obj )
    BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
    (
        std::forward<Object>(obj).get().*f
    )

    template <class Pointer, class Object>
    BOOST_FORCEINLINE
    constexpr decltype(auto) invoke_member_data_impl
        ( std::false_type /*is_base_of*/
        , std::false_type /*is_reference_wrapper*/
        , Pointer f
        , Object&& obj )
    BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
    (
        (*std::forward<Object>(obj)).*f
    )

    template <class Function, class T, class Object>
    BOOST_FORCEINLINE
    constexpr decltype(auto) invoke_member_data
        ( Function (T::*f)
        , Object&& obj ) 
    BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
    (
        invoke_member_data_impl
            ( std::is_base_of<T, std::decay_t<Object>>{}
            , is_reference_wrapper<std::decay_t<Object>>{}
            , f
            , std::forward<Object>(obj) )
    )


    template <class Function, class... Args>
    BOOST_FORCEINLINE
    constexpr decltype(auto) invoke_nonmember
        ( Function&& f
        , Args&&... args ) 
    BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
    (
        std::forward<Function>(f)(std::forward<Args>(args)... )
    )


    struct invoke_impl {
    private:
        template <class Function, class T, class Object,
            class = std::enable_if_t<std::is_member_object_pointer<
                Function(T::*)>::value> >
        BOOST_FORCEINLINE
        static constexpr decltype(auto) call_impl
            ( Function (T::*f)
            , Object&& obj )
        BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
        (
            invoke_member_data(f, std::forward<Object>(obj))
        )

        template <class Function, class T, class Object, class... Args,
            class = std::enable_if_t<std::is_member_function_pointer<
                Function(T::*)>::value> >
        BOOST_FORCEINLINE
        static constexpr decltype(auto) call_impl
            ( Function (T::*f)
            , Object&& obj
            , Args&&... args )
        BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
        (
            invoke_member_func(f, std::forward<Object>(obj),
                std::forward<Args>(args)...)
        )

        template <class Function, class... Args,
            class = std::enable_if_t<!std::is_member_pointer<
                std::decay_t<Function>>::value> >
        BOOST_FORCEINLINE
        static constexpr decltype(auto) call_impl
            ( Function&& f
            , Args&&... args )
        BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
        (
            invoke_nonmember(std::forward<Function>(f),
                std::forward<Args>(args)...)
        )

    public:

        template <class Function, class... Args>    
        BOOST_FORCEINLINE
        constexpr decltype(auto) operator()(Function&& f, Args&&... args) const
        BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
        (
            call_impl(std::forward<Function>(f),
                std::forward<Args>(args)...)
        )
    };

} // end namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::invoke_impl, invoke)

BOOST_STATIC_VIEWS_END_NAMESPACE


#endif // BOOST_STATIC_VIEWS_DETAIL_INVOKE_HPP
