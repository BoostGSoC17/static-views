//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_HASH_C_HPP
#define BOOST_STATIC_VIEWS_HASH_C_HPP

#include <type_traits>

#include "concepts.hpp"
#include "detail/config.hpp"

// clang-format off
#if __cplusplus > 201402L
#if defined(__has_include)
#   if __has_include(<string_view>)
#       include <string_view>
#       define BOOST_STATIC_VIEWS_HAVE_STRING_VIEW
        BOOST_STATIC_VIEWS_BEGIN_NAMESPACE
        template <class CharT, class Traits = std::char_traits<CharT>>
        using basic_string_view =
            std::basic_string_view<CharT, Traits>;
        using std::string_view;
        using std::u16string_view;
        using std::u32string_view;
        using std::wstring_view;
        BOOST_STATIC_VIEWS_END_NAMESPACE
#   elif __has_include(<experimental/string_view>)
#       include <experimental/string_view>
#       define BOOST_STATIC_VIEWS_HAVE_STRING_VIEW
        BOOST_STATIC_VIEWS_BEGIN_NAMESPACE
        template <class CharT, class Traits = std::char_traits<CharT>>
        using basic_string_view =
            std::experimental::basic_string_view<CharT, Traits>;
        using std::experimental::string_view;
        using std::experimental::u16string_view;
        using std::experimental::u32string_view;
        using std::experimental::wstring_view;
        BOOST_STATIC_VIEWS_END_NAMESPACE
#   endif
#endif
#endif
// clang-format on

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
template <class T>
using not_equal_t = decltype(std::declval<T>() != std::declval<T>());
} // namespace detail

// clang-format off
template <class Char>
BOOST_STATIC_VIEWS_FORCEINLINE
BOOST_STATIC_VIEWS_CONSTEXPR
auto strlen_c(Char const* const str) noexcept -> std::size_t
// clang-format on
{
    using boost::static_views::is_detected;
    static_assert(std::is_nothrow_default_constructible<Char>::value,
        "`boost::static_views::strlen_c<Char>` requires `Char` to be nothrow "
        "default-constructible.");
    static_assert(is_detected<detail::not_equal_t, Char>::value,
        "`boost::static_views::strlen_c<Char>` needs an `operator!=` for "
        "`Char`.");
    static_assert(noexcept(std::declval<Char>() != std::declval<Char>()),
        "`boost::static_views::strlen_c<Char>` assumes that `Char`'s "
        "`operator!=` is noexcept.");

    std::size_t i = 0;
    while (str[i] != Char{}) {
        ++i;
    }
    return (i == 0) ? 0 : (i - 1);
}

// This algorithm is "stolen" from the Internet.
// clang-format off
template <class Char>
BOOST_STATIC_VIEWS_FORCEINLINE
BOOST_STATIC_VIEWS_CONSTEXPR
auto crc32_hash(Char const* const str) noexcept -> std::uint32_t
// clang-format on
{
    static_assert(std::is_integral<Char>::value,
        "`boost::static_views::crc32_hash` requires `Char` to be an integral "
        "type.");
    constexpr std::uint32_t INITXOR  = 0xFFFFFFFF;
    constexpr std::uint32_t FINALXOR = 0xFFFFFFFF;
    constexpr std::uint32_t CRCPOLY  = 0xEDB88320;

    auto const l      = length_c(str);
    auto       crcreg = INITXOR;

    for (std::size_t j = 0; j < l; ++j) {
        auto b = static_cast<std::make_unsigned_t<Char>>(str[j]);
        for (std::size_t i = 0; i < 8 * sizeof(Char); ++i) {
            if ((crcreg ^ b) & 1)
                crcreg = (crcreg >> 1) ^ CRCPOLY;
            else
                crcreg >>= 1;
            b >>= 1;
        }
    }

    return crcreg ^ FINALXOR;
}

namespace detail {
// clang-format off
template <class Char>
BOOST_STATIC_VIEWS_FORCEINLINE
BOOST_STATIC_VIEWS_CONSTEXPR
auto simple_hash(Char const* const str) noexcept
// clang-format on
{
    using boost::static_views::is_detected;
    static_assert(std::is_nothrow_default_constructible<Char>::value,
        "`boost::static_views::detail::simple_hash<Char>` requires `Char` to "
        "be nothrow default-constructible.");
    static_assert(is_detected<detail::not_equal_t, Char>::value,
        "`boost::static_views::detail::simple_hash<Char>` needs an "
        "`operator!=` for `Char`.");
    static_assert(noexcept(std::declval<Char>() != std::declval<Char>()),
        "`boost::static_views::detail::simple_hash<Char>` assumes that "
        "`Char`'s `operator!=` is noexcept.");
    std::size_t hash = 0;
    std::size_t i    = 0;
    while (str[i] != Char{}) {
        hash = 37 * hash + static_cast<std::make_unsigned_t<Char>>(str[i]);
        ++i;
    }
    return hash;
}
} // namespace detail


template <class T, class = void>
struct hash_impl;

template <class T>
struct hash_impl<T, std::enable_if_t<std::is_integral<T>::value>> {
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(T const x) const noexcept -> std::size_t
    {
        return static_cast<std::size_t>(x);
    }
};

template <class Char>
struct hash_impl<Char const*, std::enable_if_t<std::is_integral<Char>::value>> {
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Char const* const x) const noexcept -> std::size_t
    {
        return simple_hash(x);
        // return static_cast<std::size_t>(crc32_hash(x));
    }
};

#if defined(BOOST_STATIC_VIEWS_HAVE_STRING_VIEW)
template <class Char>
struct hash_impl<basic_string_view<Char>> {
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(basic_string_view<Char> const x) const noexcept
        -> std::size_t
    {
        return hash_impl<Char const*>{}(x.data());
    }
};
#endif

struct hash_c {
    // clang-format off
    template <class T>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(T&& x) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(hash_impl<std::remove_cv_t<std::remove_reference_t<T>>>{}(
                std::forward<T>(x)))) -> std::size_t
    {
        return hash_impl<std::decay_t<T>>{}(std::forward<T>(x));
    }
};

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_HASH_C_HPP
