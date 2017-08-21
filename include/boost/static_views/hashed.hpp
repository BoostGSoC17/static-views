//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_HASHED_HPP
#define BOOST_STATIC_VIEWS_HASHED_HPP

#include "algorithm_base.hpp"
#include "detail/config.hpp"
#include "detail/find_first.hpp"
#include "detail/invoke.hpp"
#include "raw_view.hpp"
#include "slice.hpp"
#include "through.hpp"
#include "view_base.hpp"
#include <limits>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class View, class Hasher>
struct Constrains {

  private:
    template <class H>
    struct Impl {
        template <class T>
        using call_t = decltype(invoke(std::declval<H>(),
            concepts::View::unsafe_at(
                std::declval<T>(), std::declval<std::size_t>())));

        BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_callable, T,
            (is_detected<call_t, T>::value),
            "`boost::static_views::hashed(hf)(xs)` requires hash "
            "function `hf` to be callable with `xs[i]` as an "
            "argument. This seems to be not the case here. Hence, "
            "the error.");

        BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_correct_return_type, T,
            (std::is_convertible<detected_t<call_t, T>,
                std::size_t>::value),
            "`boost::static_views::hashed(hf)(xs)` requires the "
            "return type of the hash function to be convertible to "
            "`size_t`, i.e. `size_t hash = invoke(hf, xs[i]);` "
            "should compile cleanly. This condition has, I'm afraid, "
            "been violated. Hence, the error.");
    };

    using constrains = concepts::and_<concepts::View,
        typename Impl<Hasher>::Is_callable,
        typename Impl<Hasher>::Has_correct_return_type>;

  public:
    static constexpr auto test() noexcept -> bool
    {
        return constrains::template test<View>();
    }

    static constexpr auto check() noexcept -> bool
    {
        return constrains::template check<View>();
    }
};

template <std::size_t BucketSize, std::size_t... Is>
struct hashed_init_impl {
    std::size_t storage[sizeof...(Is)];

    // clang-format off
    template <class View, class Hasher,
        class = std::enable_if_t<Constrains<View, Hasher>::test()>>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    explicit hashed_init_impl(View const& xs, Hasher const& h)
        : storage{((void)Is, hashed_init_impl::capacity())...}
    {
        auto const size = xs.size();
        for (std::size_t i = 0; i < size; ++i) {
            auto const hash = invoke(h, concepts::View::unsafe_at(xs, i));
            insert(i, bucket_size() * (hash % bucket_count()));
        }
    }
    // clang-format on

  private:
    static constexpr auto capacity() noexcept -> std::size_t
    {
        static_assert(sizeof...(Is) > 0,
            "[INTERNAL] Invalid use of hashed_init_impl.");
        return sizeof...(Is);
    }

    static constexpr auto bucket_size() noexcept -> std::size_t
    {
        static_assert(BucketSize > 0,
            "[INTERNAL] Invalid use of hashed_init_impl.");
        return BucketSize;
    }

