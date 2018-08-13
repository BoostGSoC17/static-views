#include <functional>
#include <iostream>

#include <boost/static_views/detail/invoke.hpp>
namespace sv = boost::static_views;

struct Foo {
    constexpr Foo(int num) noexcept : num_(num) {}
    constexpr int add(int i) const noexcept { return num_ + i; }
    int           num_;
};

void print_num(int i) { std::cout << i << '\n'; }

struct PrintNum {
    void operator()(int i) const { std::cout << i << '\n'; }
};

int main()
{
    // invoke a free function
    std::invoke(print_num, -9);
    sv::invoke(print_num, -9);

    // invoke a lambda
    std::invoke([]() { print_num(42); });
    sv::invoke([]() { print_num(42); });

    // invoke a member function
    // Notice how we can use sv::invoke in a constant expression context.
    constexpr Foo        foo(314159);
    auto const           x1 = std::invoke(&Foo::add, foo, 1);
    constexpr auto const x2 = sv::invoke(&Foo::add, foo, 1);
    (void)x1;
    (void)x2;

    // noexcept-ness is propagated correctly as well
    static_assert(noexcept(std::invoke(&Foo::add, foo, 1)));
    static_assert(noexcept(sv::invoke(&Foo::add, foo, 1)));

    // invoke (access) a data member
    std::cout << "num_: " << std::invoke(&Foo::num_, foo) << '\n';
    std::cout << "num_: " << sv::invoke(&Foo::num_, foo) << '\n';

    // invoke a function object
    std::invoke(PrintNum(), 18);
    sv::invoke(PrintNum(), 18);
}

