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
#include <tuple>
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
///       static constexpr auto size(T const&) noexcept ->std::size_t;
///
///       static constexpr auto extent() noexcept ->std::ptrdiff_t;
///
///       static constexpr auto at(T const&, std::size_t) noexcept ->const_reference;
///       static constexpr auto at(T&, std::size_t) noexcept ->reference;
///       static constexpr auto at(T&&, std::size_t) noexcept ->rvalue_reference;
///   };
///
///   } // namespace static_views
///   } // namespace boost
///
/// * Elements are accessed by calling
///   ``sequence_traits<T>::at(xs, i)`` with a (const) reference to
///   ``T`` ``xs`` and index ``i``. ``at()`` must furthermore be
///   ``static`` and ``noexcept``. You're by no means required to
///   create all three overloads of ``at()``, one is sufficient.
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
/// Currently, only arrays (``T[N]`` and ``std::array<T, N>``) and
/// homogeneous tuples (``std::tuple<Ts...>`` where :math:`\exists
/// \text{U}: \forall \text{T} \in \{\text{Ts}\dots\}: \text{T} =
/// \text{U}`) have specialisations of :cpp:class:`sequencce_traits`.
/// Specialising it for other types is really simple though.
/// :doc:`Here's <examples/support_span>` how one can add
/// support for ``std::span``:
///
/// .. literalinclude:: ../example/support_span.cpp
///    :lines: 10-37
///
///
/// \endverbatim
// clang-format on
template <class T>
struct sequence_traits;

/// \brief Special value of that indicates that the size of a sequence
/// is unknown at compile-time.
constexpr std::ptrdiff_t dynamic_extent = -1;

namespace concepts {
namespace sequence {

    /// \brief Helper trait to determine whether a specialisation
    /// of #sequence_traits for `T` exists.
    template <class T>
    using has_sequence_traits_spec_t = decltype(sequence_traits<T>{});

    BOOST_STATIC_VIEWS_DEFINE_CHECK(
        HasSequenceTraitsSpecialisation, T,
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

    BOOST_STATIC_VIEWS_DEFINE_CHECK(HasExtent, T,
        (is_detected<has_extent_t, T>::value),
        "`boost::static_views::sequence_traits<T>` has no "
        "static member function `extent()`. Availability of "
        "it is, unfortunately, required by the Sequence concept. "
        "Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(ExtentHasCorrectReturnType, T,
        (std::is_convertible<detected_t<has_extent_t, T>,
            std::ptrdiff_t>::value),
        "Return type of "
        "`boost::static_views::sequence_traits<T>::extent()` is "
        "not convertible to ptrdiff_t. According to the "
        "Sequence concept, it should be. Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(ExtentIsNoexcept, T,
        (noexcept(sequence_traits<T>::extent())),
        "`boost::static_views::sequence_traits<T>::extent()` is "
        "not noexcept. It should be though, according to the "
        "Sequence concept. Hence, the error.");

    struct ExtentIsConstexpr {
      private:
        template <class T, int = (T::extent(), 0)>
        static constexpr auto test_impl(int) -> std::true_type
        {
            return {};
        }
        
        template <class T>
        static constexpr auto test_impl(...) -> std::false_type
        {
            return {};
        }

      public:
        template <class T>
        static constexpr auto test() noexcept -> bool
        {
            return test_impl<T>(int{});
        }

        template <class T>
        static constexpr auto check() noexcept -> bool
        {
            constexpr auto x = test<T>();
            static_assert(x,
                "`boost::static_views::sequence_traits<T>::extent()` "
                "is not constexpr. It should be though for `T` to "
                "satisfy the Sequence concept. Hence, the error.");
            return x;
        }
    };

    BOOST_STATIC_VIEWS_DEFINE_CHECK(ExtentHasCorrectReturnValue, T,
        (sequence_traits<T>::extent() == dynamic_extent
            || sequence_traits<T>::extent() >= 0),
        "`boost::static_views::sequence_traits<T>::extent()` returns "
        "an invald extent. Either a non-negative number or "
        "`boost::static_views::dynamic_extent` should be returned. "
        "Hence, the error.");

    template <class T>
    using has_size_t =
        decltype(sequence_traits<T>::size(std::declval<T const&>()));

    BOOST_STATIC_VIEWS_DEFINE_CHECK(HasSize, T,
        (is_detected<has_size_t, T>::value),
        "`boost::static_views::sequence_traits<T>` has no "
        "static member function `size(T const&)`. Availability of "
        "it is, unfortunately, required by the Sequence concept. "
        "Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(SizeHasCorrectReturnType, T,
        (std::is_convertible<detected_t<has_size_t, T>,
            std::size_t>::value),
        "Return type of "
        "`boost::static_views::sequence_traits<T>::size(T const&)` "
        "is not convertible to size_t. According to the "
        "Sequence concept, it should be. Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(SizeIsNoexcept, T,
        (noexcept(
            sequence_traits<T>::size(std::declval<T const&>()))),
        "`boost::static_views::sequence_traits<T>::size(T const&)` "
        "is not noexcept. It should be though, according to the "
        "Sequence concept. Hence, the error.");

    template <class T>
    using has_at_t = decltype(sequence_traits<T>::at(
        std::declval<T>(), std::declval<std::size_t>()));

    BOOST_STATIC_VIEWS_DEFINE_CHECK(HasAt, T,
        (is_detected<has_at_t, T>::value),
        "`boost::static_views::sequence_traits<T>` has no "
        "static member function `at(T, size_t)`. "
        "Availability of it is, unfortunately, required by "
        "the Sequence concept. Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(AtIsNoexcept, T,
        (noexcept(sequence_traits<T>::at(
            std::declval<T>(), std::declval<std::size_t>()))),
        "`boost::static_views::sequence_traits<T>::at(T, size_t)` "
        "is not noexcept. It should be though, according to the "
        "Sequence concept. Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(HasFixedSize, T,
        (sequence_traits<T>::extent() >= 0),
        "`boost::static_views::sequence_traits<T>::extent() should "
        "return a non-negative number. It does not. Hence, the "
        "error.");

    // clang-format off
    using Sequence =
        and_<
            HasSequenceTraitsSpecialisation,
            all_<
                and_<
                    HasExtent,
                    ExtentHasCorrectReturnType,
                    ExtentIsNoexcept,
                    ExtentHasCorrectReturnValue
                >,
                and_<
                    HasAt,
                    AtIsNoexcept
                >,
                and_<
                    HasSize,
                    SizeHasCorrectReturnType,
                    SizeIsNoexcept
                >
            >
        >;

    using StaticSequence =
        and_<
            Sequence,
            HasFixedSize
        >;
    // clang-format on

} // namespace sequence

using sequence::Sequence;
using sequence::StaticSequence;

} // namespace concepts



template <class Derived, class T>
struct sequence_traits_default {

