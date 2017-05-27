#ifndef BOOST_STATIC_VIEWS_HPP
#define BOOST_STATIC_VIEWS_HPP

#include <utility>
#include <functional>
#include <tuple>
#include <array>
#include <stdexcept>

/// \file include/boost/static_map/static_views.hpp
/// \brief A prototype for the Static Views library.
/// \detail
///
/// Concepts
/// --------
/// __Sequence__: is any type for which there exists a specialisation of
/// #sequence_traits.
///
/// __View__: provides indexed access into the underlying _View_. Yes, this
/// is recursive. The chain always terminates at the `raw_view` which provides
/// access to the underlying _Sequence_. Views thus act like proxies through 
/// which we access a sequence. We require a view to have the following 
/// operations:
/// * `static constexpr auto capacity() noexcept -> std::size_t`. 
///
///   This operation returns an upper bound on the number of elements the
///   underlying sequence/view has. Notice that the function is marked
///   `static`. This allows for creation of views that require some 
///   additional storage, like `sort`.
/// * `constexpr auto size() noexcept -> std::size_t`.
///
///   Returns the actual number of elements in the underlying view.
/// * `constexpr auto operator(std::size_t) const -> std::size_t`.
///
///   Defines the behavior of the view. Given an index `i` in `[0,..., size())`
///   returns the corresponding index in the underlying view. Thus if
///   `size()` is unchanged, views can be seen as permutations.
/// * `constexpr auto at(std::size_t)`.
///
///    `at(i)` returns `i`'th element of the sequence. Here, `i` is, again,
///    in `[0,...,size())`.
///
/// __Pipe__: is a function that, given a sequence, returns a view of it.
///

// =============================================================================


/// \brief Base class for all the views.
struct view_base {};

/// \brief Check whether `V` models the View concept.
template <class V>
struct is_view : std::is_base_of<view_base, V> {};

/// \cond
template <class V>
struct is_view<V&> : is_view<V> {};
/// \endcond


// =============================================================================

/// \brief Base class for all the pipes.
struct pipe_base {};

/// \brief Check whether `V` models the Pipe concept.
template <class V>
struct is_pipe : std::is_base_of<pipe_base, V> {};

/// \cond
template <class V>
struct is_pipe<V&> : is_pipe<V> {};
/// \endcond


// =============================================================================

/// \brief Sequence _type class_.

/// This is a common interface through which sequences are accessed. To 
/// specialise it for some type, one needs to provide the following operations:
/// * `static constexpr auto size() noexcept` --- returns the number of
///   elements is the sequence.
/// * `template <class S> static constexpr decltype(auto) at(S&& xs, 
///    std::size_t i) noexcept`. I.e. given a sequence `xs`, return its `i`th 
///    element.
///
/// Currently, two specialisations are provided: for C-style arrays and
/// `std::array`.
template <class Sequence>
struct sequence_traits;

/// \cond
template <class Sequence>
struct sequence_traits<Sequence&> : sequence_traits<Sequence> {};
/// \endcond

/// \cond
template <class T, std::size_t N>
struct sequence_traits<T[N]> {
    using type = T[N];

    static constexpr auto size() noexcept 
    { return N; }

    template <class S>
    static constexpr decltype(auto) at(S&& xs, std::size_t i) noexcept 
    { return xs[i]; }
};
/// \endcond

/// \cond
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
struct sequence_traits<std::array<T, N> const> 
    : sequence_traits<std::array<T, N>> {};
/// \endcond


// =============================================================================

#if false
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
#endif

// =============================================================================

namespace detail {
    template <class T> struct wrapper;

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
        constexpr operator T&() const noexcept { return get(); }

        template <class... Args>
        constexpr decltype(auto) operator()(Args&&... args) const noexcept
        {
            return (*_payload)(std::forward<Args>(args)...);
        }
    private:
        T*  _payload;
    };

    template <class T>
    struct wrapper<T&&> {
        using type = T;

        constexpr wrapper(T&& x) 
            noexcept(std::is_nothrow_move_constructible<T>::value)
            : _payload{ std::move(x) }
        {}

        constexpr wrapper(wrapper const&)            = default;
        constexpr wrapper(wrapper&&)                 = default;
        constexpr wrapper& operator=(wrapper const&) = default;
        constexpr wrapper& operator=(wrapper&&)      = default;

        constexpr auto get() const& noexcept -> T const& { return _payload; }
        constexpr auto get() &      noexcept -> T&       { return _payload; }
        constexpr auto get() &&     noexcept -> T        
        { return std::move(_payload); }

        constexpr operator T() const noexcept { return get(); }
        constexpr operator T()       noexcept { return get(); }

        template <class... Args>
        constexpr auto operator()(Args&&... args) const noexcept
        {
            return _payload(std::forward<Args>(args)...);
        }
    private:
        T  _payload;
    };
} // end namespace detail


