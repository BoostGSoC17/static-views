// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../../example/static_map.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <unordered_map>

// #define BOOST_STATIC_VIEWS_CONSTEXPR

using key_type    = std::uint16_t;
using mapped_type = std::uint64_t;
using field_type  = std::pair<key_type, mapped_type>;

constexpr auto bits = 11;

struct entry {
    // std::uint64_t ticks;
    // std::uint16_t randidx;
    std::uint64_t ticks : 64 - bits - 1;
    std::uint64_t randidx : bits;
    std::uint64_t found : 1;
};

BOOST_STATIC_VIEWS_CONSTEXPR auto make_hasher() noexcept
{
    using namespace boost::static_views::static_map;
    return hash_c{};
}

BOOST_STATIC_VIEWS_CONSTEXPR auto make_key_equal() noexcept
{
    using namespace boost::static_views::static_map;
    return std::equal_to<void>{};
}

using hasher_type                = decltype(make_hasher());
using key_equal_type             = decltype(make_key_equal());
constexpr auto data_size         = 1024;
constexpr auto bucket_size       = 4;
constexpr auto number_of_buckets = 2 * data_size;

BOOST_STATIC_VIEWS_CONSTEXPR auto mask(key_type const x) noexcept
    -> key_type
{
    return x & ((1 << bits) - 1);
}

struct constexpr_prng {
  private:
    struct ranctx {
        std::uint32_t a;
        std::uint32_t b;
        std::uint32_t c;
        std::uint32_t d;
    };

    ranctx _ctx;

    static constexpr auto rot(
        std::uint32_t const x, int const k) noexcept -> std::uint32_t
    {
        return (x << k) | (x >> (32 - k));
    }

  public:
    BOOST_STATIC_VIEWS_CONSTEXPR constexpr_prng(
        std::uint32_t seed) noexcept
        : _ctx{0xf1ea5eed, seed, seed, seed}
    {
        for (std::size_t i = 0; i < 20; ++i) {
            static_cast<void>(this->operator()());
        }
    }

    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()() noexcept
        -> std::uint32_t
    {
        auto&         x = this->_ctx;
        std::uint32_t e = x.a - rot(x.b, 27);
        x.a             = x.b ^ rot(x.c, 17);
        x.b             = x.c + x.d;
        x.c             = x.d + e;
        x.d             = e + x.a;
        return x.d;
    }
};

struct table {
    field_type _data[data_size];

  private:
    BOOST_STATIC_VIEWS_CONSTEXPR auto contains(
        key_type const key, std::size_t const size) noexcept -> bool
    {
        struct pred {
            key_type const _key;

            BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(
                field_type const& x) noexcept -> bool
            {
                return x.first == _key;
            }
        };

        auto const i = boost::static_views::find_first_i(
            boost::static_views::take(size)(
                boost::static_views::raw_view(_data)),
            pred{key});
        // auto const i = std::find_if(std::begin(_data),
        // std::begin(_data) + size,
        //    pred{});
        return i < size;
        // return i != std::begin(_data) + size;
    }

    BOOST_STATIC_VIEWS_CONSTEXPR auto insert(
        std::size_t const i, constexpr_prng& g) noexcept -> void
    {
        auto key = mask(static_cast<key_type>(g()));
        while (contains(key, i)) {
            key = mask(static_cast<key_type>(g()));
        }
        if (contains(key, i)) {
            std::terminate();
        }
        _data[i].first  = key;
        _data[i].second = g();
    }

  public:
    template <std::size_t... Is>
    BOOST_STATIC_VIEWS_CONSTEXPR table(
        std::uint32_t seed, std::index_sequence<Is...>) noexcept
        : _data{{Is, Is}...}
    {
        static_assert(sizeof...(Is) == data_size, "");
        constexpr_prng g{seed};
        for (std::size_t i = 0; i < data_size; ++i) {
            insert(i, g);
        }
    }
};

BOOST_STATIC_VIEWS_CONSTEXPR auto make_table(
    std::uint32_t seed) noexcept
{
    return table{seed, std::make_index_sequence<data_size>{}};
}

BOOST_STATIC_VIEWS_FORCEINLINE
constexpr auto combine(
    std::uint32_t const high, std::uint32_t const low)
{
    return (static_cast<std::uint64_t>(high) << 32) | low;
}

struct empty_tester {
    BOOST_STATIC_VIEWS_NOINLINE
    auto lookup(key_type const key) const
        -> std::tuple<bool, std::size_t>
    {
        /*
        std::uint32_t cycles_low_1, cycles_high_1, cycles_low_2,
            cycles_high_2;
        */

        /*
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(cycles_high_1), "=r"(cycles_low_1)
                     :
                     : "%rax", "%rbx", "%rcx", "%rdx");
        */
        auto const start = std::chrono::high_resolution_clock::now();

        /*
        asm volatile("RDTSCP\n\t"
                     "mov %%edx, %0\n\t "
                     "mov %%eax, %1\n\t "
                     "CPUID\n\t"
                     : "=r"(cycles_high_2), "=r"(cycles_low_2)
                     :
                     : "%rax", "%rbx", "%rcx", "%rdx");
        */
        auto const end = std::chrono::high_resolution_clock::now();

        // auto const start = combine(cycles_high_1, cycles_low_1);
        // auto const end   = combine(cycles_high_2, cycles_low_2);

        if (end < start) std::terminate();
        return {false, (end - start).count()};
    }
};