    static constexpr auto bucket_count() noexcept -> std::size_t
    {
        static_assert(sizeof...(Is) % bucket_size() == 0,
            "[INTERNAL] Invalid use of hashed_init_impl.");
        return capacity() / bucket_size();
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto insert(std::size_t const i, std::size_t const guess) -> void
    {
#if defined(__cpp_constexpr) && __cpp_constexpr >= 201603
        // We have C++17 constexpr lambdas
        constexpr auto is_empty = [](auto const x) noexcept
        {
            return x == hashed_init_impl::capacity();
        };
#else
        struct is_empty_impl {
            BOOST_STATIC_VIEWS_CONSTEXPR
            auto operator()(std::size_t const x) const noexcept
            {
                return x == hashed_init_impl::capacity();
            }
        };
        constexpr is_empty_impl is_empty{};
#endif

        auto const bucket =
            slice(guess, guess + bucket_size())(raw_view(storage));
        auto const p = find_first_i(bucket, is_empty);

        if (p != bucket_size()) {
            storage[guess + p] = i;
        }
        else {
            make_full_bucket_error("Bucket is full.");
        }
    }
};

template <std::size_t BucketSize, class V, class H, std::size_t... Is>
BOOST_STATIC_VIEWS_CONSTEXPR auto make_hashed_init_impl(
    V const& xs, H const& h, std::index_sequence<Is...> /*unused*/)
{
    return hashed_init_impl<BucketSize, Is...>{xs, h};
}

template <std::size_t BucketCount, std::size_t BucketSize, class V,
    class H>
BOOST_STATIC_VIEWS_CONSTEXPR auto make_hashed_init_impl(
    V const& xs, H const& h)
{
    static_assert(
        std::numeric_limits<std::size_t>::max() / BucketCount
            >= BucketSize,
        "[INTERNAL] Overflow detected.");
    return make_hashed_init_impl<BucketSize>(
        xs, h, std::make_index_sequence<BucketCount * BucketSize>{});
}

template <std::size_t BucketCount, std::size_t BucketSize, class View,
    class Hasher>
struct hashed_impl
    : view_adaptor_base<
          hashed_impl<BucketCount, BucketSize, View, Hasher>, View> {

  private:
    using view_type = typename View::type;

    static_assert(is_wrapper<Hasher>(),
        "[INTERNAL] Invalid use of hashed_impl.");

    using hasher_type = typename Hasher::type;

    using value_type = std::remove_reference_t<decltype(
        std::declval<view_type const&>().unsafe_at(
            std::declval<std::size_t>()))>;

  public:
    /// \brief Returns the number buckets.
    static constexpr auto bucket_count() noexcept
    {
        static_assert(BucketCount > 0,
            "[INTERNAL] Invalid use of hashed_impl.");
        return BucketCount;
    }

    /// \brief Returns the capacity of each bucket.
    static constexpr auto bucket_size() noexcept
    {
        static_assert(
            BucketSize > 0, "[INTERNAL] Invalid use of hashed_impl.");
        return BucketSize;
    }

  private:
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    static auto empty(std::size_t const x) noexcept -> bool
    {
        return x == bucket_count() * bucket_size();
    }

  public:
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
    ///   be used explicitly, use the :cpp:var:`hashed` factory
    ///   function instead.
    /// \endverbatim
    template <std::size_t... Is>
    BOOST_STATIC_VIEWS_CONSTEXPR hashed_impl(View&& xs, Hasher&& hf,
        std::size_t (&storage)[hashed_impl::bucket_count()
                               * hashed_impl::bucket_size()],
        std::index_sequence<Is...> /*unused*/)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            std::is_nothrow_constructible<
                typename hashed_impl::view_adaptor_base_type,
                View&&>::value,
            std::is_nothrow_move_constructible<Hasher>::value))
        : hashed_impl::view_adaptor_base_type{std::move(xs)}
        , _hf{std::move(hf)}
        , _storage{storage[Is]...}
    {
    }

    /// \brief Returns the capacity of the view.

    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. Hashed view is a view of buckets rather than
    /// individual elements. This function is thus equivalent to
    /// :cpp:func:`bucket_count()
    /// <detail::hashed_impl::bucket_count()>`.
    /// \endverbatim
    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return static_cast<std::ptrdiff_t>(bucket_count());
    }

    /// \brief Returns a reference to the hash function.
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO hash_function() const noexcept
    {
        static_assert(noexcept(_hf.get()),
            "[INTERNAL] hashed_impl assumes that _hf has a noexcept "
            "get().");
        return _hf.get();
    }

    /// \brief Returns the bucket corresponding to \p hash.

    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. It returns a view of elements that have hash
    /// ``hash``. If there are no such elements, the returned view
    /// will have size zero. \endverbatim
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator[](std::size_t const hash) const
    // noexcept /* TODO add specifiers */
    {
#if defined(__cpp_constexpr) && __cpp_constexpr >= 201603
        constexpr auto is_empty = [](auto const x) noexcept
        {
            return x == bucket_count() * bucket_size();
        };
#else
        struct is_empty_impl {
            BOOST_STATIC_VIEWS_CONSTEXPR
            auto operator()(std::size_t const x) const noexcept
            {
                return x == bucket_count() * bucket_size();
            }
        };
        constexpr is_empty_impl is_empty{};
#endif
        auto const i = bucket_size() * (hash % bucket_count());
        auto const n = find_first_i(
            slice(i, i + bucket_size())(raw_view(_storage)),
            is_empty);
        return through(slice(i, i + n)(raw_view(_storage)))(
            this->parent());
    }

    template <class Predicate>
    BOOST_STATIC_VIEWS_FORCEINLINE   //
        BOOST_STATIC_VIEWS_CONSTEXPR //
        auto
        lookup(std::size_t h, Predicate&& p) const noexcept
        -> value_type*
    {
        auto const bucket =
            slice(h, h + bucket_size())(raw_view(_storage));

        for (std::size_t i = 0;
             (i < bucket_size()) && !empty(bucket.unsafe_at(i));
             ++i) {

            auto* const x =
                &this->parent().unsafe_at(bucket.unsafe_at(i));
            if (invoke(p, *x)) {
                return x;
            }
        }
        return nullptr;
    }

  private:
    Hasher      _hf;
    std::size_t _storage[bucket_count() * bucket_size()];
};