/// \brief Almost `std::ref()` except that it's constexpr and the created
/// wrapper may also hold the object by value.

/// Idea is that if we get an lvalue of T, we store an lvalue, because
/// why bother copying? If, however, an rvalue is passed, we move-construct
/// an object, because otherwise we'll easily end up with a dangling reference.
/// \snippet include/boost/static_map/static_views.hpp make_wrapper implementation
///
/// The returned object provides almost the same interface as `std::ref()`.
/// If a reference is stored, then wrapper is noexcept copy/move 
/// constructible/assignable. If, however, an object is stores, wrapper
/// acts exaclty like T. Wrapper also prives `get` function to access the
/// stored object, and implicit conversion to either `T&` or `T`.
///
/// __P.S.__: _Does this sound right?_
///
/// [make_wrapper implementation]
template <class T>
constexpr auto make_wrapper(T&& x) noexcept
{
    return detail::wrapper<
        std::conditional_t< std::is_lvalue_reference<decltype(x)>::value
                          , T&
                          , T&& >
    >{std::forward<T>(x)};
}
/// [make_wrapper implementation]


// =============================================================================

namespace detail {
    template <class Maker, class... Args>
    struct pipe_impl : pipe_base {
        constexpr pipe_impl(Maker make, std::tuple<Args...>&& args) noexcept
            : _make{std::move(make)}, _args{std::move(args)}
        {
        }

        template <class View> 
        constexpr auto operator()(View&& xs) const noexcept
        {
            static_assert(is_view<View>::value, "detail::pipe_impl::operator() "
                "`View` must model the View concept.");
            return call_impl(std::forward<View>(xs), std::make_index_sequence<
                sizeof...(Args)>{});
        }

        template <class View> 
        constexpr auto operator()(View&& xs) noexcept
        {
            static_assert(is_view<View>::value, "detail::pipe_impl::operator() "
                "`View` must model the View concept.");
            return call_impl(std::forward<View>(xs), std::make_index_sequence<
                sizeof...(Args)>{});
        }

    private:
        Maker                _make;
        std::tuple<Args...>  _args;

        template <class View, std::size_t... Is>
        constexpr auto call_impl(View&& xs, std::index_sequence<Is...>) 
            && noexcept
        {
            return _make(std::move(std::get<Is>(_args))..., 
                make_wrapper(std::forward<View>(xs)));
        }

        template <class View, std::size_t... Is>
        constexpr auto call_impl(View&& xs, std::index_sequence<Is...>) 
            const& noexcept
        {
            return _make(std::get<Is>(_args)..., 
                make_wrapper(std::forward<View>(xs)));
        }
    };

    template <class Maker, class... Args>
    constexpr auto make_pipe_impl(Maker&& make, Args&&... args) noexcept
    {
        return pipe_impl<Maker, Args...>{std::forward<Maker>(make), 
            std::make_tuple(std::forward<Args>(args)...)};
    }

    template <class Maker>
    struct pipe_variable_impl {
        constexpr pipe_variable_impl(Maker&& make) noexcept
            : _make{std::move(make)}
        {
        }

        template <class... Args>
        constexpr auto operator()(Args&&... args) const& noexcept
        {
            return make_pipe_impl(_make, make_wrapper(
                std::forward<Args>(args))...);
        }

        template <class... Args>
        constexpr auto operator()(Args&&... args) && noexcept
        {
            return make_pipe_impl(std::move(_make), make_wrapper(
                std::forward<Args>(args))...);
        }
    private:
        Maker _make;
    };
} // end namespace detail

/// \brief Creates a pipe.
template <class Maker>
constexpr auto make_pipe_variable(Maker&& make) noexcept 
{
    return detail::pipe_variable_impl<decltype(make_wrapper(
        std::forward<Maker>(make)))>{make_wrapper(std::forward<Maker>(make))};
}


// =============================================================================


