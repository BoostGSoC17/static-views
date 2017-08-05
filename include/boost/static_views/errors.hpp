//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_ERRORS_HPP
#define BOOST_STATIC_VIEWS_ERRORS_HPP

#include "detail/config.hpp"
#include <exception>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \brief Exception that is thrown when an "index out of bounds
/// error" is encountered.
class out_of_bound : std::exception {
    char const* _msg;

  public:
    out_of_bound() noexcept : _msg{"Index out of bounds."} {}
    explicit out_of_bound(char const* msg) noexcept : _msg{msg} {}

    auto what() const noexcept -> char const* override
    {
        return _msg;
    }
};

/*
class invalid_range : std::exception {
    char const* _msg;
public:
    invalid_range() noexcept : _msg{ "Invalid range bounds." } {}
    invalid_range(char const* msg) noexcept : _msg{ msg } {}

    auto what() const noexcept -> char const* override
    { return _msg; }
};
*/

/// \brief Exception that is thrown when an insert into a full bucket
/// is attempted.
class full_bucket : std::exception {
    char const* _msg;

  public:
    full_bucket() noexcept : _msg{"Bucket is full."} {}
    explicit full_bucket(char const* msg) noexcept : _msg{msg} {}

    auto what() const noexcept -> char const* override
    {
        return _msg;
    }
};

namespace detail {
BOOST_STATIC_VIEWS_NORETURN
auto make_out_of_bound(char const* msg) -> void
{
    throw out_of_bound{msg};
}

/*
BOOST_STATIC_VIEWS_NORETURN
auto make_invalid_range(char const* msg) -> void
{
    throw invalid_range{msg};
}
*/

BOOST_STATIC_VIEWS_NORETURN
auto make_full_bucket(char const* msg) -> void
{
    throw full_bucket{msg};
}
} // end namespace detail

// clang-format off
void (*make_out_of_bound_error)(char const*) = &detail::make_out_of_bound;
// void (*make_invalid_range_error)(char const*) = &detail::make_invalid_range;
void (*make_full_bucket_error)(char const*) = &detail::make_full_bucket;
// clang-format on

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_ERRORS_HPP