  private:
    template <class Dummy>
    struct Impl {
      private:
        template <class U>
        using call_size_t = decltype(std::declval<U const&>().size());

        template <class U>
        using call_size_impl_t =
            decltype(U::size_impl(std::declval<T const&>()));

      public:
        struct HasExtent {
          private:
            template <class U, int = (U::extent(), 0)>
            static constexpr auto has_extent(int) noexcept
                -> std::true_type
            {
                return {};
            }

            template <class U>
            static constexpr auto has_extent(...) noexcept
                -> std::false_type
            {
                return {};
            }

          public:
            template <class U>
            static constexpr auto test() noexcept -> bool
            {
                return has_extent<U>(int{});
            }
        };

        BOOST_STATIC_VIEWS_DEFINE_CHECK(ExtentIsNonNegative, U,
            (U::extent() >= 0), "");

        BOOST_STATIC_VIEWS_DEFINE_CHECK(HasSize, U,
            (detail::is_detected<call_size_t, U>::value), "");

        BOOST_STATIC_VIEWS_DEFINE_CHECK(HasSizeImpl, U,
            (detail::is_detected<call_size_impl_t, U>::value), "");
    };

  public: // This is really unfortunate

    // Derived::extent() exists and is non-negative
    template <class Dummy = void,
        class             = std::enable_if_t<
            concepts::and_<typename Impl<Dummy>::HasExtent,
                typename Impl<Dummy>::ExtentIsNonNegative>::
                template test<Derived>()>, class = void>
    static constexpr auto size_impl(T const& /*unused*/) noexcept
        -> std::size_t
    {
        static_assert(std::is_convertible<decltype(Derived::extent()),
                          std::ptrdiff_t>::value,
            "If you provide your own `extent()` function, do you "
            "mind making it return something convertible to "
            "`std::ptrdiff_t`?");
        return static_cast<std::size_t>(Derived::extent());
    }