namespace detail {
    template <class Predicate>
    struct not_fn_impl {
        constexpr not_fn_impl(Predicate&& p) noexcept
            : _p{std::move(p)}
        {
        }
        
        template <class... Args>
        constexpr auto operator()(Args&&... args) const noexcept
        {
            return !_p(std::forward<Args>(args)...);
        }
    private:
        Predicate  _p;
    };

    struct make_not_fn_impl {
        template <class Predicate>
        constexpr auto operator()(Predicate&& p) const noexcept
        {
            auto wrapper = make_wrapper(std::forward<Predicate>(p));
            return not_fn_impl<decltype(wrapper)>{std::move(wrapper)};
        }
    };
} // end namespace detail

/// `std::experimental::not_fn` except that it's constexpr.
constexpr auto not_fn = detail::make_not_fn_impl{};


// =============================================================================


namespace detail {
    template <std::size_t I>
    struct apply_composed_impl_from {
        template <class Tuple, class... Args>
        constexpr auto operator()(Tuple&& fs, Args&&... args) const noexcept
        {
            return apply_composed_impl_from<I-1>{}(std::forward<Tuple>(fs),
                std::get<I>(fs)(std::forward<Args>(args)...));
        }
    };

    template <>
    struct apply_composed_impl_from<0> {
        template <class Tuple, class... Args>
        constexpr auto operator()(Tuple&& fs, Args&&... args) const noexcept
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


/// \brief Syntactic sugar for function composition.

/// In C++17 we can create composed functions on the fly using lambdas, but
/// in C++14 this comes quite handy.
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

/// \brief \f$\approx\f$ _fmap_ on arguments.

/// Say, we have
/// \f[ f : (x_1, x_2, \dots) \mapsto y \f]
/// Then `mapped_compose`\f$(f, g_1, g_2, \dots)\f$ creates a function
/// \f[ F : (x_1, x_2, \dots, x_N) \mapsto f((g_1 \circ g_2 \circ \dots)(x_1), 
/// (g_1 \circ g_2 \circ \dots)(x_2),\dots)) \f]
template <class F, class... _Fs>
constexpr auto mapped_compose(F&& f, _Fs&&... fs) noexcept
{
    return detail::make_mapped_compose_impl(make_wrapper(std::forward<F>(f)),
        make_wrapper(std::forward<_Fs>(fs))...);
}

// =============================================================================

struct view_adaptor_core_access {
    template <class T, class W> friend class view_adaptor_base;

    template <class View>
    static constexpr decltype(auto) map(View const& xs, std::size_t const i) 
        noexcept
    {
        return xs.map(i);
    }
};

template <class T, class Wrapper>
struct view_adaptor_base : view_base {

    constexpr view_adaptor_base(Wrapper&& wrapper) noexcept
        : _xs{wrapper}
    {
    }

    constexpr view_adaptor_base(view_adaptor_base const&)            = default;
    constexpr view_adaptor_base(view_adaptor_base &&)                = default;
    constexpr view_adaptor_base& operator=(view_adaptor_base const&) = default;
    constexpr view_adaptor_base& operator=(view_adaptor_base &&)     = default;

    static constexpr auto capacity() noexcept 
    { return type::capacity(); }

    constexpr auto size() const noexcept { return _ref().size(); }

    constexpr auto operator() (std::size_t const i) const noexcept
    {
        return _ref()(view_adaptor_core_access::map(derived(), i));
    }

    constexpr decltype(auto) at(std::size_t const i) const noexcept
    {
        return _ref().at(view_adaptor_core_access::map(derived(), i));
    }

protected:
    constexpr decltype(auto) _ref() const noexcept { return _xs.get(); }
    constexpr decltype(auto) _ref()       noexcept { return _xs.get(); }

private:
    using type = typename Wrapper::type;
    Wrapper  _xs;

    constexpr auto derived() const noexcept -> T const&
    {
        return *static_cast<T const*>(this);
    }
};


// =============================================================================


namespace detail {
    template <class Sequence>
    struct raw_view_impl : view_base {
        constexpr raw_view_impl(Sequence& xs) noexcept
            : _xs{xs}
        {
        }

        constexpr raw_view_impl(raw_view_impl const&)            = default;
        constexpr raw_view_impl(raw_view_impl &&)                = default;
        constexpr raw_view_impl& operator=(raw_view_impl const&) = default;
        constexpr raw_view_impl& operator=(raw_view_impl &&)     = default;

