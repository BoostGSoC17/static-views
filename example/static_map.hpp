//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_STATIC_MAP_HPP
#define BOOST_STATIC_VIEWS_STATIC_MAP_HPP

#include <type_traits>

#include "../include/boost/static_views/detail/config.hpp"
#include "../include/boost/static_views/detail/find_first.hpp"
#include "../include/boost/static_views/detail/invoke.hpp"
#include "../include/boost/static_views/detail/utils.hpp"
#include "../include/boost/static_views/hashed.hpp"

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

namespace static_map {

struct key_not_found_error : std::exception {
    auto what() const noexcept -> char const* override
    {
        return _msg;
    }

  private:
    static constexpr char const* _msg = "Key not found.";
};
constexpr char const* key_not_found_error::_msg;

namespace detail {

    template <class T>
    using not_equal_t =
        decltype(std::declval<T>() != std::declval<T>());

    // clang-format off
    template <class Char>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto length_c(Char const* const str) noexcept -> std::size_t
    // clang-format on
    {
        using boost::static_views::detail::is_detected;
        static_assert(
            std::is_nothrow_default_constructible<Char>::value,
            "`boost::static_views::static_map::detail::length_c<Char>"
            "` "
            "requires `Char` to be nothrow default-constructible.");
        static_assert(is_detected<not_equal_t, Char>::value,
            "`boost::static_views::static_map::detail::length_c<Char>"
            "` "
            "requires `Char` to define `operator!=`.");
        static_assert(
            noexcept(std::declval<Char>() != std::declval<Char>()),
            "`boost::static_views::static_map::detail::length_c<Char>"
            "` "
            "requires `Char`'s `operator!=` to be noexcept.");

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
            "`boost::static_views::static_map::detail::crc32_hash` "
            "requires `Char` to be an integral type.");
        constexpr std::uint32_t INITXOR  = 0xFFFFFFFF;
        constexpr std::uint32_t FINALXOR = 0xFFFFFFFF;
        constexpr std::uint32_t CRCPOLY  = 0xEDB88320;

        auto const l      = length_c(str);
        auto       crcreg = INITXOR;

        for (std::size_t j = 0; j < l; ++j) {
            auto b = static_cast<std::make_unsigned_t<Char>>(str[j]);
            for (std::size_t i = 0; i < 8 * sizeof(Char); ++i) {
                if ((crcreg ^ b) & 1) {
                    crcreg = (crcreg >> 1) ^ CRCPOLY;
                }
                else {
                    crcreg >>= 1;
                }
                b >>= 1;
            }
        }

        return crcreg ^ FINALXOR;
    }

    // clang-format off
    template <class Char>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto simple_hash(Char const* const str) noexcept
    // clang-format on
    {
        using boost::static_views::detail::is_detected;
        static_assert(
            std::is_nothrow_default_constructible<Char>::value,
            "`boost::static_views::static_map::detail::simple_hash<"
            "Char>` requires `Char` to be nothrow "
            "default-constructible.");
        static_assert(is_detected<not_equal_t, Char>::value,
            "`boost::static_views::static_map::detail::simple_hash<"
            "Char>` requires `Char` to define `operator!=`.");
        static_assert(
            noexcept(std::declval<Char>() != std::declval<Char>()),
            "`boost::static_views::static_map::detail::simple_hash<"
            "Char>` requires `Char`'s `operator!=` to be noexcept.");
        std::size_t hash = 0;
        std::size_t i    = 0;
        while (str[i] != Char{}) {
            hash = 37 * hash
                   + static_cast<std::make_unsigned_t<Char>>(str[i]);
            ++i;
        }
        return hash;
    }

    template <class T, class = void>
    struct hash_impl;

    template <class T>
    struct hash_impl<T,
        std::enable_if_t<std::is_integral<T>::value>> {

        BOOST_STATIC_VIEWS_CONSTEXPR
        auto operator()(T const x) const noexcept -> std::size_t
        {
            return static_cast<std::size_t>(x);
        }
    };

    template <class Char>
    struct hash_impl<Char const*,
        std::enable_if_t<std::is_integral<Char>::value>> {

        BOOST_STATIC_VIEWS_CONSTEXPR
        auto operator()(Char const* const x) const noexcept
            -> std::size_t
        {
            return simple_hash(x);
            // return static_cast<std::size_t>(crc32_hash(x));
        }
    };

#if defined(BOOST_STATIC_VIEWS_HAVE_STRING_VIEW)
    template <class Char>
    struct hash_impl<basic_string_view<Char>> {

        BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(
            basic_string_view<Char> const x) const noexcept
            -> std::size_t
        {
            return hash_impl<Char const*>{}(x.data());
        }
    };
#endif

} // namespace detail

struct hash_c {
    template <class T>
    BOOST_STATIC_VIEWS_FORCEINLINE BOOST_STATIC_VIEWS_CONSTEXPR auto
                                   operator()(T&& x) const BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
        detail::hash_impl<std::decay_t<T>>{}(std::forward<T>(x))))
        -> std::size_t
    {
        return detail::hash_impl<std::decay_t<T>>{}(
            std::forward<T>(x));
    }
};

namespace detail {

    template <class Pred, class GetKey, class GetMapped>
    class map_config : private std::tuple<Pred, GetKey, GetMapped> {

      private:
        static_assert(
            std::is_same<Pred, std::decay_t<Pred>>::value, "");
        static_assert(
            std::is_same<GetKey, std::decay_t<GetKey>>::value, "");
        static_assert(
            std::is_same<GetMapped, std::decay_t<GetMapped>>::value,
            "");

        using tuple_type = std::tuple<Pred, GetKey, GetMapped>;

      public:
        using key_equal     = typename Pred::type;
        using key_getter    = typename GetKey::type;
        using mapped_getter = typename GetMapped::type;

        BOOST_STATIC_VIEWS_CONSTEXPR
        explicit map_config(tuple_type&& tpl)
#if defined(BOOST_STATIC_VIEWS_NEGLECT_STD_TUPLE)
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                ::BOOST_STATIC_VIEWS_NAMESPACE::detail::utils::all(
                    std::is_nothrow_move_constructible<Pred>::value,
                    std::is_nothrow_move_constructible<GetKey>::value,
                    std::is_nothrow_move_constructible<
                        GetMapped>::value))
#else
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                std::is_nothrow_move_constructible<tuple_type>::value)
#endif
            : tuple_type{std::move(tpl)}
        {
        }

        BOOST_STATIC_VIEWS_CONSTEXPR
        decltype(auto) equal() const & noexcept
        {
            return std::get<0>(static_cast<tuple_type const&>(*this))
                .get();
        }

        BOOST_STATIC_VIEWS_CONSTEXPR
        decltype(auto) equal() && noexcept
        {
            return std::get<0>(static_cast<tuple_type&&>(*this))
                .get();
        }

        BOOST_STATIC_VIEWS_CONSTEXPR
        decltype(auto) key() const & noexcept
        {
            return std::get<1>(static_cast<tuple_type const&>(*this))
                .get();
        }

        BOOST_STATIC_VIEWS_CONSTEXPR
        decltype(auto) key() && noexcept
        {
            return std::get<1>(static_cast<tuple_type&&>(*this))
                .get();
        }

        BOOST_STATIC_VIEWS_CONSTEXPR
        decltype(auto) mapped() const & noexcept
        {
            return std::get<2>(static_cast<tuple_type const&>(*this))
                .get();
        }

        BOOST_STATIC_VIEWS_CONSTEXPR
        decltype(auto) mapped() && noexcept
        {
            return std::get<2>(static_cast<tuple_type&&>(*this))
                .get();
        }
    };

    template <class Pred, class GetKey, class GetMapped>
    BOOST_STATIC_VIEWS_CONSTEXPR auto make_map_config_impl(
        std::tuple<Pred, GetKey, GetMapped>&& tpl)
    {
        return map_config<Pred, GetKey, GetMapped>{std::move(tpl)};
    }

    template <class Pred, class GetKey, class GetMapped>
    BOOST_STATIC_VIEWS_CONSTEXPR auto make_map_config(
        Pred&& p, GetKey&& k, GetMapped&& m)
    {
        return make_map_config_impl(
            std::make_tuple(make_wrapper(std::forward<Pred>(p)),
                make_wrapper(std::forward<GetKey>(k)),
                make_wrapper(std::forward<GetMapped>(m))));
    }

    template <class HashedView, class MapConfig>
    class static_map
        : private HashedView
        , private MapConfig {

      public:
        using type        = static_map<HashedView, MapConfig>;
        using bucket_type = std::decay_t<decltype(
            std::declval<HashedView>()[std::declval<std::size_t>()])>;
        using value_type =
            std::remove_reference_t<decltype(std::declval<
                bucket_type>()[std::declval<std::size_t>()])>;
        using key_equal     = typename MapConfig::key_equal;
        using key_getter    = typename MapConfig::key_getter;
        using mapped_getter = typename MapConfig::mapped_getter;

      private:
        template <class T>
        using callable_with_value_t = decltype(invoke(
            std::declval<T const&>(), std::declval<value_type>()));

        static_assert(concepts::is_detected<callable_with_value_t,
                          key_getter>::value,
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        static_assert(concepts::is_detected<callable_with_value_t,
                          mapped_getter>::value,
            BOOST_STATIC_VIEWS_BUG_MESSAGE);

      public:
        using key_type    = std::remove_reference_t<decltype(
            invoke(std::declval<key_getter const&>(),
                std::declval<value_type&>()))>;
        using mapped_type = std::remove_reference_t<decltype(
            invoke(std::declval<mapped_getter const&>(),
                std::declval<value_type&>()))>;

      private:
        template <class T>
        using callable_with_key_t =
            decltype(invoke(std::declval<T const&>(),
                std::declval<key_type const&>()));

        template <class T>
        using callable_with_2_key_t = decltype(invoke(
            std::declval<T const&>(), std::declval<key_type const&>(),
            std::declval<key_type const&>()));

        static_assert(concepts::is_detected<callable_with_2_key_t,
                          key_equal>::value,
            BOOST_STATIC_VIEWS_BUG_MESSAGE);

      public:
        // To be improved
        using hasher =
            std::decay_t<decltype(std::declval<HashedView const&>()
                                      .hash_function()
                                      ._hasher)>;
        static_assert(
            std::is_convertible<
                concepts::detected_t<callable_with_key_t, hasher>,
                std::size_t>::value,
            BOOST_STATIC_VIEWS_BUG_MESSAGE);

        using difference_type = std::ptrdiff_t;
        using size_type       = std::size_t;

      private:
        using view_type   = HashedView;
        using config_type = MapConfig;

        BOOST_STATIC_VIEWS_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        BOOST_STATIC_VIEWS_DECLTYPE_AUTO _hash_function() const
            noexcept
        {
            // To be improved
            return this->hash_function()._hasher;
        }

        BOOST_STATIC_VIEWS_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        auto _lookup(key_type const& k) const -> value_type const*
        {
            struct pred_equal {
                key_getter const& get_key;
                key_equal const&  equal;
                key_type const&   key;

                BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(
                    value_type const& y) -> bool
                {
                    return invoke(equal, key, invoke(get_key, y));
                }
            };

            auto const hash = view_type::bucket_size()
                              * (invoke(_hash_function(), k)
                                    % view_type::bucket_count());
            return this->lookup(
                hash, pred_equal{this->key(), this->equal(), k});
            /*
            // get all elements matching key k
            auto const                               ys =
                static_cast<view_type const*>(this)->operator[](
                    boost::static_views::invoke(_hash_function(), k));
            // search for the one that compares equal to k
            auto const i = boost::static_views::find_first_i(
                ys, pred_equal{this->key(), this->equal(), k});

            return (i < ys.size()) ? &(ys.unsafe_at(i)) : nullptr;
            */
        }

      public:
        BOOST_STATIC_VIEWS_CONSTEXPR
        static_map(view_type&& xs, config_type&& conf)
            : view_type{std::move(xs)}, config_type{std::move(conf)}
        {
        }

        BOOST_STATIC_VIEWS_CONSTEXPR
        auto size() const -> size_type
        {
            return static_cast<view_type const*>(this)->size();
        }

        BOOST_STATIC_VIEWS_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        auto find(key_type const& k) const -> value_type*
        {
            return _lookup(k);
        }

        BOOST_STATIC_VIEWS_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        auto count(key_type const& k) const -> size_type
        {
            return _lookup(k) == nullptr ? 0 : 1;
        }

        BOOST_STATIC_VIEWS_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        decltype(auto) at(key_type const& k) const
        {
            auto* x = _lookup(k);
            if (x == nullptr) {
                throw key_not_found_error{};
            }
            return invoke(this->mapped(), *x);
        }

        BOOST_STATIC_VIEWS_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        decltype(auto) operator[](key_type const& k) const
        {
            return at(k);
        }
    };

} // namespace detail

// clang-format off
template <std::size_t BucketCount, std::size_t BucketSize, class View,
    class GetKey, class GetMapped,
    class KeyEqual = std::equal_to<void>, class Hasher = hash_c>
BOOST_STATIC_VIEWS_CONSTEXPR
auto make_static_map(View&& xs, GetKey&& get_key, GetMapped&& get_mapped,
    KeyEqual&& key_equal = KeyEqual{}, Hasher&& hasher = Hasher{})
{
    using value_type = std::remove_reference_t<decltype(
        xs[std::declval<std::size_t>()])>;

    struct tricky_hasher {
        Hasher _hasher;
        GetKey _get_key;

        BOOST_STATIC_VIEWS_CONSTEXPR
        auto operator()(value_type const& x) const -> std::size_t
        {
            return invoke(_hasher, invoke(_get_key, x));
        }
    };

    auto view = hashed<BucketCount, BucketSize>(
        tricky_hasher{std::forward<Hasher>(hasher), get_key})(
        std::forward<View>(xs));

    auto conf = detail::make_map_config(std::forward<KeyEqual>(key_equal),
        std::forward<GetKey>(get_key), std::forward<GetMapped>(get_mapped));

    return detail::static_map<decltype(view), decltype(conf)>{
            std::move(view), std::move(conf)};
}
// clang-format on

} // namespace static_map

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_STATIC_MAP_HPP
