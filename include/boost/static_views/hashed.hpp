//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_HASHED_HPP
#define BOOST_STATIC_VIEWS_HASHED_HPP

#include <type_traits>

#include <boost/config.hpp>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/view_base.hpp>
#include <boost/static_views/algorithm_base.hpp>
#include <boost/static_views/detail/find_first.hpp>
#include <boost/static_views/slice.hpp>
#include <boost/static_views/through.hpp>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE


namespace detail {

    template <std::size_t BucketSize, std::size_t... Is>
    struct hashed_init_impl {
        std::size_t storage[sizeof...(Is)];

        template <class View, class Hasher>
        BOOST_STATIC_VIEWS_CONSTEXPR hashed_init_impl(View&& xs, Hasher&& h)
            : storage{ ((void)Is, hashed_init_impl::capacity())... }
        {
            static_assert(sizeof...(Is) % BucketSize == 0, "Hashed view is"
                "broken.");
            constexpr std::size_t bucket_count = sizeof...(Is) / BucketSize;
            auto const size = xs.size();
            for (std::size_t i = 0; i < size; ++i) {
                insert(i, BucketSize * (h(xs[i]) % bucket_count));
            }
        }

    private:
        static BOOST_STATIC_VIEWS_CONSTEXPR auto capacity() noexcept 
        {
            return sizeof...(Is); 
        }

        BOOST_STATIC_VIEWS_CONSTEXPR 
        auto insert(std::size_t const i, std::size_t const guess)
        {
            struct is_empty {
                BOOST_STATIC_VIEWS_CONSTEXPR
                auto operator()(std::size_t const x) noexcept
                {
                    return x == hashed_init_impl::capacity();
                }
            };
            auto const bucket = slice(guess, guess + BucketSize)(
                raw_view(storage));
            auto const p      = find_first_i(bucket, is_empty{});

            if (BOOST_LIKELY(p != BucketSize)) {
                storage[guess + p] = i;
            }
            else {
                make_full_bucket_error("Bucket is full.");
            }
        }
    };

    template <std::size_t BucketSize, class V, class H,
        std::size_t... Is>
    BOOST_STATIC_VIEWS_CONSTEXPR auto make_hashed_init_impl(V&& xs, H&& h,
        std::index_sequence<Is...>)
    {
        return hashed_init_impl<BucketSize, Is...>{std::forward<V>(xs),
            std::forward<H>(h)};
    }

    template <std::size_t BucketCount, std::size_t BucketSize, class V, class H>
    BOOST_STATIC_VIEWS_CONSTEXPR auto make_hashed_init_impl(V&& xs, H&& h)
    {
        return make_hashed_init_impl<BucketSize>(std::forward<V>(xs),
            std::forward<H>(h), std::make_index_sequence<
            BucketCount * BucketSize>{});
    }

    template < std::size_t BucketCount
             , std::size_t BucketSize
             , class V >
    struct hashed_impl
        : view_adaptor_base< hashed_impl< BucketCount
                                        , BucketSize
                                        , V >
                           , V > {

        template <std::size_t... Is>
        BOOST_STATIC_VIEWS_CONSTEXPR hashed_impl(V&& xs, 
            std::size_t (&storage)[BucketCount * BucketSize],
            std::index_sequence<Is...>)
            : hashed_impl::view_adaptor_base_type{std::move(xs)}
            , _storage{ storage[Is]... }
        {
        }

        static constexpr auto bucket_count() noexcept
        {
            return BucketCount;
        }

        static constexpr auto bucket_size() noexcept
        {
            return BucketSize;
        }

        static constexpr auto capacity() noexcept
        {
            return bucket_count();
        }

        BOOST_STATIC_VIEWS_CONSTEXPR
        auto operator[](std::size_t const hash) const
            // noexcept /* TODO add specifiers */
        {
            struct is_empty {
                BOOST_STATIC_VIEWS_CONSTEXPR
                auto operator()(std::size_t const x) noexcept
                {
                    return x == bucket_count() * bucket_size();
                }
            };
            auto const i = bucket_size() * (hash % bucket_count());
            auto const n = find_first_i(slice(i, i + bucket_size())(
                raw_view(_storage)), is_empty{});
            return through(slice(i, i + n)(raw_view(_storage)))(
                this->parent());
        }

    private:
        std::size_t  _storage[bucket_count() * bucket_size()];
    };