        static constexpr auto capacity() noexcept
        { return sequence_traits<Sequence>::size(); }

        constexpr auto size() const noexcept
        { return sequence_traits<Sequence>::size(); }

        constexpr decltype(auto) at(std::size_t const i) const noexcept
        { return (i < size())
            ? sequence_traits<Sequence>::at(_xs, i)
            : (throw std::out_of_range{"detail::raw_view_impl::at() index `i`"
                " out of bounds."}, sequence_traits<Sequence>::at(_xs, i)); 
        }

        constexpr auto operator()(std::size_t const i) const noexcept 
        {
            return i;
            // return i < size()
            //     ? i
            //     : (throw std::out_of_range{"detail::raw_view_impl::operator(): " 
            //         "index `i` out of bounds."}, 0);
        }

    private:
        Sequence& _xs;
    };

    struct make_raw_view {
        template <class Sequence>
        constexpr auto operator()(Sequence&& sequence) const noexcept
        {
            return raw_view_impl<Sequence>(std::forward<Sequence>(sequence));
        }
    };
} // end namespace detail

/// \brief Raw view of the sequence.
constexpr auto raw_view = detail::make_raw_view{};


// =============================================================================


/// \brief Piping.
template <class View, class Pipe, class = std::enable_if_t<
    is_view<View>::value && is_pipe<Pipe>::value >>
constexpr decltype(auto) operator|(View&& xs, Pipe&& ys) noexcept
{
    return ys(std::forward<View>(xs));
}

/// \overload
template <class Sequence, class Pipe, class = std::enable_if_t<
    !is_view<Sequence>::value && is_pipe<Pipe>::value >>
constexpr decltype(auto) operator|(Sequence& xs, Pipe&& ys) noexcept
{
    return ys(raw_view(xs));
}


// =============================================================================


namespace detail {
    template <class View>
    struct drop_impl : view_adaptor_base<drop_impl<View>, View> {

        friend class ::view_adaptor_core_access;

        constexpr drop_impl(View&& xs, std::size_t const b) noexcept
            : base_type{std::move(xs)}, _b{b}
        {
        }

        constexpr auto size() const noexcept 
        { 
            return this->_ref().size() - _b; 
        }

    private:
        std::size_t _b;
        using base_type = view_adaptor_base<drop_impl<View>, View>;

        constexpr auto map(std::size_t const i) const noexcept
        {
            return i < size()
                ? _b + i
                : (throw std::out_of_range{"detail::drop_impl::operator(): "
                    "index `i` is out of bounds."}, 0);
        }
    };

    struct make_drop_impl {
        template <class View>
        constexpr auto operator()(std::size_t const b, 
            View&& xs) const noexcept
        {
            return drop_impl<View>(std::forward<View>(xs), 
                std::min(b, xs.get().size()));
        }
    };
} // end namespace detail

/// \brief Given an integral count, return a range consisting of all but the 
/// first count elements from the source range, or an empty range if it has 
/// fewer elements.
constexpr auto drop = make_pipe_variable(detail::make_drop_impl{});


// =============================================================================


namespace detail {
    template <class View, class Predicate>
    constexpr auto find_first_i(View&& xs, Predicate&& p) noexcept
    {
        std::size_t i = 0;
        while (i < xs.size() && !p(xs.at(i))) {
            ++i;
        }
        return i;
    }

    struct make_drop_while_impl {
        template <class View, class Predicate>
        constexpr auto operator()(Predicate&& p, 
            View&& xs) const noexcept
        {
            auto const b = find_first_i(xs.get(), 
                not_fn(std::forward<Predicate>(p)));
            return std::forward<View>(xs).get() | drop(b);
        }
    };
} // end namespace detail

/// \brief Remove elements from the front of a range that satisfy a unary 
/// predicate.
constexpr auto drop_while = make_pipe_variable(detail::make_drop_while_impl{});


// =============================================================================

namespace detail {
    template <class View>
    struct reverse_impl : view_adaptor_base<reverse_impl<View>, View> {

        friend class ::view_adaptor_core_access;

        constexpr reverse_impl(View&& xs) noexcept
            : base_type{std::move(xs)}
        {
        }

    private:
        using base_type = view_adaptor_base<reverse_impl<View>, View>;

        constexpr auto map(std::size_t const i) const noexcept
        {
            return i < this->size()
                ? this->size() - 1 - i
                : (throw std::out_of_range{"detail::rewverse_impl::operator(): "
                    "index `i` is out of bounds."}, 0);
        }
    };

