//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_STATIC_VIEWS_DETAIL_CONFIG_HPP
#define BOOST_STATIC_VIEWS_DETAIL_CONFIG_HPP


#define BOOST_STATIC_VIEWS_NAMESPACE boost::static_views

#define BOOST_STATIC_VIEWS_BEGIN_NAMESPACE \
    namespace boost { namespace static_views {

#define BOOST_STATIC_VIEWS_END_NAMESPACE \
    } /* static_views */ } /* boost */


// If a function body is just a return-statement, we can deduce the
// noexcept-ness automatically. This idea taken from range-v3.
#define BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT(...)                          \
    noexcept(noexcept(decltype(__VA_ARGS__)(__VA_ARGS__)))                    \
        { return (__VA_ARGS__); }                                             \
    /**/


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

// See http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html
template <class T>
constexpr T _static_const{};

BOOST_STATIC_VIEWS_END_NAMESPACE

#define BOOST_STATIC_VIEWS_INLINE_VARIABLE(type, name)                        \
    inline namespace {                                                        \
        constexpr auto const& name =                                          \
            ::BOOST_STATIC_VIEWS_NAMESPACE::_static_const<type>;              \
    }                                                                         \
    /**/


#endif // BOOST_STATIC_VIEWS_DETAIL_CONFIG_HPP