    template <std::size_t BucketCount, std::size_t BucketSize>
    struct make_hashed_impl {
        template <class View, class Hasher>
        BOOST_STATIC_VIEWS_CONSTEXPR
        auto operator()(View&& xs, Hasher&& hf) const
        {
            auto init = detail::make_hashed_init_impl<BucketCount,
                BucketSize>(xs.get(), hf);
            return detail::hashed_impl<BucketCount, BucketSize, 
                std::decay_t<View>>{std::forward<View>(xs), init.storage,
                std::make_index_sequence<BucketCount * BucketSize>{}};
        }
    };
} // end namespace detail


inline namespace {
    template <std::size_t BucketCount, std::size_t BucketSize>
    BOOST_STATIC_VIEWS_CONSTEXPR auto const& hashed =
        ::BOOST_STATIC_VIEWS_NAMESPACE::_static_const<
        ::BOOST_STATIC_VIEWS_NAMESPACE::detail::make_algorithm_impl<
            detail::make_hashed_impl<BucketCount, BucketSize> >>;
}


BOOST_STATIC_VIEWS_END_NAMESPACE


// =============================================================================

/*
namespace detail {

    template <std::size_t... Is>
    struct hashed_init_impl {
        std::size_t _is[hashed_init_impl::capacity()];

        template <class View, class Hasher>
        constexpr hashed_init_impl(View const& xs, Hasher&& h) noexcept
            : _is{ ((void)Is, xs.size())... }
        {
            auto size = xs.size();
            for (std::size_t i = 0; i < size; ++i) {
                insert(i, h(xs.at(i)) % capacity);
            }
        }

    private:
        static constexpr auto capacity() noexcept { return sizeof...(Is); }

        constexpr auto insert(std::size_t const i, std::size_t const guess)
            noexcept
        {
            std::size_t indent = find_first_i(raw_view(_is) | drop(guess),
                empty);
            if (guess + indent < capacity) {
                _is[guess + indent] = i;
                return;
            }

            indent = find_first_i(raw_view(_is) | take(guess),
                empty);
            _is[indent] = i;
        }

        static constexpr auto empty(std::size_t const x) noexcept
        {
            return x == capacity; 
        }
    };

    template <class V, class H, std::size_t... Is>
    constexpr auto _make_hashed_init_impl(V&& xs, H&& h, 
        std::index_sequence<Is...>) noexcept
    {
        return hashed_init_impl<Is...>{std::forward<V>(xs), std::forward<H>(h)};
    }

    template <std::size_t Capacity, class V, class H>
    constexpr auto make_hashed_init_impl(V&& xs, H&& h)
    {
        return Capacity >= xs.size()
            ? _make_hashed_init_impl(std::forward<V>(xs), std::forward<H>(h),
                std::make_index_sequence<Capacity>{})
            : (throw std::out_of_range{"detail::make_hashed_init_impl() "
                "`Capacity` too small."}, _make_hashed_init_impl(
                std::forward<V>(xs), std::forward<H>(h), 
                std::make_index_sequence<Capacity>{} ));
    }

    template <std::size_t Capacity, class V, class Hasher>
    struct hashed_impl : view_adaptor_base<hashed_impl<Capacity, V, Hasher>, V> {
        friend class ::view_adaptor_core_access;

        template <std::size_t... Is>
        constexpr hashed_impl(V&& xs, Hasher&& h,
            hashed_init_impl<Is...>&& init) noexcept
            : base_type{std::move(xs)}, _hf{std::move(h)}, _is{init._is[Is]...}
        {
        }

        static constexpr auto capacity() noexcept
        {
            return Capacity;
        }

        constexpr auto operator()() const noexcept
        {

        }

        constexpr decltype(auto) hash_function() const noexcept
        {
            return _hf.get();
        }
        
    private:
        std::size_t  _is[capacity()];
        Hasher       _hf;
        using base_type = view_adaptor_base<hashed_impl<
            Capacity, V, Hasher>, V>;

        constexpr auto map(std::size_t const i) const noexcept
        {
            return i < capacity()
                ? _is[i]
                : (throw std::out_of_range{"sort_impl::map(): index `i` "
                    " is out of bounds."}, 0);
        }
    };

    template <std::size_t Capacity>
    struct make_hashed_impl {
        template <class View, class Hasher>
        constexpr auto operator()(Hasher&& h, View&& xs) const noexcept
        {
            auto hf = make_wrapper(std::forward<Hasher>(h));
            auto init = detail::make_hashed_init_impl<Capacity>(xs.get(), 
                hf.get());
            return detail::hashed_impl<Capacity, View, Hasher>{
                std::forward<View>(xs), std::move(hf), std::move(init)};
        }
    };
} // end namespace detail


template <std::size_t Capacity>
constexpr auto hashed = make_pipe_variable(
    detail::make_hashed_impl<Capacity>{});
*/
// =============================================================================




#endif // BOOST_STATIC_VIEWS_HASHED_HPP
