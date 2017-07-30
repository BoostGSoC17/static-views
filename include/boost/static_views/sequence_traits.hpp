//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// \file sequence_traits.hpp
// \brief Implementats #boost::static_views::sequence_traits.

#ifndef BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP
#define BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP

#include <array>

#include "detail/config.hpp"
#include "detail/utils.hpp"

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

    /// \brief Helper trait to determine whether a specialisation
    /// of #sequence_traits for `T` exists.
    template <class T>
    using has_sequence_traits_spec_t =
        decltype(sequence_traits<T>{});

    BOOST_STATIC_VIEWS_DEFINE_CHECK(
        Has_sequence_traits_specialisation, T,
        (is_detected<has_sequence_traits_spec_t, T>::value),
        "There exists no specialisation of "
        "`boost::static_views::sequence_traits` for type `T`. "
        "Availability of it is, unfortunately, required "
        "by the Sequence concept. Hence, the error.");

    /// \brief Helper trait to determine whether a specialisation
    /// of #sequence_traits for `T` has a `type` typedef.
    template <class T>
    using has_type_typedef_t = typename sequence_traits<T>::type;

    BOOST_STATIC_VIEWS_DEFINE_CHECK(
        Has_type_typedef, T,
        (is_detected<has_type_typedef_t, T>::value),
        "`boost::static_views::sequence_traits<T>` has no "
        "`type` typedef. Availability of it is, unfortunately, "
        "required by the Sequence concept. Hence, the error.");

    /// \brief Helper trait to determine whether a specialisation
    /// of #sequence_traits for `T` has a static member function
    /// `size()`.
    template <class T>
    using has_static_size_t = decltype(sequence_traits<T>::size());

    BOOST_STATIC_VIEWS_DEFINE_CHECK(
        Has_static_size, T,
        (is_detected<has_static_size_t, T>::value),
        "`boost::static_views::sequence_traits<T>` has no "
        "static member function named `size()`. Availability of "
        "it is, unfortunately, required by the Sequence concept. "
        "Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_correct_return_type_size, T,
        (std::is_convertible<detected_t<has_static_size_t, T>,
            std::size_t>::value),
        "Return type of "
        "`boost::static_views::sequence_traits<T>::size()` is "
        "not convertible to size_t. According to the "
        "Sequence concept, it should be. Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(
        Is_noexcept_size, T,
        (noexcept(sequence_traits<T>::size())),
        "`boost::static_views::sequence_traits<T>::size()` is "
        "not noexcept. It should be though, according to the "
        "Sequence concept. Hence, the error.");

    /// \brief Helper trait to determine whether a specialisation
    /// of #sequence_traits for `T` has a static member function
    /// `at(sequence_traits<T>::type, size_t)`.
    template <class T>
    using has_static_at_t = decltype(sequence_traits<T>::at(
        std::declval<typename sequence_traits<T>::type>(),
        std::declval<std::size_t>()));

    BOOST_STATIC_VIEWS_DEFINE_CHECK(
        Has_static_at, T,
        (is_detected<has_static_at_t, T>::value),
        "`boost::static_views::sequence_traits<T>` has no "
        "static member function "
        "`at(std::sequence_traits<T>::type, size_t)`. "
        "Availability of it is, unfortunately, required by "
        "the Sequence concept. Hence, the error.");

    // clang-format off
    using Sequence =
        and_<
            Has_sequence_traits_specialisation,
            all_<
                and_<
                    Has_type_typedef,
                    Has_static_at
                >,
                and_<
                    Has_static_size,
                    Has_correct_return_type_size,
                    Is_noexcept_size
                >
            >
        >;
    // clang-format on

} // namespace sequence

using sequence::Sequence;

} // namespace concepts
} // namespace detail

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP
