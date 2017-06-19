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

    
    template <class T> struct wrapper;

    /// \cond
    template <class T>
    struct wrapper<T&> {
        using type = T;

        explicit constexpr wrapper(T& x) noexcept
            : _payload{ &x }
        {}

        constexpr wrapper(wrapper const& other)            = default;
        constexpr wrapper(wrapper&&)                       = default;
        constexpr wrapper& operator=(wrapper const& other) = default;
        constexpr wrapper& operator=(wrapper&&)            = default;

        constexpr auto get() const noexcept -> T& { return *_payload; }
        constexpr operator T&() const noexcept { return get(); }

        template <class... Args>
        constexpr decltype(auto) operator()(Args&&... args) const
            noexcept(noexcept(
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

        constexpr wrapper(T&& x)
            noexcept(std::is_nothrow_move_constructible<T>::value)
            : _payload{ std::move(x) }
        {}

        constexpr wrapper(wrapper const&)            = default;
        constexpr wrapper(wrapper&&)                 = default;
        constexpr wrapper& operator=(wrapper const&) = default;
        constexpr wrapper& operator=(wrapper&&)      = default;

#if BOOST_WORKAROUND(BOOST_GCC, BOOST_TESTED_AT(BOOST_GCC))
        constexpr auto get() const&
            noexcept(std::is_nothrow_copy_constructible<T>::value)
            -> T
        {
            return _payload;
        }
#else
        constexpr auto get() &
            noexcept
            -> T&
        {
            return _payload;
        }

        constexpr auto get() const&
            noexcept(std::is_nothrow_copy_constructible<T>::value)
            -> T const&
        {
            return _payload;
        }
#endif


        constexpr auto get() &&
            noexcept(std::is_nothrow_move_constructible<T>::value)
            -> T
        {
            return std::move(_payload);
        }

        constexpr operator T() const noexcept { return get(); }
        constexpr operator T()       noexcept { return get(); }

        template <class... Args>
        constexpr decltype(auto) operator()(Args&&... args) &
            noexcept(noexcept(
                invoke(std::declval<wrapper &>().get(),
                    std::declval<Args&&>()...)
            ))
        {
            return invoke(get(), std::forward<Args>(args)...);
        }

        template <class... Args>
        constexpr decltype(auto) operator()(Args&&... args) const&
            noexcept(noexcept(
                invoke(std::declval<wrapper const&>().get(),
                    std::declval<Args&&>()...)
            ))
        {
            return invoke(get(), std::forward<Args>(args)...);
        }

        template <class... Args>
        constexpr decltype(auto) operator()(Args&&... args) &&
            noexcept(noexcept(
                invoke(std::declval<wrapper &&>().get(),
                    std::declval<Args&&>()...)
            ))
        {
            return invoke(get(), std::forward<Args>(args)...);
        }

    private:
        T  _payload;
    };
    /// \endcond

} // end namespace detail


template <class T>
constexpr auto make_wrapper(T&& x)
BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
(
    detail::wrapper<decltype(x)>{ std::forward<T>(x) }
)


BOOST_STATIC_VIEWS_END_NAMESPACE


#endif // BOOST_STATIC_VIEWS_DETAIL_WRAPPER_HPP