    // Derived::extent() either doesn't exist or returns a negative
    // number, but T::size() does exist.
    template <class Dummy = void,
        class             = std::enable_if_t<
            !concepts::and_<typename Impl<Dummy>::HasExtent,
                typename Impl<Dummy>::ExtentIsNonNegative>::
                template test<Derived>()
            && Impl<Dummy>::HasSize::template test<T>()>>
    static constexpr auto size_impl(T const& xs) noexcept
        -> std::size_t
    {
        static_assert(std::is_convertible<
            decltype(std::declval<T const&>().size()),
                          std::size_t>::value,
            "If you provide your own `size()` function, do you "
            "mind making it return something convertible to "
            "`std::size_t`?");
        return xs.size();
    }

  public:
    template <class S,
        class = std::enable_if_t<std::is_same<T,
            std::remove_cv_t<std::remove_reference_t<S>>>::value>>
    static constexpr auto at(S&& xs, std::size_t i) noexcept
        -> decltype(std::forward<S>(xs)[i])
    {
        static_assert(
            noexcept(std::forward<S>(xs)[i]), "Why not noexcept?");
        return std::forward<S>(xs)[i];
    }

    // Only enable if size_impl is available
    template <class Dummy = void,
        class = std::enable_if_t<Impl<Dummy>::HasSizeImpl::
                template test<sequence_traits_default>()>>
    static constexpr auto size(T const& xs) noexcept
    {
        return sequence_traits_default::size_impl(xs);
    }

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return dynamic_extent;
    }
};

/// \brief Specialisation of #sequence_traits for C-arrays.
template <class T, std::size_t N>
struct sequence_traits<T[N]>
    : sequence_traits_default<sequence_traits<T[N]>, T[N]> {

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        constexpr std::size_t max_size = static_cast<std::size_t>(
            std::numeric_limits<std::ptrdiff_t>::max());
        static_assert(N <= max_size, "Overflow detected.");
        return static_cast<std::ptrdiff_t>(N);
    }
};

/// \brief Specialisation of #sequence_traits for std::array.
template <class T, std::size_t N>
struct sequence_traits<std::array<T, N>>
    : sequence_traits_default<sequence_traits<std::array<T, N>>,
          std::array<T, N>> {

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        constexpr std::size_t max_size = static_cast<std::size_t>(
            std::numeric_limits<std::ptrdiff_t>::max());
        static_assert(N <= max_size, "Overflow detected.");
        return static_cast<std::ptrdiff_t>(N);
    }
};


namespace detail {

template <class Tuple, class = void>
struct sequence_traits_tuple;

template <>
struct sequence_traits_tuple<std::tuple<>> {
    static constexpr auto at(
        std::tuple<> xs, std::size_t const i) noexcept -> nonesuch;
};

template <class T, class... Ts>
struct sequence_traits_tuple<std::tuple<T, Ts...>,
    std::enable_if_t<utils::all(std::is_same<T, Ts>::value...)>> {

  private:
    using tuple_type = std::tuple<T, Ts...>;
    using value_type = T;

    template <class U>
    static constexpr decltype(auto) dummy_get(U&& tpl) noexcept
    {
        return std::get<0>(std::forward<U>(tpl));
    }

    // This is just wonderful!
    template <class U>
    using get_type =
        std::decay_t<auto(U)->decltype(dummy_get(std::declval<U>()))>;

    template <class Tuple, std::size_t... Is>
    static constexpr decltype(auto) at_impl(
        Tuple&& xs, std::size_t const i,
        std::index_sequence<Is...> /*unused*/) noexcept
    {
        constexpr get_type<Tuple&&> getters[] = {&std::get<Is>...};
        return (*getters[i])(std::forward<Tuple>(xs));
    }

  public:
    template <class Tuple,
        class = std::enable_if_t<std::is_same<tuple_type,
            std::remove_cv_t<std::remove_reference_t<Tuple>>>::value>>
    static constexpr decltype(auto) at(
        Tuple&& xs, std::size_t const i) noexcept
    {
        return at_impl(std::forward<Tuple>(xs), i,
            std::make_index_sequence<sizeof...(Ts) + 1>{});
    }
};

} // namespace detail

template <class... Ts>
struct sequence_traits<std::tuple<Ts...>>
    : detail::sequence_traits_tuple<std::tuple<Ts...>>
    , sequence_traits_default<sequence_traits<std::tuple<Ts...>>,
          std::tuple<Ts...>> {

    using detail::sequence_traits_tuple<std::tuple<Ts...>>::at;

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        constexpr std::size_t max_size = static_cast<std::size_t>(
            std::numeric_limits<std::ptrdiff_t>::max());
        static_assert(sizeof...(Ts) <= max_size, "Overflow detected.");

        return static_cast<std::ptrdiff_t>(sizeof...(Ts));
    }
};

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP
