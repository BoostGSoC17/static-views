//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// \file sequence_traits.hpp
// \brief Implementats #boost::static_views::sequence_traits.

#ifndef BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP
#define BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP

#include "detail/config.hpp"
#include "detail/utils.hpp"
#include <array>
#include <limits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \brief A proxy through which functions access sequences.

// clang-format off
/// \verbatim embed:rst:leading-slashes
///
/// :ref:`As mentioned <sequence-concept>` earlier, sequences are
/// homogeneous collections with random random access to elements.
/// Sequences of length known at compile-time are called *static
/// sequences*. Sequences of unknown (at compile-time) length are,
/// obviously, called *dynamic sequences*. The distinction is made
/// using the property called *extent* which is similar to the
/// ``Extent`` template parameter in the |std::span proposal|_.
///
/// .. |std::span proposal| replace:: ``std::span`` proposal
/// .. _std::span proposal: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0122r4.pdf
///
/// :cpp:class:`sequence_traits` provides a uniform interface for
/// working with sequences. A general :cpp:class:`sequence_traits`
/// declaration may look like this:
///
/// .. code-block:: cpp
///
///   namespace boost {
///   namespace static_views {
///
///   template <class T>
///   struct sequence_traits {
///       static constexpr auto size(T const&)
///           noexcept ->std::size_t;
///
///       static constexpr auto extent()
///           noexcept ->std::ptrdiff_t;
///
///       static constexpr auto at(T&, std::size_t)
///           noexcept ->reference;
///
///       static constexpr auto at(T const&, std::size_t)
///           noexcept ->const_reference;
///   };
///
///   } // namespace static_views
///   } // namespace boost
///
/// To make a type ``T`` model
/// the :ref:`sequence <sequence-concept>` concept one only needs to
/// specialise :cpp:class:`sequence_traits` for ``T``.
///
/// * Elements are accessed by calling
///   ``sequence_traits<T>::at(xs, i)`` with a (const) reference to
///   ``T`` ``xs`` and index ``i``. ``at()`` must furthermore be
///   ``static`` and ``noexcept``.
///
/// * Extent is detemined by calling ``sequence_traits<T>::extent()``.
///   It should be ``static``, ``noexcept``, and have return type
///   convertible to ``std::ptrdiff_t``. If the size of ``T`` is known
///   at compile-time, ``extent()`` should return it. Otherwise, a
///   special value :cpp:var:`dynamic_extent` should be returned.
///
/// * Number of elements in a sequence ``xs`` is determined by calling
///   ``sequence_traits<T>::size(xs)``. ``size()`` must be ``static``,
///   ``noexcept``, and have return type convertible to
///   ``std::size_t``. Furthermore it it assumed that if
///   ``sequence_traits<T>::extent()`` returns a non-negative number,
///   calling ``size`` with any sequence of type ``T`` will return the
///   same result.
///
///
/// \endverbatim
// clang-format on
template <class T>
struct sequence_traits;

/// \brief Special value of that indicates that the size of a sequence
/// is unknown at compile-time.
constexpr std::ptrdiff_t dynamic_extent = -1;

template <class T>
struct sequence_traits_default {
    template <class S>
    static constexpr decltype(auto) at(S&& xs, std::size_t i) noexcept
    {
        static_assert(
            noexcept(std::forward<S>(xs)[i]), "why not noexcept?");
        return std::forward<S>(xs)[i];
    }

    static constexpr auto size(T const& xs) noexcept -> std::size_t
    {
        static_assert(noexcept(xs.size()), "why not noexcept?");
        return xs.size();
    }

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return dynamic_extent;
    }
};

/// \brief Specialisation of #sequence_traits for C-arrays.
template <class T, std::size_t N>
struct sequence_traits<T[N]> : sequence_traits_default<T[N]> {
  private:
    using type = T[N];

  public:
    using sequence_traits_default<T[N]>::at;

    static constexpr auto size(type const&) noexcept -> std::size_t
    {
        return N;
    }

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        static_assert(static_cast<std::size_t>(
                          std::numeric_limits<std::ptrdiff_t>::max())
                          >= N,
            "overflow");
        return static_cast<std::ptrdiff_t>(N);
    }
};

/// \brief Specialisation of #sequence_traits for std::array.
template <class T, std::size_t N>
struct sequence_traits<std::array<T, N>> {