    struct make_reverse_impl {
        template <class View>
        constexpr auto operator()(View&& xs) const noexcept
        {
            return reverse_impl<View>(std::forward<View>(xs));
        }
    };
} // end namespace detail

/// \brief Create a new range that traverses the source range in reverse order.
constexpr auto reverse = make_pipe_variable(detail::make_reverse_impl{})();

// =============================================================================

namespace detail {
    struct make_take_impl {
        template <class View>
        constexpr auto operator()(std::size_t const n, View&& xs) const 
            noexcept
        {
            auto const size = xs.get().size();
            return reverse(std::forward<View>(xs).get())
                | drop(size - std::min(n, size))
                | reverse;
        }
    };
} // end namespace detail

/// \brief Given a source range and an integral count, return a range 
/// consisting of the first count elements from the source range, or the 
/// complete range if it has fewer elements.
constexpr auto take = make_pipe_variable(detail::make_take_impl{});

// =============================================================================

namespace detail {
    struct make_slice_impl {
        template <class View>
        constexpr auto operator()(std::size_t const b, 
            std::size_t const e, View&& xs) const noexcept
        {
            return std::forward<View>(xs).get() | take(e) | drop(b);
        }
    };
} // end namespace detail

/// \brief Give a source range a lower bound (inclusive) and an upper bound 
/// (exclusive), create a new range that begins and ends at the specified 
/// offsets. 
constexpr auto slice = make_pipe_variable(detail::make_slice_impl{});


// =============================================================================


namespace detail {
    template <std::size_t... Is>
    struct filter_init_impl {
        std::size_t _is[sizeof...(Is)];
        std::size_t _n;

        template <class S, class P>
        constexpr filter_init_impl(S&& xs, P&& p) noexcept
            : _is{ ((void)Is, sizeof...(Is))... }, _n{}
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

    template <class S, class P, std::size_t... Is>
    constexpr auto _make_filter_init_impl(S&& xs, P&& p,
        std::index_sequence<Is...>) noexcept
    {
        return filter_init_impl<Is...>(std::forward<S>(xs), 
            std::forward<P>(p));
    }

    template <class S, class P>
    constexpr auto make_filter_init_impl(S&& xs, P&& p) noexcept
    {
        constexpr auto capacity = std::remove_reference_t<S>::capacity();
        return _make_filter_init_impl(std::forward<S>(xs), 
            std::forward<P>(p), std::make_index_sequence<capacity>{});
    }

    template <class V>
    struct filter_impl : view_adaptor_base<filter_impl<V>, V> {
        friend ::view_adaptor_core_access;

        template <std::size_t... Is>
        constexpr filter_impl(V&& xs, filter_init_impl<Is...>&& init) 
            noexcept
            : base_type{std::move(xs)}, _n{init._n}, _is{init._is[Is]...}
        {
        }

        constexpr auto size() const noexcept { return _n; }

    private:
        std::size_t       _n;
        std::size_t       _is[filter_impl::capacity()];
        using base_type = view_adaptor_base<filter_impl<V>, V>;

        constexpr auto map(std::size_t const i) const noexcept
        {
            return i < size()
                ? _is[i]
                : (throw std::out_of_range{"filter_impl::map(): index `i` "
                    " is out of bounds."}, 0);
        }
    };

    struct make_filter_impl {
        template <class View, class Predicate>
        constexpr auto operator()(Predicate&& p, View&& xs) const noexcept
        {
            auto init = detail::make_filter_init_impl(xs.get(), 
                std::forward<Predicate>(p));
            return detail::filter_impl<View>(std::forward<View>(xs),
                std::move(init));
        }
    };
} // end namespace detail

/// \brief Given a source range and a unary predicate, filter the elements 
/// that satisfy the predicate. (For users of Boost.Range, this is like the 
/// filter adaptor.)
constexpr auto filter = make_pipe_variable(detail::make_filter_impl{});


// =============================================================================


namespace detail {

    template <std::size_t N, class Predicate>
    constexpr auto bubble_sort_impl(std::size_t (&xs)[N], std::size_t const size,
        Predicate&& less) noexcept
    {
        struct swap_impl {
            constexpr auto operator() (std::size_t& x, std::size_t& y) const 
                noexcept
            { auto const _temp = x; x = y; y = _temp; }
        };
        constexpr swap_impl swap{};

        for (std::size_t i = size - 1; i > 0; --i) {
            for (std::size_t j = 0; j < i; ++j) {
                if (less(xs[j+1], xs[j])) swap(xs[j+1], xs[j]);
            }
        }
    }

