#ifndef BOOST_STATIC_VIEWS_PARSE_HPP
#define BOOST_STATIC_VIEWS_PARSE_HPP

#include <iostream>
#include "static_views.hpp"

using namespace boost::static_views;

// Converts a type to its "code"
template <class> struct type_to_code;

template <> struct type_to_code<int>   { static constexpr char value = 'd'; };
template <> struct type_to_code<float> { static constexpr char value = 'f'; };

// Format specification
struct format_spec { char code; };


// Parses the STR part, returns the length
template <class S>
constexpr auto next_length(S&& xs) noexcept
{
    return (xs | take_while([](auto&& c) { return c != '%'; })).size();
}

// Parses the FMT part, returns format_spec
template <class S>
constexpr auto parse_format(S&& xs) noexcept -> format_spec
{
    if (xs.size() < 2 || xs[0] != '%') throw std::runtime_error{""};
        return { xs[1] };
}

// A vector with fixed-size underlying storage.
template <class T, std::size_t N>
struct static_vector {
    std::array<T, N> data;
    std::size_t      size;

    constexpr static_vector() noexcept
        : static_vector{std::make_index_sequence<N>{}}
    {
    }

    constexpr auto push_back(T x) noexcept
    {
        data[size] = x;
        ++size;
    }

private:
    template <std::size_t... Is>
    constexpr static_vector(std::index_sequence<Is...>) noexcept
        : data{ ((void)Is, T{})... }
        , size{ 0 }
    {
    }
};


// The parsing function
template <std::size_t N>
constexpr auto parse(char const (&xs)[N])
{
    static_vector<std::size_t, N / 2 + 1> begins;
    static_vector<std::size_t, N / 2 + 1> sizes;
    static_vector<format_spec, N / 2>     fmts;

    std::size_t b = 0; 
    std::size_t n = next_length(xs | drop(b));
    begins.push_back(b);
    sizes.push_back(n);

    b = b + n;
    while (b < N) {
        // read the FMT part
        fmts.push_back(parse_format(xs | drop(b)));
        b = b + 2;
        // read the STR part
        n = next_length(xs | drop(b));
        begins.push_back(b);
        sizes.push_back(n);
        b = b + n;
    }

    // for simplicity return just as a tuple
    return std::make_tuple(begins, sizes, fmts);
}

// Given a pack of types Args... and a list of format specifiers fmts,
// returns whether Args match the types in fmts.
template <std::size_t N>
constexpr auto verify(static_vector<format_spec, N> const& fmts,
    std::size_t const i) -> bool
{
    if (fmts.data[i].code == '%')
        return verify(fmts, i + 1);
    return i == fmts.size;
}

template <class Arg, class... Args, std::size_t N>
constexpr auto verify(static_vector<format_spec, N> const& fmts,
    std::size_t const i) -> bool
{
    if (fmts.data[i].code == '%')
        return verify<Arg, Args...>(fmts, i + 1);
    return fmts.data[i].code == type_to_code<Arg>::value
        && verify<Args...>(fmts, i + 1);
}


// Print to std::cout
template <std::size_t N>
auto print_impl(std::size_t i, char const* s, 
    static_vector<std::size_t, N+1> const& begins,
    static_vector<std::size_t, N+1> const& sizes,
    static_vector<format_spec, N> const& fmts) -> void
{
    // No arguments, we're done
    return;
}

// `i` points to the "current" element in fmts
template <std::size_t N, class Arg, class... Args>
auto print_impl(std::size_t i, char const* s,
    static_vector<std::size_t, N+1> const& begins,
    static_vector<std::size_t, N+1> const& sizes,
    static_vector<format_spec, N> const& fmts,
    Arg&& arg, Args&&... args) -> void
{
    if (fmts.data[i].code == '%') {
        std::cout << '%';
        std::cout.write(s + begins.data[i+1], sizes.data[i+1]);
        print_impl(i+1, s, begins, sizes, fmts, std::forward<Arg>(arg),
            std::forward<Args>(args)...);
    }
    else {
        std::cout << std::forward<Arg>(arg);
        std::cout.write(s + begins.data[i+1], sizes.data[i+1]);
        print_impl(i+1, s, begins, sizes, fmts, std::forward<Args>(args)...);
    }
}


#define FORMAT(str)                                                 \
    [] {                                                            \
        constexpr auto tpl  = parse(str);                           \
        constexpr auto fmts = std::get<2>(tpl);                     \
        constexpr auto begins = std::get<0>(tpl);                   \
        constexpr auto sizes = std::get<1>(tpl);                    \
        return [begins, sizes, fmts](auto&&... args) {              \
            static_assert(verify<std::decay_t<decltype(args)>...>(  \
                fmts, 0), "");                                      \
            constexpr char const s[] = str;                         \
            std::cout.write(s + begins.data[0], sizes.data[0]);     \
            print_impl(0, s, begins, sizes, fmts, args...);         \
        };                                                          \
    }()



#endif // BOOST_STATIC_VIEWS_PARSE_HPP
