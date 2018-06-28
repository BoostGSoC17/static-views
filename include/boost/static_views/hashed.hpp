//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_HASHED_HPP
#define BOOST_STATIC_VIEWS_HASHED_HPP

#include "concepts.hpp"
#include "detail/find_first.hpp"
#include "detail/invoke.hpp"
#include "drop.hpp"
#include "raw_view.hpp"
#include "slice.hpp"
#include "take.hpp"
#include "through.hpp"
#include <type_traits>
#include <iostream>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

#if 0
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
#endif

#if defined(BOOST_STATIC_VIEWS_CONCEPTS)
template <class H, class T>
concept bool               Hasher =
    MoveConstructible<H>&& is_invocable_r<std::size_t, H, T>::value;
#else
template <class H, class T>
constexpr bool             Hasher =
    MoveConstructible<H>&& is_invocable_r<std::size_t, H, T>::value;
#endif


#if 0
template <std::size_t BucketSize, std::size_t... Is>
struct hashed_init_impl {
    std::size_t storage[sizeof...(Is)];

    // clang-format off
    template <class V, class H
        BOOST_STATIC_VIEWS_REQUIRES(View<V> && Hasher<H, typename V::reference>)
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    hashed_init_impl(V const& xs, H const& h)
        : storage{((void)Is, hashed_init_impl::capacity())...}
    {
        using index_type = typename V::index_type;
        auto const size = static_cast<index_type>(xs.size());
        for (index_type i = 0; i < size; ++i) {
            auto const hash = invoke(h, xs.unsafe_at(i));
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
#endif

template <std::size_t BucketCount, std::size_t BucketSize, class Wrapper,
    class Hasher>
struct hashed_view_impl
    : view_adaptor_base<
          hashed_view_impl<BucketCount, BucketSize, Wrapper, Hasher>, Wrapper> {

  private:
    using view_type = typename Wrapper::value_type;

    static_assert(is_wrapper<Hasher>(),
        "[INTERNAL] Invalid use of hashed_impl.");

    using base        = view_adaptor_base<
        hashed_view_impl<BucketCount, BucketSize, Wrapper, Hasher>, Wrapper>;
    using base_index_type = typename base::index_type;

  public:
    using hasher_type = typename Hasher::value_type;
    using index_type = std::size_t;
    using typename base::size_type;

    using base::parent;

    /// \brief Returns the number buckets.
    static constexpr auto bucket_count() noexcept
    {
        static_assert(BucketCount > 0,
            "Zero-sized boost::static_views::hashed_view is not supported.");
        return static_cast<size_type>(BucketCount);
    }

    /// \brief Returns the capacity of each bucket.
    static constexpr auto bucket_size() noexcept
    {
        static_assert(BucketSize > 0, "Zero-sized buckets are not supported by "
                                      "boost::static_views::hashed_view.");
        return static_cast<size_type>(BucketSize);
    }

  private:
    using storage_type = base_index_type[bucket_count() * bucket_size()];
    struct array_wrapper {
        storage_type unpack;
    };

    std::tuple<array_wrapper, Hasher> _data;

    // Returns whether a bucket at given index is empty.
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    static auto empty(base_index_type const x) noexcept -> bool
    {
        return x
               == static_cast<base_index_type>(bucket_count() * bucket_size());
    }

    struct is_empty {
        constexpr auto operator()(base_index_type const x) const noexcept
            -> bool
        {
            return empty(x);
        }
    };

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto storage() const & noexcept -> storage_type const&
    {
        return std::get<0>(_data).unpack;
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto storage() & noexcept -> storage_type&
    {
        return std::get<0>(_data).unpack;
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto bucket(base_index_type const i) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    {
        // Note that i absolutely must not exceed the size of storage.
        return take_exactly(drop_exactly(raw_view(storage()), i),
            std::integral_constant<base_index_type, bucket_size()>{});
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto insert(typename base::index_type const i,
        typename base::index_type const         guess) -> void
    {
        auto const p = find_first_i(bucket(guess), is_empty{});
        if (p != bucket_size()) {
            storage()[guess + p] = i;
        }
        else {
            make_full_bucket_error("Bucket is full.");
        }
    }

    /// \brief Finds an element in a bucket at the given index which satisfies
    /// the given predicate.
    // clang-format off
    template <class Predicate
        BOOST_STATIC_VIEWS_REQUIRES(
            is_invocable_r<bool, Predicate, typename base::reference>::value)
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto find_in_bucket(typename base::index_type const h, Predicate&& p)
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(invoke(p, std::declval<typename base::reference>())))
        -> typename base::value_type*
    {
        auto const xs = bucket(h);
        for (typename base::index_type i = 0;
             i < bucket_size() && !empty(xs.unsafe_at(i)); ++i) {
            // i -- index in the bucket
            // xs.unsafe_at(i) -- index in the parent view
            // parent().unsafe_at(xs.unsafe_at(i)) -- element in the parent
            // view.
            auto& x = parent().unsafe_at(xs.unsafe_at(i));
            if (invoke(p, x)) return &x;
        }
        return nullptr;
    }

    // clang-format off
    template <class Equal>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto insert(typename base::index_type const i,
        typename base::index_type const guess, Equal& equal) -> void
    // clang-format on
    {
        struct Predicate {
            Equal&                                     _equal;
            std::add_const_t<typename base::reference> _value;

            constexpr auto operator()(typename base::reference x)
            {
                return invoke(_equal, _value, x);
            }
        };

        if (find_in_bucket(guess, Predicate{equal, parent().unsafe_at(i)})
            != nullptr) {
            // We already have this element.
            return;
        }
        auto const p = find_first_i(bucket(guess), is_empty{});
        if (p != bucket_size()) {
            storage()[guess + p] = i;
        }
        else {
            make_full_bucket_error("Bucket is full.");
        }
    }

    BOOST_STATIC_VIEWS_CONSTEXPR auto initialise_storage()
    {
        using I = typename base::index_type;
        for (std::size_t i = 0; i < bucket_count() * bucket_size(); ++i)
            storage()[i] = bucket_count() * bucket_size();

        for (I i = 0, size = static_cast<I>(parent().size()); i < size;
             ++i) {
            auto const hash = invoke(hash_function(), parent().unsafe_at(i));
            insert(i, static_cast<I>(bucket_size() * (hash % bucket_count())));
        }
    }

    template <class Equal>
    BOOST_STATIC_VIEWS_CONSTEXPR auto initialise_storage(Equal&& equal)
    {
        using I = typename base::index_type;
        for (std::size_t i = 0; i < bucket_count() * bucket_size(); ++i)
            storage()[i] = bucket_count() * bucket_size();

        for (I i = 0, size = static_cast<I>(parent().size()); i < size;
             ++i) {
            auto const hash = invoke(hash_function(), parent().unsafe_at(i));
            insert(i, static_cast<I>(bucket_size() * (hash % bucket_count())),
                equal);
        }
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
    BOOST_STATIC_VIEWS_CONSTEXPR
    hashed_view_impl(Wrapper&& xs, Hasher&& hf)
        : base{std::move(xs)}, _data{array_wrapper{}, std::move(hf)}
    {
        initialise_storage();
    }

    template <class Equal>
    BOOST_STATIC_VIEWS_CONSTEXPR
    hashed_view_impl(Wrapper&& xs, Hasher&& hf, Equal&& equal)
        : base{std::move(xs)}, _data{array_wrapper{}, std::move(hf)}
    {
        initialise_storage(std::forward<Equal>(equal));
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
    auto const& hash_function() const& noexcept
    {
        return std::get<1>(_data).get();
    }

    /// \brief Returns the bucket corresponding to \p hash.

    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. It returns a view of elements that have hash
    /// ``hash``. If there are no such elements, the returned view
    /// will have size zero. \endverbatim
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator[](index_type const hash)
        const& BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    {
        auto const i = static_cast<typename base::index_type>(
            bucket_size() * (hash % bucket_count()));
        auto const n = find_first_i(bucket(i), is_empty{});
        return through(parent(), slice(raw_view(storage()), i, i + n));
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto unsafe_at(index_type const hash)
        const& BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    {
        return this->operator[](hash);
    }

    using reference = decltype(
        std::declval<hashed_view_impl const&>()[std::declval<index_type>()]);
    using value_type = std::remove_reference_t<reference>;

    template <class Predicate>
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto lookup(index_type h, Predicate&& p) const noexcept
        -> typename value_type::value_type*
    {
        auto const xs = bucket(h);
        for (typename base::index_type i = 0;
             i < bucket_size() && !empty(xs.unsafe_at(i)); ++i) {

            auto* const x = &parent().unsafe_at(xs.unsafe_at(i));
            if (invoke(p, *x)) return x;
        }
        return nullptr;
    }

};

template <std::size_t BucketCount, std::size_t BucketSize = 4u>
struct hashed_impl {

  private:
    template <class Wrapper, class Hasher>
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(Wrapper xs, Hasher hf) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            hashed_view_impl<BucketCount, BucketSize, Wrapper, Hasher>{
                std::move(xs), std::move(hf)});

    template <class Wrapper, class Hasher, class Equal>
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(
        Wrapper xs, Hasher hf, Equal&& equal) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            hashed_view_impl<BucketCount, BucketSize, Wrapper, Hasher>{
                std::move(xs), std::move(hf), std::forward<Equal>(equal)});

    static_assert(BucketCount > 0,
        "`boost::static_views::hashed<BucketCount, BucketSize>` "
        "requires BucketCount to be greater than zero.");
    static_assert(BucketCount > 0,
        "`boost::static_views::hashed<BucketCount, BucketSize>` "
        "requires BucketSize to be greater than zero.");

  public:
    // clang-format off
    template <class V, class H
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>
         && Hasher<H, typename std::remove_cv_t<
                          std::remove_reference_t<V>>::reference>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(V&& xs, H&& hf) const
    // clang-format on
    {
        return call_impl(make_wrapper(std::forward<V>(xs)),
            make_wrapper(std::forward<H>(hf)));
    }

    // clang-format off
    template <class V, class H, class Equal
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>
         && Hasher<H, typename std::remove_cv_t<
                          std::remove_reference_t<V>>::reference>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(V&& xs, H&& hf, Equal&& equal) const
    // clang-format on
    {
        return call_impl(make_wrapper(std::forward<V>(xs)),
            make_wrapper(std::forward<H>(hf)), std::forward<Equal>(equal));
    }
};

#if 0
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
#endif
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
template <std::size_t BucketCount, std::size_t BucketSize = 4u>
BOOST_STATIC_VIEWS_CONSTEXPR auto const& hashed =
    ::BOOST_STATIC_VIEWS_NAMESPACE::_static_const<
        detail::hashed_impl<BucketCount, BucketSize>>;
} // anonymous namespace
#endif

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_HASHED_HPP
