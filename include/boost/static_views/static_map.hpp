//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_STATIC_MAP_HPP
#define BOOST_STATIC_VIEWS_STATIC_MAP_HPP

#include <exception>
#include <type_traits>

#include "compose.hpp"
#include "hash_c.hpp"
#include "hashed.hpp"

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

struct key_not_found_error : public virtual std::exception {
    auto what() const noexcept -> char const* override { return _msg; }

  private:
    static constexpr char const* _msg = "Key not found.";
};
constexpr char const* key_not_found_error::_msg;

namespace detail {

template <class Pred, class GetKey, class GetMapped>
class map_config
    // TODO: This will probably break if get_key and get_mapped functions have
    // the same type...
    : private Pred
    , private GetKey
    , private GetMapped {

  public:
    using key_equal     = typename Pred::value_type;
    using key_getter    = typename GetKey::value_type;
    using mapped_getter = typename GetMapped::value_type;

    BOOST_STATIC_VIEWS_CONSTEXPR
    map_config(Pred&& equal, GetKey&& get_key, GetMapped&& get_mapped)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<Pred>::value&&
                    std::is_nothrow_move_constructible<GetKey>::value&&
                    std::is_nothrow_move_constructible<GetMapped>::value)
        : Pred{std::move(equal)}
        , GetKey{std::move(get_key)}
        , GetMapped{std::move(get_mapped)}
    {
    }

    map_config(map_config const&) = default;
    map_config(map_config&&)      = default;
    map_config& operator=(map_config const&) = default;
    map_config& operator=(map_config&&) = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto const& equal() const& noexcept
    {
        return static_cast<Pred const&>(*this).get();
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto equal()
        && BOOST_STATIC_VIEWS_NOEXCEPT_IF(
               std::is_nothrow_move_constructible<key_equal>::value)
    {
        return static_cast<Pred&&>(*this).get();
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto const& key() const& noexcept
    {
        return static_cast<GetKey const&>(*this).get();
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto key()
        && BOOST_STATIC_VIEWS_NOEXCEPT_IF(
               std::is_nothrow_move_constructible<key_getter>::value)
    {
        return static_cast<GetKey&&>(*this).get();
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto const& mapped() const& noexcept
    {
        return static_cast<GetMapped const&>(*this).get();
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto mapped()
        && BOOST_STATIC_VIEWS_NOEXCEPT_IF(
               std::is_nothrow_move_constructible<mapped_getter>::value)
    {
        return static_cast<GetMapped&&>(*this).get();
    }
};

// clang-format off
template <class Pred, class GetKey, class GetMapped>
BOOST_STATIC_VIEWS_FORCEINLINE
BOOST_STATIC_VIEWS_CONSTEXPR
auto make_map_config_impl(Pred p, GetKey k, GetMapped m)
    BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN
(
    map_config<Pred, GetKey, GetMapped>{
        std::move(p), std::move(k), std::move(m)}
);
// clang-format on

// clang-format off
template <class Pred, class GetKey, class GetMapped>
BOOST_STATIC_VIEWS_FORCEINLINE
BOOST_STATIC_VIEWS_CONSTEXPR
auto make_map_config(Pred&& p, GetKey&& k, GetMapped&& m)
    BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN
(
    make_map_config_impl(
        make_wrapper(std::forward<Pred>(p)),
        make_wrapper(std::forward<GetKey>(k)),
        make_wrapper(std::forward<GetMapped>(m)))
);
// clang-format on

template <class HashedView, class MapConfig>
class static_map
    : private HashedView
    , private MapConfig {

    using type = static_map<HashedView, MapConfig>;

  public:
    using bucket_type   = typename HashedView::value_type;
    using value_type    = typename bucket_type::value_type;
    using reference     = typename bucket_type::reference;
    using key_equal     = typename MapConfig::key_equal;
    using key_getter    = typename MapConfig::key_getter;
    using mapped_getter = typename MapConfig::mapped_getter;

    static_assert(is_invocable<key_getter, reference>::value,
        "`key_getter` in boost::static_views::static_map must be invocable "
        "with `reference`.");

    using key_type = std::remove_reference_t<decltype(
        invoke(std::declval<key_getter const&>(), std::declval<reference>()))>;

    static_assert(is_invocable<key_getter, reference>::value,
        "`mapped_getter` in boost::static_views::static_map must be "
        "invocable with `reference`.");

    using mapped_type = std::remove_reference_t<decltype(invoke(
        std::declval<mapped_getter const&>(), std::declval<reference>()))>;

    static_assert(is_invocable_r<bool, key_equal, key_type&, key_type&>::value,
        "`key_equal` in boost::static_views::static_map must be invocable "
        "with two `key_type&` and, when invoked, return something "
        "convertible to `bool`..");

    using typename HashedView::size_type;
    using key_hasher = typename HashedView::hasher_type::first_fn;
    using index_type = key_type;

  private:
    using view_type   = HashedView;
    using config_type = MapConfig;

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto const& _hash_function() const noexcept
    {
        // TODO: This is ugly!
        return this->hash_function().second();
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto* _lookup(key_type const& k) const BOOST_STATIC_VIEWS_NOEXCEPT_IF(
        noexcept(invoke(std::declval<key_equal const&>(),
            std::declval<key_type const&>(),
            invoke(
                std::declval<key_getter const&>(), std::declval<reference>())))
        && noexcept(
               invoke(std::declval<static_map const&>()._hash_function(), k)))
    {
        struct pred_equal {
            key_getter const& get_key;
            key_equal const&  equal;
            key_type const&   key;

            BOOST_STATIC_VIEWS_FORCEINLINE
            BOOST_STATIC_VIEWS_CONSTEXPR
            auto operator()(value_type const& y) BOOST_STATIC_VIEWS_NOEXCEPT_IF(
                noexcept(invoke(std::declval<key_equal const&>(),
                    std::declval<key_type const&>(),
                    invoke(std::declval<key_getter const&>(), y)))) -> bool
            {
                return invoke(equal, key, invoke(get_key, y));
            }
        };

        auto const hash =
            view_type::bucket_size()
            // TODO: For optimal performance, I need a way to mark a hash
            // function "trustworthy". Such functions are then assumed to never
            // return values larger that bucker_count() which would avoid a
            // costly `mod` operation.
            * (invoke(_hash_function(), k) % view_type::bucket_count());
        return static_cast<HashedView const&>(*this).lookup(
            hash, pred_equal{this->key(), this->equal(), k});
    }

  public:
    BOOST_STATIC_VIEWS_CONSTEXPR
    static_map(view_type&& xs, config_type&& conf)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<view_type>::value&&
                std::is_nothrow_move_constructible<config_type>::value)
        : view_type{std::move(xs)}, config_type{std::move(conf)}
    {
    }

    static_map(static_map const&) = default;
    static_map(static_map&&)      = default;
    static_map& operator=(static_map const&) = default;
    static_map& operator=(static_map&&) = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto size() const noexcept -> size_type
    {
        return static_cast<view_type const&>(*this).size();
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto find(key_type const& k) const BOOST_STATIC_VIEWS_NOEXCEPT_IF(
        noexcept(std::declval<static_map const&>()._lookup(k))) -> value_type*
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
        if (x == nullptr) { throw key_not_found_error{}; }
        return invoke(this->mapped(), *x);
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) operator[](key_type const& k) const { return at(k); }

    using view_type::begin;
    using view_type::bucket_count;
    using view_type::bucket_size;
    using view_type::end;
};

} // namespace detail

template <std::size_t BucketCount = 0, std::size_t BucketSize = 0>
struct make_static_map_impl {
  private:
    template <class HashedView, class MapConfig>
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(
        HashedView view, MapConfig config) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            detail::static_map<HashedView, MapConfig>{
                std::move(view), std::move(config)});

  public:
    // clang-format off
    template <class View, class GetKey, class GetMapped,
        class KeyEqual = std::equal_to<void>, class Hasher = hash_c>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs, GetKey get_key, GetMapped&& get_mapped,
        KeyEqual&& key_equal = KeyEqual{}, Hasher&& hasher = Hasher{})
    // clang-format on
    {
        using view_type = std::remove_cv_t<std::remove_reference_t<View>>;
        static_assert(view_type::extent() != dynamic_extent,
            "For views of unknown size you need to manually specify a non-zero "
            "BucketCount.");

        constexpr auto bucket_count =
            (BucketCount == 0) ? 2 * view_type::extent() : BucketCount;
        constexpr auto bucket_size = (BucketSize == 0) ? 2 : BucketSize;

        static_assert(std::is_copy_constructible<GetKey>::value,
            "Current implemetation of boost::static_views::static_map requires "
            "GetKey to be copy constructible. If you absolutely cannot live "
            "with this, please submit a bug report.");
        auto view = hashed<bucket_count, bucket_size>(std::forward<View>(xs),
            compose(std::forward<Hasher>(hasher), GetKey{get_key}), key_equal);
        auto conf = detail::make_map_config(std::forward<KeyEqual>(key_equal),
            std::move(get_key), std::forward<GetMapped>(get_mapped));

        return call_impl(std::move(view), std::move(conf));
    }
};

#if 0
// clang-format off
template <std::size_t BucketCount = 0, std::size_t BucketSize = 0,
    class View = void, class GetKey = void, class GetMapped = void,
    class KeyEqual = std::equal_to<void>, class Hasher = hash_c>
BOOST_STATIC_VIEWS_CONSTEXPR
auto make_static_map(View&& xs, GetKey&& get_key, GetMapped&& get_mapped,
    KeyEqual&& key_equal = KeyEqual{}, Hasher&& hasher = Hasher{})
// clang-format on
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

    using view_type = std::remove_cv_t<std::remove_reference_t<View>>;
    static_assert(view_type::extent() != dynamic_extent,
        "For views of unknown size you need to manually specify a non-zero "
        "BucketCount.");
    constexpr auto bucket_count =
        (BucketCount == 0) ? 2 * view_type::extent() : BucketCount;
    constexpr auto bucket_size = (BucketSize == 0) ? 2 : BucketSize;

    auto view = hashed<bucket_count, bucket_size>(
        tricky_hasher{std::forward<Hasher>(hasher), get_key})(
        std::forward<View>(xs));

    auto conf = detail::make_map_config(std::forward<KeyEqual>(key_equal),
        std::forward<GetKey>(get_key), std::forward<GetMapped>(get_mapped));

    return detail::static_map<decltype(view), decltype(conf)>{
            std::move(view), std::move(conf)};
}
#endif

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_STATIC_MAP_HPP
