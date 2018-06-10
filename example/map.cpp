#include <iostream>
#include "static_map.hpp"

struct equal_c {

    template <class T>
    using equal_t = decltype(std::declval<T>() == std::declval<T>());

    template <class T>
    using not_equal_t =
        decltype(std::declval<T>() != std::declval<T>());

    template <class Char>
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(
        Char const* const a, Char const* const b) const noexcept
    {
        using boost::static_views::is_detected;
        static_assert(
            std::is_nothrow_default_constructible<Char>::value,
            "`equal_c` requires `Char` to be nothrow "
            "default-constructible.");
        static_assert(is_detected<equal_t, Char>::value
                          && is_detected<not_equal_t, Char>::value,
            "`equal_c` requires `Char` to define `operator==` and "
            "`operator!=`.");
        static_assert(
            noexcept(std::declval<Char>() == std::declval<Char>())
                && noexcept(
                       std::declval<Char>() != std::declval<Char>()),
            "`equal_c` requires `Char`'s `operator==` and "
            "`operator!=` to be noexcept.");

        std::size_t i = 0;
        while (a[i] != Char{} && a[i] == b[i]) {
            ++i;
        }
        return a[i] == Char{} && b[i] == Char{};
    }
};

enum class weekday {
    sunday,
    monday,
    tuesday,
    wednesday,
    thursday,
    friday,
    saturday
};

void test1()
{
    {
        using value_type = std::pair<int const, char const*>;
        static constexpr value_type map_data[] = {
            {5, "apple"}, {8, "pear"}, {0, "banana"}};

        struct get_key {
            constexpr decltype(auto) operator()(
                std::pair<int const, char const*> const& x) const
                noexcept
            {
                return x.first;
            }
        };
        struct get_mapped {
            constexpr decltype(auto) operator()(
                std::pair<int const, char const*> const& x) const
                noexcept
            {
                return x.second;
            }
        };
        constexpr auto cmap =
            boost::static_views::static_map::make_static_map_impl<10, 1>{}(
                boost::static_views::raw_view(map_data), get_key{},
                get_mapped{});

        static_assert(equal_c{}(cmap[5], "apple"), "");
        static_assert(equal_c{}(cmap[8], "pear"), "");
        static_assert(equal_c{}(cmap[0], "banana"), "");
        static_assert(cmap.find(8) == &map_data[1], "");
        static_assert(cmap.find(10) == nullptr, "");

        if (cmap.find(10) != nullptr) std::terminate();
    }
}

int main(int argc, char** argv)
{
    test1();
    return 0;
}

