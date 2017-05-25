#ifndef BOOST_STATIC_VIEWS_HPP
#define BOOST_STATIC_VIEWS_HPP


// =============================================================================

/// \brief Base class for all the views.

/// If all views derive from this class, we have a simple way to check whether
/// a class is a view. This can be accomplished in al least two other ways.
/// There is a method of specialisation of some template for each view, i.e. 
/// the way how `std::is_floating_point` works. And finally, there is Hana's
/// tags approach.
/// The base class approach seems like the simplest, though.
struct view_base {};

template <class V>
struct is_view : std::is_base_of<view_base, V> {};

template <class V>
struct is_view<V&> : is_view<V> {};


// =============================================================================

/// \brief Sequence "type class". We can specialise it for arrays, tuples, etc.
template <class Sequence>
struct sequence_traits;

template <class Sequence>
struct sequence_traits<Sequence&> : sequence_traits<Sequence> {};

template <class T, std::size_t N>
struct sequence_traits<T[N]> {
    using type = T[N];

    static constexpr auto size() noexcept 
    { return N; }

    template <class S>
    static constexpr decltype(auto) at(S&& xs, std::size_t i) noexcept 
    { return xs[i]; }
};

template <class T, std::size_t N>
struct sequence_traits<std::array<T, N>> {
    using type = std::array<T, N>;

    static constexpr auto size() noexcept 
    { return N; }

    template <class S>
    static constexpr decltype(auto) at(S&& xs, std::size_t i) noexcept 
    { return xs[i]; }
};

template <class T, std::size_t N>
struct sequence_traits<std::array<T, N> const> : sequence_traits<std::array<T, N>> {};


// =============================================================================

namespace detail {
    template <class F, class... Args, class = decltype(std::declval<F&&>()(
        std::declval<Args&&>()...))>
    constexpr auto is_valid_impl(int) noexcept
    {
        return true;
    }

    template <class F, class... Args>
    constexpr auto is_valid_impl(...) noexcept
    {
        return false;
    }

    template <class F>
    struct is_valid_fn {
        template <class... Args>
        constexpr auto operator() (Args&&...) const noexcept
        { return is_valid_impl<F, Args&&...>(int{}); }
    };
} // end namespace detail

template <class F>
constexpr auto is_valid(F&&) noexcept
{
    return detail::is_valid_fn<F&&>{};
}

// =============================================================================

namespace detail {
    template <class T>
    struct wrapper;

    template <class T>
    struct wrapper<T&> {
        using type = T;

        constexpr wrapper(T& x) noexcept
            : _payload{ &x }
        {}
        constexpr wrapper(wrapper const& other)            = default;
        constexpr wrapper(wrapper&&)                       = default;
        constexpr wrapper& operator=(wrapper const& other) = default;
        constexpr wrapper& operator=(wrapper&&)            = default;
        constexpr auto get() const noexcept -> T& { return *_payload; }

        template <class... Args>
        constexpr auto operator()(Args&&... args) const noexcept
        {
            return (*_payload)(std::forward<Args>(args)...);
        }
    private:
        T*  _payload;
    };

    template <class T>
    struct wrapper<T&&> {
        using type = T;

        constexpr wrapper(T&& x) noexcept
            : _payload{ std::move(x) }
        {}
        constexpr wrapper(wrapper const&)            = default;
        constexpr wrapper(wrapper&&)                 = default;
        constexpr wrapper& operator=(wrapper const&) = default;
        constexpr wrapper& operator=(wrapper&&)      = default;
        constexpr auto get() const & noexcept -> T const& { return _payload; }
        constexpr auto get() &       noexcept -> T&       { return _payload; }
        constexpr auto get() &&      noexcept -> T        { return _payload; }

        template <class... Args>
        constexpr auto operator()(Args&&... args) const noexcept
        {
            return _payload(std::forward<Args>(args)...);
        }
    private:
        T  _payload;
    };
} // end namespace detail

template <class T>
constexpr auto make_wrapper(T&& x) noexcept
{
    return detail::wrapper<
        std::conditional_t< std::is_lvalue_reference<decltype(x)>::value
                          , T&
                          , T&& >
    >{std::forward<T>(x)};
}


// =============================================================================


namespace detail {

    template <std::size_t I>
    struct apply_composed_impl_from {
        template <class Tuple, class... Args>
        constexpr auto operator()(Tuple&& fs, Args&&... args) noexcept
        {
            return apply_composed_impl_from<I-1>{}(std::forward<Tuple>(fs),
                std::get<I>(fs)(std::forward<Args>(args)...));
        }
    };

    template <>
    struct apply_composed_impl_from<0> {
        template <class Tuple, class... Args>
        constexpr auto operator()(Tuple&& fs, Args&&... args) noexcept
        {
            return std::get<0>(fs)(std::forward<Args>(args)...);
        }
    };

    template <class Tuple, class... Args>
    constexpr auto apply_composed_impl(Tuple&& fs, Args&&... args) noexcept
    {
        constexpr auto size = std::tuple_size<std::remove_reference_t<Tuple>>::value;
        static_assert(size > 0, "Tuple of functions shouldn't be empty.");
        return apply_composed_impl_from<size - 1>{}(std::forward<Tuple>(fs), 
            std::forward<Args>(args)...);
    }

    template <class... Fs>
    struct compose_impl {
        constexpr compose_impl(Fs&&... fs) noexcept
            : _fs{std::move(fs)...}
        {
        }

        template <class... Args>
        constexpr auto operator()(Args&&... args) const noexcept
        {
            return apply_composed_impl(_fs, std::forward<Args>(args)...);
        }

    private:
        std::tuple<Fs...> _fs;
    };

    template <class... Fs>
    constexpr auto make_compose_impl(Fs&&... fs) noexcept
    {
        return compose_impl<Fs...>(std::forward<Fs>(fs)...);
    }

} // end namespace detail

template <class... Fs>
constexpr auto compose(Fs&&... fs) noexcept
{
    return detail::make_compose_impl(make_wrapper(std::forward<Fs>(fs))...);
}


// =============================================================================

namespace detail {
    template <class F, class... Fs>
    struct mapped_compose_impl {
        constexpr mapped_compose_impl(F&& f, Fs&&... fs) noexcept
            : _f{std::move(f)}, _fs{compose(std::move(fs)...)}
        {
        }

        template <class... Args>
        constexpr auto operator()(Args&&... args) const noexcept
        {
            return _f(_fs(args)...);
        }

    private:
        F _f;
        decltype(compose(std::declval<Fs&&>()...)) _fs;
    };

    template <class F, class... Fs>
    constexpr auto make_mapped_compose_impl(F&& f, Fs&&... fs) noexcept
    {
        return mapped_compose_impl<F, Fs...>(std::forward<F>(f),
            std::forward<Fs>(fs)...);
    }
} // end namespace detail

template <class F, class... _Fs>
constexpr auto mapped_compose(F&& f, _Fs&&... fs) noexcept
{
    return detail::make_mapped_compose_impl(make_wrapper(std::forward<F>(f)),
        make_wrapper(std::forward<_Fs>(fs))...);
}


// =============================================================================


template <class T, class Wrapper>
struct view_adaptor_base : view_base {

    constexpr view_adaptor_base(Wrapper&& wrapper) noexcept
        : _xs{ wrapper }
    {
    }

    static constexpr auto capacity() noexcept 
    { return type::capacity(); }

    constexpr auto operator() (std::size_t const i) const noexcept
    {
        return _ref()(static_cast<T const*>(this)->map(i));
    }

    constexpr decltype(auto) at(std::size_t const i) const noexcept
    {
        return _ref().at(static_cast<T const*>(this)->map(i));
    }

protected:
    using type = typename Wrapper::type;
    Wrapper  _xs;

    constexpr decltype(auto) _ref() const noexcept { return _xs.get(); }
    constexpr decltype(auto) _ref()       noexcept { return _xs.get(); }
};

// =============================================================================


namespace detail {
template <class Sequence>
struct identity_impl : view_base {
    constexpr identity_impl(Sequence& xs) noexcept
        : _xs{ xs }
    {
    }

    constexpr identity_impl(identity_impl const&) = default;
    constexpr identity_impl(identity_impl &&) = default;
    constexpr identity_impl& operator=(identity_impl const&) = default;
    constexpr identity_impl& operator=(identity_impl &&) = default;