    using sequence_traits_default<T[N]>::at;
    using sequence_traits_default<T[N]>::size;

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        static_assert(static_cast<std::size_t>(
                          std::numeric_limits<std::ptrdiff_t>::max())
                          >= N,
            "overflow");
        return static_cast<std::ptrdiff_t>(N);
    }
};

namespace concepts {
namespace sequence {

    /// \brief Helper trait to determine whether a specialisation
    /// of #sequence_traits for `T` exists.
    template <class T>
    using has_sequence_traits_spec_t = decltype(sequence_traits<T>{});

    BOOST_STATIC_VIEWS_DEFINE_CHECK(
        Has_sequence_traits_specialisation, T,
        (is_detected<has_sequence_traits_spec_t, T>::value),
        "There exists no specialisation of "
        "`boost::static_views::sequence_traits` for type `T`. "
        "Availability of it is, unfortunately, required "
        "by the Sequence concept. Hence, the error.");

    /*
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
    */

    template <class T>
    using has_extent_t = decltype(sequence_traits<T>::extent());

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_extent, T,
        (is_detected<has_extent_t, T>::value),
        "`boost::static_views::sequence_traits<T>` has no "
        "static member function `extent()`. Availability of "
        "it is, unfortunately, required by the Sequence concept. "
        "Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_correct_return_type_extent, T,
        (std::is_convertible<detected_t<has_extent_t, T>,
            std::ptrdiff_t>::value),
        "Return type of "
        "`boost::static_views::sequence_traits<T>::extent()` is "
        "not convertible to ptrdiff_t. According to the "
        "Sequence concept, it should be. Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_noexcept_extent, T,
        (noexcept(sequence_traits<T>::extent())),
        "`boost::static_views::sequence_traits<T>::size()` is "
        "not noexcept. It should be though, according to the "
        "Sequence concept. Hence, the error.");

    template <class T>
    using has_size_t =
        decltype(sequence_traits<T>::size(std::declval<T const&>()));

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_size, T,
        (is_detected<has_size_t, T>::value),
        "`boost::static_views::sequence_traits<T>` has no "
        "static member function `size(T const&)`. Availability of "
        "it is, unfortunately, required by the Sequence concept. "
        "Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_correct_return_type_size, T,
        (std::is_convertible<detected_t<has_size_t, T>,
            std::size_t>::value),
        "Return type of "
        "`boost::static_views::sequence_traits<T>::size(T const&)` "
        "is not convertible to size_t. According to the "
        "Sequence concept, it should be. Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_noexcept_size, T,
        (noexcept(
            sequence_traits<T>::size(std::declval<T const&>()))),
        "`boost::static_views::sequence_traits<T>::size(T const&)` "
        "is not noexcept. It should be though, according to the "
        "Sequence concept. Hence, the error.");

    template <class T>
    using has_at_t = decltype(sequence_traits<T>::at(
        std::declval<T>(), std::declval<std::size_t>()));

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_at, T,
        (is_detected<has_at_t, T>::value),
        "`boost::static_views::sequence_traits<T>` has no "
        "static member function `at(T, size_t)`. "
        "Availability of it is, unfortunately, required by "
        "the Sequence concept. Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_noexcept_at, T,
        (noexcept(sequence_traits<T>::at(
            std::declval<T>(), std::declval<std::size_t>()))),
        "`boost::static_views::sequence_traits<T>::at(T, size_t)` "
        "is not noexcept. It should be though, according to the "
        "Sequence concept. Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_fixed_size, T,
        (sequence_traits<T>::extent() >= 0),
        "`boost::static_views::sequence_traits<T>::extent() should "
        "return a non-negative number. It does not. Hence, the "
        "error.");

    // clang-format off
    using Sequence =
        and_<
            Has_sequence_traits_specialisation,
            all_<
                and_<
                    Has_extent,
                    Has_correct_return_type_extent,
                    Is_noexcept_extent
                >,
                and_<
                    Has_at,
                    Is_noexcept_at
                >,
                and_<
                    Has_size,
                    Has_correct_return_type_size,
                    Is_noexcept_size
                >
            >
        >;

    using StaticSequence =
        and_<
            Sequence,
            Has_fixed_size
        >;
    // clang-format on

} // namespace sequence

using sequence::Sequence;
using sequence::StaticSequence;

} // namespace concepts

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP
