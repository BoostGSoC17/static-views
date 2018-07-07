//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// \file sequence_traits.hpp
// \brief Implementats #boost::static_views::sequence_traits.

#ifndef BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP
#define BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP

#include "concepts.hpp"
#include "detail/utils.hpp"

#include <array>
#include <limits>
#include <tuple>

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

template <class Derived, class Sequence>
struct sequence_traits_default {

    using size_type       = unsigned; // std::size_t;
    using difference_type = int;      // std::ptrdiff_t;
    using index_type      = int;      // difference_type;

  private:
    template <class Dummy>
    struct traits {
        using derived  = Derived;
        using sequence = Sequence;
        using type     = sequence_traits_default<Derived, Sequence>;
    };

    /// \brief Returns whether U "is similar enough to" S, i.e. U is either equal
    /// to S or a const of volatile qualified version thereof.
    ///
    /// This is a hack to make sequence_traits work with const and volatile
    /// qualifiers.
    template <class U, class S>
    static constexpr auto is_similar_enough_to()
    {
        using U2 = std::remove_reference_t<U>;
        return Same<S,
                   U2> || Same<S const, U2> || Same<S volatile, U2> || Same<S const volatile, U2>;
    }

    // Derived::extent() is non-negative
    // clang-format off
    template <class U, class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(traits<Dummy>::derived::extent() >= 0
            && is_similar_enough_to<U, Sequence>())
    static constexpr auto size_impl(U& /*unused*/, int /*unused*/) noexcept
    // clang-format on
    {
        return static_cast<size_type>(Derived::extent());
    }

    // Sequence::size() exists
    // clang-format on
    template <class U, class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(
            HasSize<Sequence> && is_similar_enough_to<U, Sequence>())
    static constexpr auto size_impl(U& xs, ... /*unused*/) noexcept
    // clang-format off
    {
        static_assert(noexcept(xs.size()), "Throwing size() is a bad idea :)");
        return xs.size();
    }

    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(
            HasExtent<typename traits<Dummy>::sequence>)
    static constexpr auto extent_impl(int /*unused*/) noexcept
    // clang-format on
    {
        return Sequence::extent();
    }

    static constexpr auto extent_impl(...) noexcept
    {
        return dynamic_extent;
    }

  public:
    // clang-format off
    template <class IndexType, class S
        BOOST_STATIC_VIEWS_REQUIRES(
            HasIndexOperator<Sequence, IndexType>
                && is_similar_enough_to<S, Sequence>())
    static constexpr decltype(auto) at(S&& xs, IndexType i)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(std::forward<S>(xs)[i]))
    // clang-format on
    {
        return std::forward<S>(xs)[i];
    }

    // Only enable if size_impl is available
    template <class S>
    static constexpr auto size(S& xs) noexcept
        -> decltype(sequence_traits_default::size_impl(xs, int{}))
    {
        return sequence_traits_default::size_impl(xs, int{});
    }

    template <class Dummy = void>
    static constexpr auto extent() noexcept
        -> decltype(traits<Dummy>::type::extent_impl(int{}))
    {
        constexpr auto extent = sequence_traits_default::extent_impl(int{});
        static_assert(extent >= 0 || extent == dynamic_extent,
            "Extent of a Sequence cannot be negative.");
        return extent;
    }
};

#if defined(BOOST_STATIC_VIEWS_CONCEPTS)
// clang-format off
template <class T>
concept bool Sequence = requires(T& a, typename sequence_traits<T>::index_type i) {
    { sequence_traits<T>::extent() } noexcept -> std::ptrdiff_t;
    sequence_traits<T>::extent() >= 0
        || sequence_traits<T>::extent() == dynamic_extent;
    { sequence_traits<T>::size(a) } noexcept
        -> typename sequence_traits<T>::size_type;
    { sequence_traits<T>::at(a, i) }
        ->typename sequence_traits<T>::reference;
};

template <class T>
concept bool StaticSequence = requires() {
    Sequence<T>;
    sequence_traits<T>::extent() >= 0;
};
// clang-format on
#else
namespace detail {

template <class T>
using has_sequence_traits_specialisation_t =
    decltype(sequence_traits<T>{});

template <class T>
constexpr bool HasSequenceTraitsSpecialisation =
    is_detected<has_sequence_traits_specialisation_t, T>::value;

template <class T, class = void>
struct _SequenceTraitsHaveExtent : std::false_type {
};

template <class T>
struct _SequenceTraitsHaveExtent<T,
    std::enable_if_t<
        // First of all, make sure sequence_traits<T>::extent() exists
        // and returns a std::ptrdiff_t
        // P.S. No need to go through the detected_t path as
        // std::is_same is lazy. Yay! :)
        std::is_same<decltype(sequence_traits<T>::extent()),
            std::ptrdiff_t>::value
        // std::is_same<detected_t<sequence_traits_have_extent_t, T>,
        //     std::ptrdiff_t>::value
        // Next, make sure it is constexpr
        && (sequence_traits<T>::extent(), true)>>
    // Finally, check that the returned value makes sense.
    : std::conditional_t<sequence_traits<T>::extent() >= 0
                             || sequence_traits<T>::extent()
                                    == dynamic_extent,
          std::true_type, std::false_type> {
    // This is a hack
    using is_static =
        std::conditional_t<sequence_traits<T>::extent() >= 0,
            std::true_type, std::false_type>;
};

template <class T>
constexpr bool SequenceTraitsHaveExtent =
    _SequenceTraitsHaveExtent<T>::value;

template <class T, class = void>
struct _SequenceTraitsHaveSize : std::false_type {
};

template <class T>
struct _SequenceTraitsHaveSize<T,
    std::enable_if_t<std::is_same<decltype(sequence_traits<T>::size(
                                      std::declval<T const&>())),
        typename sequence_traits<T>::size_type>::value>>
    : std::true_type {
};

template <class T>
constexpr bool SequenceTraitsHaveSize =
    _SequenceTraitsHaveSize<T>::value;

template <class T, class = void>
struct _SequenceTraitsHaveAt : std::false_type {
};

template <class T>
struct _SequenceTraitsHaveAt<T,
    std::enable_if_t<std::is_same<
        decltype(sequence_traits<T>::at(std::declval<T&>(),
            std::declval<typename sequence_traits<T>::index_type>())),
        typename sequence_traits<T>::reference>::value>>
    : std::true_type {
};

template <class T>
constexpr bool SequenceTraitsHaveAt = _SequenceTraitsHaveAt<T>::value;

} // namespace detail

// clang-format off
template <class T>
constexpr bool Sequence =
    detail::HasSequenceTraitsSpecialisation<T>
    && detail::SequenceTraitsHaveExtent<T>
    && detail::SequenceTraitsHaveSize<T>
    && detail::SequenceTraitsHaveAt<T>;
// clang-format on

template <class T>
constexpr bool StaticSequence = Sequence<T>&& std::is_same<
    typename detail::_SequenceTraitsHaveExtent<T>::is_static,
    std::true_type>::value;
#endif

/// \brief Specialisation of #sequence_traits for C-arrays.
template <class T, std::size_t N>
struct sequence_traits<T[N]>
    : sequence_traits_default<sequence_traits<T[N]>, T[N]> {

    using value_type      = T;
    using reference       = std::add_lvalue_reference_t<value_type>;
    using index_type      = int;
    using size_type       = unsigned;
    using difference_type = index_type;

    static_assert(std::numeric_limits<index_type>::max() >= N,
        "Wow! That's one long array! Please, submit a bug report "
        "here " BOOST_STATIC_VIEWS_ISSUES_LINK);

    static constexpr auto extent() noexcept
    {
        return static_cast<std::ptrdiff_t>(N);
    }
};