    constexpr auto size() const noexcept
    { return sequence_traits<Sequence>::size(); }

    static constexpr auto capacity() noexcept
    { return sequence_traits<Sequence>::size(); }

    template <class Size_t>
    constexpr decltype(auto) at(Size_t i) const noexcept
    { return sequence_traits<Sequence>::at(_xs, i); }

    template <class Size_t>
    constexpr auto operator()(Size_t i) const noexcept 
    {
        return i < size()
            ? i
            : (throw std::out_of_range{ "identity_impl::operator(): " 
                "index `i` out of bounds."}, 0);
    }

private:
    Sequence& _xs;
};

struct make_identity_impl {
    template <class T>
    constexpr auto operator()(T&& xs) const noexcept
    {
        return call_impl(std::forward<T>(xs), is_view<T>{});
    }

private:
    template <class View>
    constexpr auto call_impl(View&& view, std::true_type) const noexcept
    {
        return view;
    }

    template <class Sequence>
    constexpr auto call_impl(Sequence&& sequence, std::false_type) const 
        noexcept
    {
        return identity_impl<Sequence>(std::forward<Sequence>(sequence));
    }
};
} // end namespace detail

constexpr detail::make_identity_impl identity{};


// =============================================================================


namespace detail {
template <class Wrapper>
struct subset_impl : public view_adaptor_base<subset_impl<Wrapper>, Wrapper> {
private:
    using base_type = view_adaptor_base<subset_impl<Wrapper>, Wrapper>;

public:
    constexpr subset_impl(Wrapper&& xs, std::size_t const b,
        std::size_t const e) noexcept
        : base_type{std::move(xs)}, _b{ b }, _e{ e }
    {
    }

    constexpr subset_impl(subset_impl const&) = default;
    constexpr subset_impl(subset_impl &&) = default;
    constexpr subset_impl& operator=(subset_impl const&) = default;
    constexpr subset_impl& operator=(subset_impl &&) = default;



    constexpr auto size() const noexcept { return _b - _e; }

    constexpr auto map(std::size_t const i) const noexcept
    {
        return (_b + i < _e)
            ? _b + i
            : (throw std::out_of_range{"subset_impl::operator(): index "
                "`i` is out of bounds."}, 0);
    }
private:
    std::size_t _b;
    std::size_t _e;
};

struct make_subset_impl {
    template <class View>
    constexpr auto operator()(View&& xs, std::size_t const b, 
        std::size_t const e) const noexcept
    {
        static_assert(is_view<View>::value, "make_subset_impl::operator(): "
            "View must model the View concept.");
        auto wrapper = make_wrapper(std::forward<View>(xs));
        return subset_impl<decltype(wrapper)>(std::move(wrapper), b, e);
    }
};
} // end namespace detail

constexpr detail::make_subset_impl subset{};


// =============================================================================


namespace detail {

template <std::size_t Capacity>
struct filter_initialiser_impl {
    std::size_t _is[Capacity];
    std::size_t _n;

    template <class S, class P, std::size_t... Is>
    constexpr filter_initialiser_impl(S&& xs, P&& p, 
        std::index_sequence<Is...>) noexcept
        : _is{ ((void)Is, Capacity)... }, _n{}
    {
        auto const  size = xs.size();
        std::size_t n    = 0;
        std::size_t i    = 0;

        while (i < size) {
            if (p(xs.at(i))) {
                _is[n] = i;
                ++n;
            }
            ++i;
        }
        _n = n;
    }
};

template <class Wrapper>
struct filter_impl : public view_adaptor_base<filter_impl<Wrapper>, Wrapper> {
private:
    using base_type = view_adaptor_base<filter_impl<Wrapper>, Wrapper>;

public:
    template <std::size_t Capacity, std::size_t... Is>
    constexpr filter_impl(Wrapper&& xs, filter_initialiser_impl<Capacity>&& 
        init, std::index_sequence<Is...>) noexcept
        : base_type{std::move(xs)}, _n{init._n}, _is{init._is[Is]...}
    {
        static_assert(Capacity == filter_impl::capacity(), "");
    }

    constexpr auto size() const noexcept { return _n; }

