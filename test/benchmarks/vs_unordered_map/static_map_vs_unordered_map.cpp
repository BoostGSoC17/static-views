//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <unordered_map>

#include <benchmark/benchmark.h>

#include <boost/static_views.hpp>

// Configuration
#if !defined(BUCKET_SIZE)
#error "Need BUCKET_SIZE"
#endif

#if !defined(BUCKET_COUNT)
#error "Need BUCKET_COUNT"
#endif

// #define BOOST_STATIC_VIEWS_CONSTEXPR

namespace sv = boost::static_views;

using key_type       = std::uint16_t;
using mapped_type    = std::uint64_t;
using field_type     = std::pair<key_type, mapped_type>;
using hasher_type    = sv::hash_c;
using key_equal_type = std::equal_to<>;

constexpr auto bucket_size       = BUCKET_SIZE;
constexpr auto number_of_buckets = BUCKET_COUNT;

static constexpr field_type data_table[] = {
#include "test_data.ipp"
};

constexpr auto data_size = std::end(data_table) - std::begin(data_table);

auto generate_lookup_keys(std::size_t const n, double const positive_frac,
    std::size_t seed) -> std::vector<key_type>
{
    if (n < static_cast<std::size_t>(data_size)) {
        throw std::invalid_argument{"Currently, only `n`s greater or equal to "
                                    "`data_size` are supported."};
    }
    if (positive_frac < 0 || positive_frac > 1) {
        throw std::invalid_argument{
            "Fraction of positive samples must be between 0 and 1."};
    }

    std::mt19937                            gen(seed);
    std::uniform_int_distribution<key_type> dist;
    std::vector<key_type>                   keys(n);
    auto const pos_n = static_cast<std::size_t>(positive_frac * n);

    std::transform(std::begin(data_table), std::end(data_table),
        std::begin(keys), [](auto const& x) { return x.first; });
    std::shuffle(std::begin(keys), std::begin(keys) + data_size, gen);
    // Yes, I know this is extremely slow and may even never terminate.
    // But it's simple! :)
    for (auto i = std::begin(keys) + pos_n; i != std::end(keys); ++i) {
        auto const end = std::begin(keys) + pos_n;
        auto       k   = dist(gen);
        while (std::find(std::begin(keys), end, k) != end) {
            ++k;
        }
        *i = k;
    }
    std::shuffle(std::begin(keys), std::end(keys), gen);

    return keys;
}

auto lookup_noop(key_type const k) -> field_type* { return nullptr; }

auto benchmark_empty_lookup(benchmark::State& state)
{
    auto const seed = state.range(0);
    auto const n    = state.range(1);
    auto const keys = generate_lookup_keys(n, 0.5, seed);

    for (auto _ : state) {
        for (auto k : keys) {
            benchmark::DoNotOptimize(lookup_noop(k));
        }
    }
}

auto benchmark_static_map_lookup(benchmark::State& state)
{
    auto const seed = state.range(0);
    auto const n    = state.range(1);
    auto const keys = generate_lookup_keys(n, 0.5, seed);

    static constexpr auto map =
        sv::make_static_map_impl<number_of_buckets, bucket_size>{}(
            sv::raw_view(data_table), &field_type::first, &field_type::second,
            key_equal_type{}, hasher_type{});

    for (auto _ : state) {
        for (auto k : keys) {
            benchmark::DoNotOptimize(map.find(k));
        }
    }

    state.SetComplexityN(state.range(1));
}

auto benchmark_unordered_map_lookup(benchmark::State& state)
{
    auto const seed = state.range(0);
    auto const n    = state.range(1);
    auto const keys = generate_lookup_keys(n, 0.5, seed);

    auto const map =
        std::unordered_map<key_type, mapped_type, hasher_type, key_equal_type>{
            std::begin(data_table), std::end(data_table), number_of_buckets};

    for (auto _ : state) {
        for (auto k : keys) {
            benchmark::DoNotOptimize(map.find(k));
        }
    }

    state.SetComplexityN(state.range(1));
}

constexpr auto N = static_cast<long>(1.5 * data_size);

// clang-format off
BENCHMARK(benchmark_static_map_lookup)
    ->Args({123, 1 * N})
    ->Args({124, 1 * N})
    ->Args({125, 1 * N})
    ->Args({126, 1 * N})
    ->Args({127, 2 * N})
    ->Args({128, 2 * N})
    ->Args({129, 2 * N})
    ->Args({130, 2 * N})
    ->Args({131, 3 * N})
    ->Args({132, 3 * N})
    ->Args({133, 3 * N})
    ->Args({134, 3 * N})
    ->Args({135, 6 * N})
    ->Args({136, 6 * N})
    ->Args({137, 6 * N})
    ->Args({138, 6 * N})
    ->Args({139, 8 * N})
    ->Args({140, 8 * N})
    ->Args({141, 8 * N})
    ->Args({142, 8 * N})
    ->Complexity([](auto const n) -> double { return n; });

BENCHMARK(benchmark_unordered_map_lookup)
    ->Args({123, 1 * N})
    ->Args({124, 1 * N})
    ->Args({125, 1 * N})
    ->Args({126, 1 * N})
    ->Args({127, 2 * N})
    ->Args({128, 2 * N})
    ->Args({129, 2 * N})
    ->Args({130, 2 * N})
    ->Args({131, 3 * N})
    ->Args({132, 3 * N})
    ->Args({133, 3 * N})
    ->Args({134, 3 * N})
    ->Args({135, 6 * N})
    ->Args({136, 6 * N})
    ->Args({137, 6 * N})
    ->Args({138, 6 * N})
    ->Args({139, 8 * N})
    ->Args({140, 8 * N})
    ->Args({141, 8 * N})
    ->Args({142, 8 * N})
    ->Complexity([](auto const n) -> double { return n; });
// clang-format on

BENCHMARK_MAIN();