static BOOST_STATIC_VIEWS_CONSTEXPR auto const table =
    make_table(12345);
static auto const static_map =
    boost::static_views::static_map::make_static_map<
        number_of_buckets, bucket_size>(
        boost::static_views::raw_view(table._data),
        &field_type::first, &field_type::second, make_key_equal(),
        make_hasher());

struct static_map_tester {
    BOOST_STATIC_VIEWS_NOINLINE
    auto lookup(key_type const key) const
        -> std::tuple<bool, std::size_t>
    {
        /*
        std::uint32_t cycles_low_1, cycles_high_1, cycles_low_2,
            cycles_high_2;
        */
        bool contains;

        /*
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(cycles_high_1), "=r"(cycles_low_1)
                     :
                     : "%rax", "%rbx", "%rcx", "%rdx");
        */
        auto const start = std::chrono::high_resolution_clock::now();

        contains = (static_map.count(key) == 1);

        /*
        asm volatile("RDTSCP\n\t"
                     "mov %%edx, %0\n\t "
                     "mov %%eax, %1\n\t "
                     "CPUID\n\t"
                     : "=r"(cycles_high_2), "=r"(cycles_low_2)
                     :
                     : "%rax", "%rbx", "%rcx", "%rdx");
        */
        auto const end = std::chrono::high_resolution_clock::now();

        // auto const start = combine(cycles_high_1, cycles_low_1);
        // auto const end   = combine(cycles_high_2, cycles_low_2);

        if (end < start) std::terminate();
        return {contains, (end - start).count()};
    }
};

BOOST_STATIC_VIEWS_FORCEINLINE
auto initialise_unordered_map()
{
    std::unordered_map<key_type, mapped_type, hasher_type,
        key_equal_type>
        map{number_of_buckets, make_hasher(), make_key_equal()};
    std::for_each(std::begin(table._data), std::end(table._data),
        [&map](auto const x) {
            if (!map.insert(x).second) std::terminate();
        });
    return map;
}

auto const unordered_map = initialise_unordered_map();

struct unordered_map_tester {
    BOOST_STATIC_VIEWS_NOINLINE
    auto lookup(key_type const key) const
        -> std::tuple<bool, std::size_t>
    {
        /*
        std::uint32_t cycles_low_1, cycles_high_1, cycles_low_2,
            cycles_high_2;
        */
        bool contains;

        /*
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(cycles_high_1), "=r"(cycles_low_1)
                     :
                     : "%rax", "%rbx", "%rcx", "%rdx");
        */
        auto const start = std::chrono::high_resolution_clock::now();

        contains = (unordered_map.count(key) == 1);

        /*
        asm volatile("RDTSCP\n\t"
                     "mov %%edx, %0\n\t "
                     "mov %%eax, %1\n\t "
                     "CPUID\n\t"
                     : "=r"(cycles_high_2), "=r"(cycles_low_2)
                     :
                     : "%rax", "%rbx", "%rcx", "%rdx");
        */
        auto const end = std::chrono::high_resolution_clock::now();

        // auto const start = combine(cycles_high_1, cycles_low_1);
        // auto const end   = combine(cycles_high_2, cycles_low_2);

        if (end < start) std::terminate();
        return {contains, (end - start).count()};
    }
};

struct statistics {

  private:
    std::vector<entry> _data;

    auto generate_random(std::size_t size)
    {
        constexpr_prng g{54321};

        std::vector<entry> entries(size);
        std::generate(
            std::begin(entries), std::end(entries), [&g]() -> entry {
                return {0, mask(g()), 0};
            });
        return entries;
    }

  public:
    statistics(std::size_t size) : _data{generate_random(size)} {}

    template <class Tester>
    auto run(Tester const& tester)
    {
        std::for_each(
            std::begin(_data), std::end(_data), [&tester](auto& x) {
                auto answer = tester.lookup(x.randidx);
                x.found     = std::get<0>(answer);
                x.ticks     = std::get<1>(answer);
            });
    }

    auto reset()
    {
        std::for_each(
            std::begin(_data), std::end(_data), [](auto& x) {
                x.ticks = 0;
                x.found = 0;
            });
    }

    auto sum()
    {
        return std::accumulate(std::begin(_data), std::end(_data),
            std::size_t{0}, [](auto const acc, auto const x) {
                if (acc + x.ticks < acc) std::terminate();
                return acc + x.ticks;
            });
    }

    template <class OStream>
    auto save(OStream& out)
    {
        out << "randidx\tticks\n";
        std::for_each(std::begin(_data), std::end(_data),
            [&out](auto const& x) {
                out << x.randidx << '\t' << x.ticks << '\t' << x.found
                    << '\n';
            });
    }
};

int main(int argc, char** argv)
{
    static_map_tester    sm_tester;
    unordered_map_tester um_tester;
    empty_tester         dummy_tester;

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " count\n"
                  << "    with count -- number of data points.\n";
        return 1;
    }
    statistics stats{std::stoul(argv[1])};

    for (auto bucket : static_map) {
        std::cout << bucket.size() << '\n';
    }

    stats.run(dummy_tester);
    {
        std::ofstream out{"empty.dat"};
        stats.save(out);
    }

    stats.reset();

    stats.run(sm_tester);
    {
        std::ofstream out{"static_map.dat"};
        stats.save(out);
    }

    stats.reset();

    stats.run(um_tester);
    {
        std::ofstream out{"unordered_map.dat"};
        stats.save(out);
    }
    return 0;
}