    template <std::size_t... Is>
    struct sort_init_impl {
        std::size_t _is[sizeof...(Is)];

        template <class View, class Predicate>
        constexpr sort_init_impl(View const& xs, Predicate&& p) noexcept
            : _is{ Is... }
        {
            bubble_sort_impl(_is, xs.size(), mapped_compose(
                std::forward<Predicate>(p), view_at<View>{xs}));
        }

        template <class View>
        struct view_at {
            View const& _xs;

            constexpr auto operator()(std::size_t const i) const noexcept
            {
                return _xs.at(i);
            }
        };
    };

    template <class V, class P, std::size_t... Is>
    constexpr auto _make_sort_init_impl(V&& xs, P&& p, 
        std::index_sequence<Is...>) noexcept
    {
        return sort_init_impl<Is...>{std::forward<V>(xs), std::forward<P>(p)};
    }

    template <class V, class P>
    constexpr auto make_sort_init_impl(V&& xs, P&& p) noexcept
    {
        constexpr auto capacity = std::remove_reference_t<V>::capacity();
        return _make_sort_init_impl(std::forward<V>(xs), std::forward<P>(p),
            std::make_index_sequence<capacity>{});
    }

    template <class V>
    struct sort_impl : view_adaptor_base<sort_impl<V>, V> {
        friend class ::view_adaptor_core_access;

        template <std::size_t... Is>
        constexpr sort_impl(V&& xs, sort_init_impl<Is...>&& init) noexcept
            : base_type{std::move(xs)}, _is{init._is[Is]...}
        {
        }

        constexpr auto size() const noexcept { return this->_ref().size(); }


    private:
        std::size_t _is[sort_impl::capacity()];
        using base_type = view_adaptor_base<sort_impl<V>, V>;

        constexpr auto map(std::size_t const i) const noexcept
        {
            return i < size()
                ? _is[i]
                : (throw std::out_of_range{"sort_impl::map(): index `i` "
                    " is out of bounds."}, 0);
        }
    };

    struct make_sort_impl {
        template <class View, class Predicate>
        constexpr auto operator()(Predicate&& p, View&& xs) const noexcept
        {
            auto init = detail::make_sort_init_impl(xs.get(), 
                std::forward<Predicate>(p));
            return detail::sort_impl<View>(std::forward<View>(xs),
                std::move(init));
        }
    };
} // end namespace detail

/// \brief Given a source range and a binary predicate, returns a sorted
/// view of the source range.
constexpr auto sort = make_pipe_variable(detail::make_sort_impl{});


// =============================================================================

namespace detail {

    template <std::size_t... Is>
    struct hashed_init_impl {
        static constexpr std::size_t capacity = sizeof...(Is);
        std::size_t _is[capacity];

        template <class View, class Hasher>
        constexpr hashed_init_impl(View const& xs, Hasher&& h) noexcept
            : _is{ ((void)Is, capacity)... }
        {
            auto size = xs.size();
            for (std::size_t i = 0; i < size; ++i) {
                insert(i, h(xs.at(i)) % capacity);
            }
        }

    private:
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

    template <std::size_t... Is>
    constexpr std::size_t hashed_init_impl<Is...>::capacity;

    template <class V, class H, std::size_t... Is>
    constexpr auto _make_hashed_init_impl(V&& xs, H&& h, 
        std::index_sequence<Is...>) noexcept
    {
        return hashed_init_impl<Is...>{std::forward<V>(xs), std::forward<H>(h)};
    }

    template <std::size_t Capacity, class V, class H>
    constexpr auto make_hashed_init_impl(V&& xs, H&& h) noexcept
    {
        return Capacity >= xs.size()
            ? _make_hashed_init_impl(std::forward<V>(xs), std::forward<H>(h),
                std::make_index_sequence<Capacity>{})
            : (throw std::out_of_range{"detail::make_hashed_init_impl() "
                "`Capacity` too small."}, _make_hashed_init_impl(std::forward<V>(xs), 
                std::forward<H>(h), std::make_index_sequence<Capacity>{}));
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


// =============================================================================




#endif // BOOST_STATIC_VIEWS_HPP
