//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_ERRORS_HPP
#define BOOST_STATIC_VIEWS_ERRORS_HPP


#include <exception>

#include <boost/static_views/detail/config.hpp>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE


struct out_of_bound : std::exception {

private:
    char const* _msg;
public:

    out_of_bound() noexcept
        : _msg{ "Index out of bounds." }
    {
    }

    out_of_bound(char const* msg) noexcept
        : _msg{ msg }
    {
    }

    auto what() const noexcept -> char const* override
    { return _msg; }
};

namespace detail {
    auto make_out_of_bound(char const* msg)
    {
        throw out_of_bound(msg);
    }
} // end namespace detail

void (*make_out_of_bound_error)(char const*) = &detail::make_out_of_bound;



BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_ERRORS_HPP
