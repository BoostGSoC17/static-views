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
using mapped_type = std::uint16_t;
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

auto global_generator()
{
    static std::uniform_int_distribution<key_type> dist{
        0, (1 << bits) - 1};
    static std::mt19937 g;
    return [ dist = std::ref(dist), g = std::ref(g) ]()
    {
        return dist(g.get());
    };
}

namespace detail {
template <class Vector, class Generator>
static auto push_chunk(
    Vector& xs, std::size_t const chunk_size, Generator& g)
{
    xs.reserve(xs.size() + chunk_size);
    for (std::size_t i = 0; i < chunk_size; ++i) {
        xs.push_back(g());
    }
}

template <class Generator>
static auto create(std::size_t const size, Generator& g)
{
    std::vector<key_type> xs;

    for (std::size_t i = 0; i < size; ++i) {
        xs.push_back(i);
    };
    std::random_shuffle(std::begin(xs), std::end(xs));
    return xs;
}

template <std::size_t... Is>
BOOST_STATIC_VIEWS_FORCEINLINE auto make_table_impl(
    std::index_sequence<Is...>)
    -> std::array<field_type, sizeof...(Is)>
{
    auto const g  = global_generator();
    auto const xs = create((1 << bits) - 1, g);
    return {field_type{xs[Is], g()}...};
}
} // namespace detail

auto make_table()
{
    return detail::make_table_impl(
        std::make_index_sequence<data_size>{});
}

static std::array<field_type, data_size> table = make_table();

BOOST_STATIC_VIEWS_FORCEINLINE
constexpr auto combine(
    std::uint32_t const high, std::uint32_t const low)
{
    return (static_cast<std::uint64_t>(high) << 32) | low;
}

template <class Function>
BOOST_STATIC_VIEWS_FORCEINLINE auto time(Function&& func)
{
    std::uint32_t cycles_low_1, cycles_high_1, cycles_low_2,
        cycles_high_2;

    asm volatile("CPUID\n\t"
                 "RDTSC\n\t"
                 "mov %%edx, %0\n\t"
                 "mov %%eax, %1\n\t"
                 : "=r"(cycles_high_1), "=r"(cycles_low_1)
                 :
                 : "%rax", "%rbx", "%rcx", "%rdx");
    // auto const start = std::chrono::high_resolution_clock::now();

    auto const x = std::forward<Function>(func)();

    asm volatile("RDTSCP\n\t"
                 "mov %%edx, %0\n\t "
                 "mov %%eax, %1\n\t "
                 "CPUID\n\t"
                 : "=r"(cycles_high_2), "=r"(cycles_low_2)
                 :
                 : "%rax", "%rbx", "%rcx", "%rdx");
    // auto const end = std::chrono::high_resolution_clock::now();

    auto const start = combine(cycles_high_1, cycles_low_1);
    auto const end   = combine(cycles_high_2, cycles_low_2);

    if (end < start) std::terminate();
    return std::make_tuple(x, end - start);
}

template <class Table>
BOOST_STATIC_VIEWS_FORCEINLINE auto initialise_unordered_map(
    Table& table)
{
    std::unordered_map<key_type, mapped_type, hasher_type,
        key_equal_type>
        map{number_of_buckets, make_hasher(), make_key_equal()};
    std::for_each(
        std::begin(table), std::end(table), [&map](auto const x) {
            if (!map.insert(x).second) std::terminate();
        });
    return map;
}

struct statistics {

  private:
    std::vector<entry> _data;

    static auto generate_random(std::size_t size)
    {
        std::vector<entry> entries(size);
        std::generate(std::begin(entries),
            std::end(entries), [g = global_generator()]()->entry {
                return {0, g(), 0};
            });
        return entries;
    }

  public:
    statistics(std::size_t size) : _data{generate_random(size)} {}

    template <class Map>
    auto run(Map const& map)
    {
        auto const func = [&map](auto& x) -> bool {
            return map.count(x) == 1;
        };
        std::for_each(
            std::begin(_data), std::end(_data), [&func](auto& x) {
                auto answer = time(
                    [ i = x.randidx, &func ]() { return func(i); });
                x.found = std::get<0>(answer);
                x.ticks = std::get<1>(answer);
            });
    }

    auto run_nop()
    {
        auto const func = [](auto& x) -> bool { return false; };
        std::for_each(
            std::begin(_data), std::end(_data), [&func](auto& x) {
                auto answer = time(
                    [ i = x.randidx, &func ]() { return func(i); });
                x.found = std::get<0>(answer);
                x.ticks = std::get<1>(answer);
            });
    }

    auto reset()
    {
        std::for_each(
            std::begin(_data), std::end(_data), [](auto& x) {
                x.found = 0;
                x.ticks = 0;
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
    auto const static_map =
        boost::static_views::static_map::make_static_map<
            number_of_buckets, bucket_size>(
            boost::static_views::raw_view(table), &field_type::first,
            &field_type::second, make_key_equal(), make_hasher());
    auto const unordered_map = initialise_unordered_map(table);

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " count\n"
                  << "    with count -- number of data points.\n";
        return 1;
    }
    statistics stats{std::stoul(argv[1])};

    for (auto bucket : static_map) {
        std::cout << bucket.size() << '\n';
    }

    stats.run_nop();
    {
        std::ofstream out{"empty.dat"};
        stats.save(out);
    }

    stats.reset();

    stats.run(static_map);
    {
        std::ofstream out{"static_map.dat"};
        stats.save(out);
    }

    stats.reset();

    stats.run(unordered_map);
    {
        std::ofstream out{"unordered_map.dat"};
        stats.save(out);
    }
    return 0;
}
