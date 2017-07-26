//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// \file sequence_traits.hpp
// \brief Implementats #boost::static_views::sequence_traits.

#ifndef BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP
#define BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP

#include <array>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/detail/utils.hpp>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \brief Proxy through which StaticViews functions access sequences.

/// \code{.cpp}
/// template <class Sequence>
/// struct sequence_traits {
///     using type = Sequence;
///
///     static constexpr std::size_t size() noexcept;
///
///     template <class S>
///     static constexpr reference at(S&& xs, std::size_t i) noexcept;
/// };
/// \endcode
template <class Sequence>
struct sequence_traits;

/// \cond
template <class T, std::size_t N>
struct sequence_traits<T[N]> {
    using type = T[N];

    static constexpr auto size() noexcept { return N; }

    template <class S>
    static constexpr decltype(auto) at(S&& xs, std::size_t i) noexcept
    {
        return std::forward<S>(xs)[i];
    }
};
/// \endcond

/// \cond
template <class T, std::size_t N>
struct sequence_traits<std::array<T, N>> {
    using type = std::array<T, N>;

    static constexpr auto size() noexcept { return N; }

    template <class S>
    static constexpr decltype(auto) at(S&& xs, std::size_t i) noexcept
    {
        return std::forward<S>(xs)[i];
    }
};
/// \endcond

namespace detail {
namespace concepts {
namespace sequence {

    /// \brief Trait that determines whether a specialisation of
    /// #sequence_traits for `T` exists.
    template <class T>
    using has_sequence_traits_spec_t =
        decltype(sequence_traits<T>{});

    /// \brief Trait that determines whether a specialisation of
    /// #sequence_traits for `T` exists _and_ has a `type` typedef.
    template <class T>
    using has_type_typedef_t = typename sequence_traits<T>::type;

    /// \brief Trait that determines whether a specialisation of
    /// #sequence_traits for `T` exists _and_ has a static member
    /// function `size()`.
    template <class T>
    using sequence_traits_size_t = decltype(sequence_traits<T>::size());

    /// \brief Trait that determines whether a specialisation of
    /// #sequence_traits for `T` exists _and_ has a static member
    /// function `at(T, size_t)` taking a `T` and `size_t` as
    /// arguments.
    template <class T>
    using sequence_traits_at_t = decltype(sequence_traits<T>::at(
        std::declval<T>(), std::declval<std::size_t>()));

    template <class T>
    constexpr auto check_specialisation() noexcept -> bool
    {
        return is_detected<has_sequence_traits_spec_t, T>::value;
    }

    template <class T>
    constexpr auto assert_specialisation() noexcept -> bool
    {
        constexpr bool x = check_specialisation<T>();
        static_assert(x,
            "There exists no specialisation of "
            "`boost::static_views::sequence_traits` for type `T`. "
            "Availability of it is, unfortunately, required "
            "by the Sequence concept.");
        return x;
    }

    template <class T>
    constexpr auto check_type_typedef() noexcept -> bool
    {
        return is_detected<has_type_typedef_t, T>::value;
    }

    template <class T>
    constexpr auto assert_type_typedef() noexcept -> bool
    {
        constexpr bool x = check_type_typedef<T>();
        static_assert(x,
            "`boost::static_views::sequence_traits<T>` has no "
            "`type` typedef. Availability of it is, unfortunately, "
            "required by the Sequence concept.");
        return x;
    }

    template <class T>
    constexpr auto check_static_size() noexcept -> bool
    {
        return std::is_convertible<
            detected_t<sequence_traits_size_t, T>,
            std::size_t>::value;
    }

    template <class T>
    constexpr auto assert_static_size() noexcept -> bool
    {
        constexpr bool x = check_static_size<T>();
        static_assert(x,
            "`boost::static_views::sequence_traits<T>` has no "
            "static constexpr member function `size()` with "
            "return type convertible to size_t. Availability of "
            "it is, unfortunately, required by the Sequence "
            "concept.");
        return x;
    }

    template <class T>
    constexpr auto check_static_at() noexcept -> bool
    {
        return is_detected<sequence_traits_at_t, T>::value;
    }

    template <class T>
    constexpr auto assert_static_at() noexcept -> bool
    {
        constexpr bool x = check_static_at<T>();
        static_assert(x,
            "`boost::static_views::sequence_traits<T>` has no "
            "static constexpr member function `at(T, size_t)` "
            "taking a sequence and an index as arguments. "
            "Availability of it is, unfortunately, required "
            "by the Sequence concept.");
        return x;
    }

} // namespace sequence


template <class T>
constexpr auto is_Sequence() noexcept -> bool
{
    return check(&sequence::check_specialisation<T>,
        &sequence::check_type_typedef<T>,
        &sequence::check_static_size<T>,
        &sequence::check_static_at<T>);
}

template <class T>
constexpr auto assert_Sequence() noexcept -> bool
{
    return check(&sequence::assert_specialisation<T>,
        &sequence::assert_type_typedef<T>,
        &sequence::assert_static_size<T>,
        &sequence::assert_static_at<T>);
}

} // namespace concepts
} // namespace detail

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP
