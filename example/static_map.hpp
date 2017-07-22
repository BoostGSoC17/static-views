//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_STATIC_MAP_HPP
#define BOOST_STATIC_VIEWS_STATIC_MAP_HPP

#include <type_traits>

#include <experimental/string_view>

#include <boost/config.hpp>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/detail/utils.hpp>
#include <boost/static_views/detail/invoke.hpp>
#include <boost/static_views/detail/find_first.hpp>
#include <boost/static_views/hashed.hpp>


struct key_not_found_error : std::exception {
    auto what() const noexcept -> char const* override
    { return _msg; }
private:
    static constexpr char const* _msg = "Key not found.";
};

constexpr char const* key_not_found_error::_msg;


namespace {

template <class _Char>
inline constexpr auto length_c(_Char const* const str) noexcept -> std::size_t
{
    std::size_t i = 0;
    while (str[i] != _Char{}) {
        ++i;
    }
    return (i == 0) ? 0 : (i - 1);
}

template <class _Char>
inline constexpr auto crc32_hash(_Char const* const str) noexcept -> std::uint32_t
{
    constexpr std::uint32_t INITXOR  = 0xFFFFFFFF;
    constexpr std::uint32_t FINALXOR = 0xFFFFFFFF;
    constexpr std::uint32_t CRCPOLY  = 0xEDB88320;

    auto const l = length_c(str);
    auto crcreg  = INITXOR;

    for (std::size_t j = 0; j < l; ++j) {
        auto b = static_cast<std::make_unsigned_t<_Char>>(str[j]);
        for (std::size_t i = 0; i < 8 * sizeof(_Char); ++i) {
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

template <class _Char>
inline constexpr auto simple_hash(_Char const* const str) noexcept
{
    std::size_t hash = 0;
    std::size_t i = 0;
    while (str[i] != _Char{}) {
        hash = 37 * hash + str[i];
        ++i;
    }
    return hash;
}

} // unnamed namespace 


namespace {
    template <class T, class = void>
    struct hash_impl;

    template <class T>
    struct hash_impl<T, std::enable_if_t<std::is_integral<T>::value>> {
        constexpr auto operator()(T const x) const
            noexcept 
            -> std::size_t
        { return static_cast<std::size_t>(x); }
    };

    template <>
    struct hash_impl<char const*> {
        constexpr auto operator()(char const* const x) const
            noexcept
            -> std::size_t
        {
            // return simple_hash(x) % _N;
            return static_cast<std::size_t>(crc32_hash(x));
        }
    };

    template <>
    struct hash_impl<std::experimental::string_view> {
        constexpr auto operator()(std::experimental::string_view const& x)
            const 
            noexcept
            -> std::size_t
        { return hash_impl<char const*>{}(x.data()); }
    };

} // unnamed namespace


struct hash_c {
    template <class T>
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(T&& x) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            hash_impl<std::decay_t<T>>{}(std::forward<T>(x))
        ))
    {
        return hash_impl<std::decay_t<T>>{}(std::forward<T>(x));
    }

};


template < class HashedView
         , class Pred
         >
class static_map : private HashedView {

public:
    using type                = static_map<HashedView, Pred>;
    using value_type          = std::remove_reference_t<decltype(
        std::declval<HashedView>()[std::declval<std::size_t>()][
            std::declval<std::size_t>()])>;
    using key_type            = std::remove_reference_t<decltype(
        std::get<0>(std::declval<value_type>()))>;
    using mapped_type         = std::remove_reference_t<decltype(
        std::get<1>(std::declval<value_type>()))>;
    using key_equal           = Pred;
    using hasher              = std::decay_t<decltype(
        std::declval<HashedView const&>().hash_function().hash_function)>;
    using difference_type     = std::ptrdiff_t;
    using size_type           = std::size_t;

private:
    using view_type           = HashedView;
    key_equal    const _eq;