/// \brief Specialisation of #sequence_traits for std::array.
template <class T, std::size_t N>
struct sequence_traits<std::array<T, N>>
    : sequence_traits_default<sequence_traits<std::array<T, N>>,
          std::array<T, N>> {

    using value_type      = T;
    using reference       = std::add_lvalue_reference_t<T>;
    using index_type      = int;
    using size_type       = unsigned;
    using difference_type = index_type;

    static_assert(std::numeric_limits<index_type>::max() >= N,
        "Wow! That's one long array! Please, submit a bug report "
        "here " BOOST_STATIC_VIEWS_ISSUES_LINK);

    template <class S>
    static constexpr decltype(auto) at(S& xs, index_type const i) noexcept
    {
        return xs.data()[i];
    }

    static constexpr auto extent() noexcept
    {
        return static_cast<std::ptrdiff_t>(N);
    }
};

template <class T, std::size_t N>
struct sequence_traits<std::array<T, N> const>
    : sequence_traits<std::array<T, N>> {
    using value_type      = std::add_const_t<T>;
    using reference       = std::add_lvalue_reference_t<T>;
};

template <class T, std::size_t N>
struct sequence_traits<std::array<T, N> volatile>
    : sequence_traits<std::array<T, N>> {
    using value_type      = std::add_volatile_t<T>;
    using reference       = std::add_lvalue_reference_t<T>;
};

template <class T, std::size_t N>
struct sequence_traits<std::array<T, N> const volatile>
    : sequence_traits<std::array<T, N>> {
    using value_type      = std::add_volatile_t<std::add_const_t<T>>;
    using reference       = std::add_lvalue_reference_t<T>;
};

namespace detail {

template <class Tuple, class = void>
struct sequence_traits_tuple;

template <>
struct sequence_traits_tuple<std::tuple<>> {
    static constexpr auto at(
        std::tuple<> xs, std::size_t i) noexcept -> nonesuch;
};

template <class T, class... Ts>
struct sequence_traits_tuple<std::tuple<T, Ts...>,
    std::enable_if_t<all(std::is_same<T, Ts>::value...)>> {

  public:
    using value_type      = T;
    using reference       = std::add_lvalue_reference_t<T>;

  private:
    using tuple_type = std::tuple<T, Ts...>;

    template <class U>
    static constexpr decltype(auto) dummy_get(U&& tpl) noexcept
    {
        return std::get<0>(std::forward<U>(tpl));
    }

    // This is just wonderful!
    template <class U>
    using get_type =
        std::decay_t<auto(U)->decltype(dummy_get(std::declval<U>()))>;

    template <class Tuple, class IndexType, std::size_t... Is>
    static constexpr decltype(auto) at_impl(
        Tuple&& xs, IndexType i, std::index_sequence<Is...> /*unused*/) noexcept
    {
        constexpr get_type<Tuple&&> getters[] = {&std::get<Is>...};
        return (*getters[i])(std::forward<Tuple>(xs));
    }

  public:
    // clang-format off
    template <class Tuple, class IndexType
        BOOST_STATIC_VIEWS_REQUIRES(
            Same<tuple_type, std::remove_cv_t<std::remove_reference_t<Tuple>>>)
    static constexpr decltype(auto) at(Tuple&& xs, IndexType i)
        noexcept
    // clang-format on
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

    static constexpr auto extent() noexcept
    {
        return static_cast<std::ptrdiff_t>(sizeof...(Ts));
    }
};

template <class... Ts>
struct sequence_traits<std::tuple<Ts...> const>
    : sequence_traits<std::tuple<Ts...>> {

    using value_type = std::add_const_t<
        typename sequence_traits<std::tuple<Ts...>>::value_type>;
    using reference = std::add_lvalue_reference_t<value_type>;
};

template <class... Ts>
struct sequence_traits<std::tuple<Ts...> volatile>
    : sequence_traits<std::tuple<Ts...>> {

    using value_type = std::add_volatile_t<
        typename sequence_traits<std::tuple<Ts...>>::value_type>;
    using reference = std::add_lvalue_reference_t<value_type>;
};

template <class... Ts>
struct sequence_traits<std::tuple<Ts...> const volatile>
    : sequence_traits<std::tuple<Ts...>> {

    using value_type = std::add_cv_t<
        typename sequence_traits<std::tuple<Ts...>>::value_type>;
    using reference = std::add_lvalue_reference_t<value_type>;
};

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_SEQUENCE_TRAITS_HPP