template <std::size_t BucketCount, std::size_t BucketSize>
struct make_hashed_impl {

    static_assert(BucketCount > 0,
        "`boost::static_views::hashed<BucketCount, BucketSize>` "
        "requires BucketCount to be greater than zero.");
    static_assert(BucketCount > 0,
        "`boost::static_views::hashed<BucketCount, BucketSize>` "
        "requires BucketSize to be greater than zero.");

    // clang-format off
    template <class View, class Hasher>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View&& xs, Hasher&& hf) const
    {
        static_assert(is_wrapper<std::decay_t<View>>::value,
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        static_assert(is_wrapper<std::decay_t<Hasher>>::value,
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        using view_type = typename std::decay_t<View>::type;
        using hasher_type = typename std::decay_t<Hasher>::type;
        concepts::View::check<view_type>();
        Constrains<view_type, hasher_type>::check();

        auto const& xs_ref = xs.get();
        auto const& hf_ref = hf.get();
        auto init = detail::make_hashed_init_impl<
            BucketCount, BucketSize>(xs_ref, hf_ref);
        return detail::hashed_impl<BucketCount, BucketSize,
            std::decay_t<View>,
            decltype(make_wrapper(std::forward<Hasher>(hf).get()))>{
            std::forward<View>(xs),
            make_wrapper(std::forward<Hasher>(hf).get()),
            init.storage,
            std::make_index_sequence<BucketCount * BucketSize>{}};
    }
    // clang-format on
};

template <std::size_t BucketCount, std::size_t BucketSize>
struct make_hashed_algo_impl {
    template <class Hasher>
    BOOST_STATIC_VIEWS_FORCEINLINE BOOST_STATIC_VIEWS_CONSTEXPR auto
                                   operator()(Hasher&& hf) const
    {
        return algorithm(make_hashed_impl<BucketCount, BucketSize>{},
            make_wrapper(std::forward<Hasher>(hf)));
        /*
            decltype(make_wrapper(std::forward<Proxy>(proxy)))>{
            make_through_impl{},
           make_wrapper(std::forward<Proxy>(proxy))}*/
    }
};

} // end namespace detail

/// \brief A functor for creating "hashed views"

/// \f[
/// \text{hashed} : (\text{T} \to \mathbb{N}) \to \text{View} \to
/// \text{View} \f]
///
/// Given a hash function `hf` and a view `xs`, creates a view of `xs`
/// which uses hashes as indices to access elements.
///
/// So, say, \f$ \text{hf} : \text{T} \to \mathbb{N} \f$ is a hash
/// function, `xs` is a view of elements of type `T`, and `ys =`
/// #hashed `(hf)(xs)`. Then
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
///   :math:`\text{hashed}(\text{hf}) : \text{View} \to \text{View}`
///   models the :ref:`algorithm <algorithm-concept>` concept.
/// \endverbatim
#if defined(DOXYGEN_IN_HOUSE)
template <std::size_t BucketCount, std::size_t BucketSize>
constexpr auto hashed = implementation detail;
#else
inline namespace {
template <std::size_t BucketCount, std::size_t BucketSize>
BOOST_STATIC_VIEWS_CONSTEXPR auto const& hashed =
    ::BOOST_STATIC_VIEWS_NAMESPACE::_static_const<
        detail::make_hashed_algo_impl<BucketCount, BucketSize>>;
} // anonymous namespace
#endif

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_HASHED_HPP
