//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_HASHED_HPP
#define BOOST_STATIC_VIEWS_HASHED_HPP

#include <type_traits>

#include <boost/config.hpp>
#include <boost/static_views/algorithm_base.hpp>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/detail/find_first.hpp>
#include <boost/static_views/detail/invoke.hpp>
#include <boost/static_views/slice.hpp>
#include <boost/static_views/through.hpp>
#include <boost/static_views/view_base.hpp>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <std::size_t BucketSize, std::size_t... Is>
struct hashed_init_impl {
    std::size_t storage[sizeof...(Is)];

    template <class View, class Hasher>
    BOOST_FORCEINLINE BOOST_STATIC_VIEWS_CONSTEXPR hashed_init_impl(
        View&& xs, Hasher&& h)
        : storage{((void)Is, hashed_init_impl::capacity())...}
    {
        static_assert(
            sizeof...(Is) % BucketSize == 0, "hashed_init_impl is broken.");
        constexpr std::size_t bucket_count = sizeof...(Is) / BucketSize;
        auto const            size         = xs.size();
        for (std::size_t i = 0; i < size; ++i) {
            insert(i, BucketSize * (h(xs[i]) % bucket_count));
        }
    }

  private:
    static constexpr auto capacity() noexcept { return sizeof...(Is); }

    BOOST_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto insert(
        std::size_t const i, std::size_t const guess) -> void
    {
        struct is_empty {
            BOOST_STATIC_VIEWS_CONSTEXPR
            auto operator()(std::size_t const x) noexcept
            {
                return x == hashed_init_impl::capacity();
            }
        };
        auto const bucket = slice(guess, guess + BucketSize)(raw_view(storage));
        auto const p      = find_first_i(bucket, is_empty{});

        if (BOOST_LIKELY(p != BucketSize)) {
            storage[guess + p] = i;
        }
        else {
            make_full_bucket_error("Bucket is full.");
        }
    }
};

template <std::size_t BucketSize, class V, class H, std::size_t... Is>
BOOST_STATIC_VIEWS_CONSTEXPR auto make_hashed_init_impl(
    V&& xs, H&& h, std::index_sequence<Is...>)
{
    return hashed_init_impl<BucketSize, Is...>{
        std::forward<V>(xs), std::forward<H>(h)};
}

template <std::size_t BucketCount, std::size_t BucketSize, class V, class H>
BOOST_STATIC_VIEWS_CONSTEXPR auto make_hashed_init_impl(V&& xs, H&& h)
{
    return make_hashed_init_impl<BucketSize>(std::forward<V>(xs),
        std::forward<H>(h),
        std::make_index_sequence<BucketCount * BucketSize>{});
}

template <std::size_t BucketCount, std::size_t BucketSize, class View,
    class Hasher>
struct hashed_impl
    : view_adaptor_base<hashed_impl<BucketCount, BucketSize, View, Hasher>,
          View> {

    /// \brief Constructs a hashed view of \p xs using \p hf as a hash
    /// function.

    /// \tparam BucketCount
    /// Number of buckets.
    /// \tparam BucketSize
    /// Capacity of each bucket.
    /// \tparam View
    /// \verbatim embed:rst:leading-slashes
    /// Wrapper around a view, i.e. ``typename View::type`` is a view
    /// and
    /// must model the :ref:`view <view-concept>` concept.
    /// \endverbatim
    /// \param xs      Rvalue reference to a wrapper around a view.
    /// \param hf      Hash function to use.
    /// \param storage Array of indices initialised by
    /// #hashed_init_impl.
    ///
    ///
    /// \verbatim embed:rst:leading-slashes
    /// .. note::
    ///   This function requires an initialiser storage as an argument
    ///   which
    ///   is an implementation detail. This constructor is thus not
    ///   meant to
    ///   be used explicitly, use the :cpp:var:`hashed` factory function
    ///   instead.
    /// \endverbatim
    template <class HashFunction, std::size_t... Is>
    BOOST_STATIC_VIEWS_CONSTEXPR hashed_impl(View&& xs, HashFunction&& hf,
        std::size_t (&storage)[BucketCount * BucketSize],
        std::index_sequence<Is...>)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_constructible<
                typename hashed_impl::view_adaptor_base_type, View&&>::value,
            std::is_nothrow_constructible<Hasher, HashFunction&&>::value))
        : hashed_impl::view_adaptor_base_type{std::move(xs)}
        , _hf{std::forward<HashFunction>(hf)}
        , _storage{storage[Is]...}
    {
    }

    /// \brief Returns the number buckets.
    static constexpr auto bucket_count() noexcept { return BucketCount; }

    /// \brief Returns the capacity of each bucket.
    static constexpr auto bucket_size() noexcept { return BucketSize; }

    /// \brief Returns the capacity of the view.

    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. Hashed view is a view of buckets rather than individual
    /// elements. This function is thus equivalent to
    /// :cpp:func:`bucket_count()
    /// <detail::hashed_impl::bucket_count()>`.
    /// \endverbatim
    static constexpr auto capacity() noexcept { return bucket_count(); }

    /// \brief Returns a reference to the hash function.
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO hash_function() const noexcept
    {
        return _hf;
    }

    /// \brief Returns the bucket corresponding to \p hash.

    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. It returns a view of elements that have hash ``hash``.
    /// If there are no such elements, the returned view will have size
    /// zero.
    /// \endverbatim
    BOOST_FORCEINLINE
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
        auto const n = find_first_i(
            slice(i, i + bucket_size())(raw_view(_storage)), is_empty{});
        return through(slice(i, i + n)(raw_view(_storage)))(this->parent());
    }

  private:
    Hasher      _hf;
    std::size_t _storage[bucket_count() * bucket_size()];
};