    constexpr auto map(std::size_t const i) const noexcept
    {
        return i < size()
            ? _is[i]
            : (throw std::out_of_range{"filter_impl::map(): index `i` "
                " is out of bounds."}, 0);
    }

private:
    std::size_t       _n;
    std::size_t       _is[filter_impl::capacity()];

};

struct make_filter_impl {
    template <class View, class Predicate>
    constexpr auto operator()(View&& xs, Predicate&& p) const noexcept
    {
        static_assert(is_view<View>::value, "make_subset_impl::operator(): "
            "View must model the View concept.");
        auto wrapper = make_wrapper(std::forward<View>(xs));
        detail::filter_initialiser_impl<View::capacity()> init{wrapper.get(), 
            std::forward<Predicate>(p), std::make_index_sequence<
            View::capacity()>{}};
        return detail::filter_impl<decltype(wrapper)>(std::move(wrapper),
            std::move(init), std::make_index_sequence<View::capacity()>{});
    }
};
} // end namespace detail

constexpr detail::make_filter_impl filter{};


// =============================================================================

namespace detail {

template <std::size_t N, class Predicate>
constexpr auto bubble_sort_impl(std::size_t (&xs)[N], std::size_t const size,
    Predicate&& less) noexcept
{
    struct swap_impl {
        constexpr auto operator() (std::size_t& x, std::size_t& y) const noexcept
        { auto const _temp = x; x = y; y = _temp; }
    };
    constexpr swap_impl swap{};

    for (std::size_t i = size - 1; i > 0; --i) {
        for (std::size_t j = 0; j < i; ++j) {
            if (less(xs[j+1], xs[j])) swap(xs[j+1], xs[j]);
        }
    }
}

template <std::size_t Capacity>
struct sort_initialiser_impl {
    std::size_t _is[Capacity];

    template <class View, class Predicate, std::size_t... Is>
    constexpr sort_initialiser_impl(View const& xs, Predicate&& p,
        std::index_sequence<Is...>) noexcept
        : _is{ Is... }
    {
        bubble_sort_impl(_is, xs.size(), mapped_compose(
            std::forward<Predicate>(p), view_at<View>{xs}));
    }

    template <class View>
    struct view_at {
        constexpr view_at(View const& xs) noexcept
            : _xs{xs}
        {
        }

        constexpr auto operator()(std::size_t const i) const noexcept
        {
            return _xs.at(i);
        }
    private:
        View const& _xs;
    };
};

template <class Wrapper>
struct sort_impl : public view_adaptor_base<sort_impl<Wrapper>, Wrapper> {
private:
    using base_type = view_adaptor_base<sort_impl<Wrapper>, Wrapper>;

public:
    template <std::size_t Capacity, std::size_t... Is>
    constexpr sort_impl(Wrapper&& xs, sort_initialiser_impl<Capacity>&& 
        init, std::index_sequence<Is...>) noexcept
        : base_type{std::move(xs)}, _is{init._is[Is]...}
    {
        static_assert(Capacity == sort_impl::capacity(), "");
    }

    constexpr auto size() const noexcept { return this->_ref().size(); }

    constexpr auto map(std::size_t const i) const noexcept
    {
        return i < size()
            ? _is[i]
            : (throw std::out_of_range{"sort_impl::map(): index `i` "
                " is out of bounds."}, 0);
    }

private:
    std::size_t       _is[sort_impl::capacity()];
};

struct make_sort_impl {
    template <class View, class Predicate>
    constexpr auto operator()(View&& xs, Predicate&& p) const noexcept
    {
        static_assert(is_view<View>::value, "make_subset_impl::operator(): "
            "View must model the View concept.");
        auto wrapper = make_wrapper(std::forward<View>(xs));
        detail::sort_initialiser_impl<View::capacity()> init{wrapper.get(), 
            std::forward<Predicate>(p), std::make_index_sequence<
            View::capacity()>{}};
        return detail::sort_impl<decltype(wrapper)>(std::move(wrapper),
            std::move(init), std::make_index_sequence<View::capacity()>{});
    }
};
} // end namespace detail

constexpr detail::make_sort_impl sort{};


#endif // BOOST_STATIC_VIEWS_HPP
