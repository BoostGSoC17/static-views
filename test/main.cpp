#include <iostream>
#include <stdexcept>
#include <utility>
#include <functional>
#include <tuple>
#include <array>
#include <initializer_list>


#include "../include/boost/static_map/static_views.hpp"

using namespace boost::static_views;

template <std::size_t N>
struct is_a_impl {
    template <class T>
    constexpr auto operator()(T&& x) const noexcept
    {
        return x == N;
    }
};

template <std::size_t N>
constexpr auto is_a = is_a_impl<N>{};

template <class T>
constexpr auto copy(T x) noexcept
{
    return T{x};
}


int count = 0;

template <class T, std::size_t N>
struct Foo {
    int data[N];

    constexpr Foo(std::initializer_list<T> il) noexcept
        : Foo(il, std::make_index_sequence<N>{})
    {
    }

    template <std::size_t... Is>
    constexpr Foo(std::initializer_list<T> il,
        std::index_sequence<Is...>) noexcept
        : data{((void)Is, 0)...}
    {
        for (std::size_t i = 0; i < il.size(); ++i)
            data[i] = il.begin()[i];
    }
};


template <int N>
struct less_than_impl {
    template <class X>
    constexpr auto operator()(X&& x) const noexcept
    {
        return x < N;
    }
};

template <int N>
constexpr auto less_than = less_than_impl<N>{};


int main()
{
    {
        constexpr int xs[] = {8, -7, 10, -1, 8, -2, 5};

        // The example from above
        constexpr auto m = xs | drop(std::size(xs) / 2) 
                              | min();
        static_assert(m == -2, "");
    }

    {
#ifndef BOOST_GCC
        constexpr std::array<int, 7> xs {{8, -7, 10, -1, 8, -2, 5}};
        static_assert(
            compose( [](auto&& v) { return v.size(); }
                   , filter([](auto&& x) { return x % 2 == 0; })
                   , raw_view
                   )(xs) == 4, "");
#endif

    }

    constexpr static int xs[] = {1, 2, 3, 4, 5};
    constexpr static std::array<int, 6> as{{3, -4, 2, -8, -10, 2}};

    // adopting simple sequences
    constexpr auto ys  = raw_view(xs);
    constexpr auto bs  = raw_view(as);

    // testing for concepts
    static_assert(is_view<decltype(ys)>::value, "");
    static_assert(!is_view<decltype(xs)>::value, "");
    static_assert(is_view<decltype(raw_view(ys))>::value, "");

    // querying for size and elements
    static_assert(ys.capacity() == 5, "");
    static_assert(ys.size() == 5, "");
    static_assert(ys.at(2) == 3, "");
    // compile-time error:
    //     static_assert(ys.at(6) == 3, "");

    // DROP
    {
        static_assert((ys | drop(3)).at(0) == 4, "");
        constexpr auto foo_variant_1 = drop(3)(copy(ys));
        constexpr auto foo_variant_2 = copy(ys) | drop(3);
        constexpr auto foo_variant_3 = copy(foo_variant_2) | drop(1);

        static_assert(foo_variant_1.at(0) == 4, "");
        static_assert(foo_variant_2.size() == 2, "");
        static_assert(foo_variant_3.at(0) == 5, "");
    }

    // DROP_WHILE
    {
        constexpr auto not_three = not_fn(is_a<3>);
        static_assert((ys | drop_while(not_three)).at(0) == 3, "");
        constexpr auto foo_variant_1 = drop_while(not_three)(copy(ys));
        constexpr auto foo_variant_2 = copy(ys) | drop_while(not_three);
        constexpr auto foo_variant_3 = copy(foo_variant_2) | drop(1);
         
        static_assert(foo_variant_1.at(0) == 3, "");
        static_assert(foo_variant_3.at(0) == 4, "");
        static_assert(foo_variant_2.size() == 3, "");
    }

    // REVERSE
    {
        constexpr auto not_three = not_fn(is_a<3>);
        static_assert((ys | reverse).at(0) == 5, "");
        constexpr auto foo = copy(ys) | drop_while(not_three) | reverse;
         
        static_assert(foo.at(0) == 5, "");
        static_assert(foo.size() == 3, "");
    }

    // TAKE
    {
        constexpr auto not_three = not_fn(is_a<3>);
        static_assert((ys | take(2)).at(1) == 2, "");
        constexpr auto foo = copy(ys) | drop_while(not_three) 
                                      | reverse 
                                      | take(1);
         
        static_assert(foo.at(0) == 5, "");
        static_assert(foo.size() == 1, "");
    }

    // SLICE
    {
        static_assert((xs | slice(2, 5)).at(1) == 4, "");
        static_assert((xs | slice(2, 5) | slice(1, 3)).at(0) == 4, "");
        constexpr auto foo = xs | slice(1, 3);
        constexpr auto bar = xs | take(3) | drop(1);

        static_assert(foo.at(0) == bar.at(0), "");
        static_assert(foo.at(1) == bar.at(1), "");
        static_assert(foo.size() == bar.size(), "");


    }

    {
        // constexpr std::initializer_list<int> cs = {1, 3, 5, 7, 9};
        // constexpr auto s = cs.size();
        // constexpr auto ds = Foo<int, s>{ cs };
        /*
        std::vector<int> vi{1,2,3,4,5,6,7,8,9,10};
        auto rng = vi | filter([](int i){return i % 2 == 0;})
                      | transform([](int i){return std::to_string(i);});
        */
        // rng == {"2","4","6","8","10"};
    }

    // FILTER
    {
        static_assert(!less_than<3>(4), "");
        static_assert((xs | filter(less_than<3>)).size() 
            == 2, "");
        static_assert((xs | remove_if(less_than<3>)).size() 
            == 3, "");
    }
    // std::experimental::not_fn :D

    {
        static_assert(compose(filter(less_than<3>), drop(1))(ys).at(0) == 2, "");

    }

    {
        // alternative syntax, for people who hate pipes
        static_assert(sort(std::less<void>{})(bs).at(0) == -10, "");
        static_assert(slice(1, 6)(raw_view(as)).size() == 5, "");
        static_assert(sort(std::less<void>{})(slice(1, 6)(raw_view(as))).at(2) == -4, "");

    }
    /*
    // also hashed view is supported
    // it's not very useful without any algorithms though
    // 
    // choose some stupid hash function:
    static_assert((as | hashed<20>([](auto&& x) { return 1 << (x > 0 ? x : (-x)); }))(8) == 0);

    // I still have no idea as to why this doesn't work:
    constexpr auto hashed_as = as | hashed<20>([](auto&& x) { return 1 << (x > 0 ? x : (-x)); });
    static_assert(hashed_as(8) == 0);
    // while this does:
    constexpr auto reversed_ys = ys | reverse;
    */
}