template <std::size_t BucketCount, std::size_t BucketSize>
struct make_hashed_impl {
    template <class View, class Hasher>
    BOOST_FORCEINLINE BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(
        View&& xs, Hasher&& hf) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(detail::utils::all(
            noexcept(detail::make_hashed_init_impl<BucketCount, BucketSize>(
                xs.get(), hf)),
            noexcept(detail::hashed_impl<BucketCount, BucketSize,
                std::decay_t<View>, std::decay_t<Hasher>>{
                std::forward<View>(xs), std::forward<Hasher>(hf),
                std::declval<std::size_t (&)[BucketCount * BucketSize]>(),
                std::make_index_sequence<BucketCount * BucketSize>{}})))
    {
        auto init = detail::make_hashed_init_impl<BucketCount, BucketSize>(
            xs.get(), hf);
        return detail::hashed_impl<BucketCount, BucketSize, std::decay_t<View>,
            std::decay_t<Hasher>>{std::forward<View>(xs),
            std::forward<Hasher>(hf), init.storage,
            std::make_index_sequence<BucketCount * BucketSize>{}};
    }
};
} // end namespace detail

/// \brief A functor for creating "hashed views"

/// \f[
/// \text{hashed} : (\text{T} \to \mathbb{N}) \to \text{View} \to \text{View}
/// \f]
///
/// Given a hash function `hf` and a view `xs`, creates a view of `xs`
/// which uses hashes as indices to access elements.
///
/// So, say, \f$ \text{hf} : \text{T} \to \mathbb{N} \f$ is a hash function,
/// `xs` is a view of elements of type `T`, and `ys =` #hashed `(hf)(xs)`. Then
///
/// \f[
/// \{ x \in \text{xs} \;\;|\;\;
///     \text{hf}(x) = h \,\%\, \text{ys.bucket_count}() \}
///     = \text{ys}[h] \;, \forall h \in \mathbb{N}\;.
/// \f]
///
/// \verbatim embed:rst:leading-slashes
/// .. note::
///   Haskell notation is used here, i.e. the function is curried and
///   :math:`\text{hashed}(\text{hf}) : \text{View} \to \text{View}` models
///   the :ref:`algorithm <algorithm-concept>` concept.
/// \endverbatim
#if defined(DOXYGEN_IN_HOUSE)
template <std::size_t BucketCount, std::size_t BucketSize>
constexpr auto hashed = implementation detail;
#else
inline namespace {
template <std::size_t BucketCount, std::size_t BucketSize>
BOOST_STATIC_VIEWS_CONSTEXPR auto const& hashed =
    ::BOOST_STATIC_VIEWS_NAMESPACE::_static_const<::
            BOOST_STATIC_VIEWS_NAMESPACE::detail::make_algorithm_impl<detail::
                    make_hashed_impl<BucketCount, BucketSize>>>;
}
#endif

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_HASHED_HPP