    template <class ValueType>
    BOOST_FORCEINLINE
    static BOOST_STATIC_VIEWS_CONSTEXPR 
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO _get_key(ValueType&& x)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::get<0>(std::forward<ValueType>(x))
        ))
    {
        return std::get<0>(std::forward<ValueType>(x));
    }


    template <class ValueType>
    BOOST_FORCEINLINE
    static BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO _get_value(ValueType&& x)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::get<1>(std::forward<ValueType>(x))
        ))
    {
        return std::get<1>(std::forward<ValueType>(x));
    }


    BOOST_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO _hash_function() const
    {
        return this->hash_function().hash_function;
    }

    template <class Key>
    BOOST_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto _lookup(Key const& k) const
        // I know the following specification is formally wrong, but it "should"
        // be correct
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(boost::static_views::detail::utils::all(
            noexcept(std::declval<key_equal const&>()(
                std::declval<Key const&>(),
                std::declval<Key const&>())),
            noexcept(std::declval<hasher const&>()(
                std::declval<Key const&>()))
        ))
        -> value_type *
    {
        struct pred_equal {
            key_equal const& eq;
            Key       const& x;

            BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(value_type const& y)
                BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
                    std::declval<key_equal const&>()(
                        std::declval<Key const&>(),
                        std::declval<Key const&>())
                ))
            {
                return boost::static_views::invoke(eq, x, _get_key(y));
            }
        };

        auto const ys = static_cast<view_type const*>(this)->operator[](
            boost::static_views::invoke(_hash_function(), k));
        auto const i  = boost::static_views::find_first_i(ys,
            pred_equal{_eq, k});

        return (i < ys.size())
            ? &ys[i]
            : nullptr;
    }

public:
    template <class KeyEqual>
    BOOST_STATIC_VIEWS_CONSTEXPR static_map(view_type && xs, KeyEqual&& eq)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(boost::static_views::detail::utils::all(
            std::is_nothrow_move_constructible<view_type>::value,
            std::is_nothrow_constructible<key_equal, KeyEqual&&>::value
        ))
        : view_type{ std::move(xs) }
        , _eq{ std::forward<KeyEqual>(eq) }
    {
    }

    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<view_type const&>().size()
        ))
        -> size_type
    {
        return static_cast<view_type const*>(this)->size();
    }

    template <class Key>
    BOOST_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto find(Key const& k) const 
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<static_map const&>()._lookup(
                std::declval<Key const&>())
        ))
        -> value_type *
    {
        return _lookup(k);
    }

    template <class Key>
    BOOST_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto count(Key const& k) const 
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<static_map const&>()._lookup(
                std::declval<Key const&>())
        ))
        -> size_type
    {
        return _lookup(k) == nullptr ? 0 : 1;
    }

    template <class Key>
    BOOST_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto at(Key const& k) const
        -> mapped_type &
    {
        auto* x = _lookup(k);
        return x != nullptr
            ? _get_value(*x)
            : (throw key_not_found_error{}, _get_value(*x));
    }

    template <class Key>
    BOOST_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator[](Key const& k) const
        -> mapped_type &
    {
        return at(k);
    }
};


template < std::size_t BucketCount
         , std::size_t BucketSize
         , class View
         , class KeyEqual = std::equal_to<void>
         , class Hasher   = hash_c
         >
BOOST_STATIC_VIEWS_CONSTEXPR auto make_static_map(View&& xs,
    KeyEqual&& equal = KeyEqual{}, Hasher&& hasher = Hasher{})
{
    using std::get;
    using value_type = std::remove_reference_t<
        decltype(xs[std::declval<std::size_t>()])>;

    struct good_hasher {
        Hasher hash_function;

        BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(value_type const& x)
            const
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
                boost::static_views::invoke(std::declval<Hasher const&>(),
                    std::get<0>(std::declval<value_type const&>())) 
            ))
            -> std::size_t
        {
            return boost::static_views::invoke(hash_function, get<0>(x));
        }
    };
    auto view = boost::static_views::hashed<BucketCount, BucketSize>(
        good_hasher{std::forward<Hasher>(hasher)})(std::forward<View>(xs));

    return static_map<decltype(view), KeyEqual>(std::move(view),
        std::forward<KeyEqual>(equal));
}


#endif // BOOST_STATIC_VIEWS_STATIC_MAP_HPP
