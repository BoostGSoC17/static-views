//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Implementation of boost::static_views::make_wrapper.


#ifndef BOOST_STATIC_VIEWS_DETAIL_WRAPPER_HPP
#define BOOST_STATIC_VIEWS_DETAIL_WRAPPER_HPP

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/detail/invoke.hpp>


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE


namespace detail {

    /// \brief Wraps an lvalue or an rvalue reference.    

    /// This is basically a `constexpr` version of `std::reference_wrapper`. The
    /// difference is that #detail::wrapper can wrap both lvalue and rvalue
    /// references. This is just syntactic sugar.
    ///
    /// \code{.cpp}
    /// template <class T>
    /// struct wrapper {
    ///     constexpr auto get() const& noexcept(whenever possible);
    ///     constexpr auto get() &      noexcept(whenever possible);
    ///     constexpr auto get() &&     noexcept(whenever possible);
    ///
    ///     template <class... Args>
    ///     constexpr auto operator() (Args&&...) const
    ///         noexcept(whenever possible);
    /// };
    /// \endcode
    ///
    /// Constructors are intentionally not listed. Use make_wrapper(T&&) to
    /// create wrappers.
    template <class T> struct wrapper;

    /// \cond
    template <class T>
    struct wrapper<T&> {
        using type = T;

        explicit BOOST_STATIC_VIEWS_CONSTEXPR wrapper(type& x) noexcept
            : _payload{ &x }
        {}

        BOOST_STATIC_VIEWS_CONSTEXPR wrapper(wrapper const& other) =
#       if BOOST_WORKAROUND(BOOST_GCC, BOOST_TESTED_AT(BOOST_GCC))
            default;
#       else
            // default;
            delete;
#       endif

        BOOST_STATIC_VIEWS_CONSTEXPR
        wrapper(wrapper&&) = default;

        BOOST_STATIC_VIEWS_CONSTEXPR
        wrapper& operator=(wrapper const& other) = delete;

        BOOST_STATIC_VIEWS_CONSTEXPR
        wrapper& operator=(wrapper&&) = default;

        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR auto get() const noexcept -> type&
        {
            return *_payload;
        }

        /*
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR operator type&() const noexcept
        {
            return get();
        }
        */

        template <class... Args>
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        decltype(auto) operator()(Args&&... args) const
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
                invoke(std::declval<T const&>(), std::declval<Args&&>()...)
            ))
        {
            return invoke(*_payload, std::forward<Args>(args)...);
        }

    private:
        T*  _payload;
    };
    /// \endcond

    /// \cond
    template <class T>
    struct wrapper<T&&> {
        using type = T;

        explicit BOOST_STATIC_VIEWS_CONSTEXPR wrapper(type&& x)
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                std::is_nothrow_move_constructible<type>::value)
            : _payload{ std::move(x) }
        {}

        BOOST_STATIC_VIEWS_CONSTEXPR wrapper(wrapper const&) = 
#       if BOOST_WORKAROUND(BOOST_GCC, BOOST_TESTED_AT(BOOST_GCC))
            default;
#       else
            // default;
            delete;
#       endif

        BOOST_STATIC_VIEWS_CONSTEXPR wrapper(wrapper&&) 
            = default;

        BOOST_STATIC_VIEWS_CONSTEXPR wrapper& operator=(wrapper const&) 
            = delete;

        BOOST_STATIC_VIEWS_CONSTEXPR wrapper& operator=(wrapper&&) 
            = default;

#if BOOST_WORKAROUND(BOOST_GCC, BOOST_TESTED_AT(BOOST_GCC))
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR auto get() const&
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                std::is_nothrow_copy_constructible<type>::value)
            -> type
        {
            return _payload;
        }
#else
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR auto get() &
            noexcept
            -> type&
        {
            return _payload;
        }

        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR auto get() const&
            noexcept
            -> type const&
        {
            return _payload;
        }
#endif

        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR auto get() &&
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                std::is_nothrow_move_constructible<T>::value)
            -> type
        {
            return std::move(_payload);
        }

        /*
        BOOST_STATIC_VIEWS_CONSTEXPR operator type() const noexcept
        {
            return get();
        }

        BOOST_STATIC_VIEWS_CONSTEXPR operator type() noexcept
        {
            return get();
        }
        */

        template <class... Args>
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        decltype(auto) operator()(Args&&... args) &
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
                invoke(std::declval<wrapper &>().get(),
                    std::declval<Args&&>()...)
            ))
        {
            return invoke(std::forward<wrapper>(*this).get(),
                std::forward<Args>(args)...);
        }

        template <class... Args>
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        decltype(auto) operator()(Args&&... args) const&
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
                invoke(std::declval<wrapper const&>().get(),
                    std::declval<Args&&>()...)
            ))
        {
            return invoke(std::forward<wrapper>(*this).get(),
                std::forward<Args>(args)...);
        }

        template <class... Args>
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        decltype(auto) operator()(Args&&... args) &&
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
                invoke(std::declval<wrapper &&>().get(),
                    std::declval<Args&&>()...)
            ))
        {
            return invoke(std::forward<wrapper>(*this).get(),
                std::forward<Args>(args)...);
        }

    private:
        T  _payload;
    };
    /// \endcond

} // end namespace detail


/// \brief Makes a wrapper around an rvalue or lvalue reference.

/// Creates a #detail::wrapper of `T&` or `T&&` depending on the type of `x`.
template <class T>
BOOST_FORCEINLINE
BOOST_STATIC_VIEWS_CONSTEXPR auto make_wrapper(T&& x)
BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
(
    detail::wrapper<decltype(x)>{ std::forward<T>(x) }
)


BOOST_STATIC_VIEWS_END_NAMESPACE


#endif // BOOST_STATIC_VIEWS_DETAIL_WRAPPER_HPP
