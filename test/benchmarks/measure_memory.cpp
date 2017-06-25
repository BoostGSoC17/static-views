//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

#include <boost/assert.hpp>
#include <boost/config.hpp>



struct mem_info {
    std::size_t ram;
    std::size_t swap;
};


constexpr auto operator+ (mem_info const& x, mem_info const& y) -> mem_info
{
    BOOST_ASSERT_MSG(std::numeric_limits<std::size_t>::max() - x.ram >= y.ram,
        "Overflow detected.");
    BOOST_ASSERT_MSG(std::numeric_limits<std::size_t>::max() - x.swap >= y.swap,
        "Overflow detected.");
    return { x.ram + y.ram, x.swap + y.swap };
}

constexpr auto operator- (mem_info const& x, mem_info const& y) -> mem_info
{
    BOOST_ASSERT_MSG(x.ram >= y.ram, "Overflow detected.");
    BOOST_ASSERT_MSG(x.swap >= y.swap, "Overflow detected.");
    return { x.ram - y.ram, x.swap - y.swap };
}

constexpr auto operator* (mem_info const& x, mem_info const& y) -> mem_info
{
    BOOST_ASSERT_MSG(std::numeric_limits<std::size_t>::max() / x.ram >= y.ram,
        "Overflow detected.");
    BOOST_ASSERT_MSG(std::numeric_limits<std::size_t>::max() / x.swap >= y.swap,
        "Overflow detected.");
    return { x.ram * y.ram, x.swap * y.swap };
}

constexpr auto operator* (std::size_t const k, mem_info const& x) -> mem_info
{
    BOOST_ASSERT_MSG(std::numeric_limits<std::size_t>::max() / k >= x.ram,
        "Overflow detected.");
    BOOST_ASSERT_MSG(std::numeric_limits<std::size_t>::max() / k >= x.swap,
        "Overflow detected.");
    return { k * x.ram, k * x.swap };
}

constexpr auto operator/ (mem_info const& x, std::size_t const k) -> mem_info
{
    return { x.ram / k, x.swap / k};
}

constexpr auto operator== (mem_info const& x, mem_info const& y)
{
    return x.ram == y.ram && x.swap == y.swap;
}

constexpr auto operator!= (mem_info const& x, mem_info const& y)
{
    return !(x == y);
}


#if defined(_WIN64) 
// 64-bit Windows
// Use GlobalMemoryStatusEx()
#include <windows.h>

auto _call_global_memory_status_ex(MEMORYSTATUSEX & ms) -> void
{
    std::memset(&ms, 0, sizeof(ms));
    ms.dwLength = sizeof(ms);
    if (GlobalMemoryStatusEx(&ms) == 0) {
        throw std::system_error{GetLastError(), std::generic_category()};
    }
}

auto get_total_mem() -> mem_info
{
    MEMORYSTATUSEX ms;
    _call_global_memory_status_ex(ms);
    return { ms.ullTotalPhys, ms.ullTotalPageFile };
}

auto get_avail_mem() -> mem_info
{
    MEMORYSTATUSEX ms;
    _call_global_memory_status_ex(ms);
    return { ms.ullAvailPhys, ms.ullAvailPageFile };
}


#elif defined(_WIN32)
// 32-bit Windows
// Use GlobalMemoryStatus()
#include <windows.h>

auto _call_global_memory_status(MEMORYSTATUS & ms) -> void
{
    std::memset(&ms, 0, sizeof(ms));
    ms.dwLength = sizeof(ms);
    GlobalMemoryStatus(&ms);
}

auto get_total_mem() -> mem_info
{
    MEMORYSTATUS ms;
    _call_global_memory_status(ms);
    return { ms.dwTotalPhys, ms.dwTotalPageFile };
}

auto get_avail_mem() -> mem_info
{
    MEMORYSTATUS ms;
    _call_global_memory_status(ms);
    return { ms.dwAvailPhys, ms.dwAvailPageFile };
}


#elif defined(__linux__) || defined(__linux) || defined(__gnu_linux)
// Linux
// Use sysinfo()
#include <sys/sysinfo.h>

auto _call_sysinfo(struct sysinfo & info)
{
    if (sysinfo(&info) == -1) {
        throw std::system_error{errno, std::generic_category()};
    }
}

auto get_total_mem() -> mem_info
{
    struct sysinfo info;
    _call_sysinfo(info);
    
    static_assert(std::numeric_limits<unsigned long>::max() 
        * std::numeric_limits<unsigned int>::max() 
            <= std::numeric_limits<std::size_t>::max(), 
        "Possible overflow.");
    return { static_cast<std::size_t>(info.totalram) * info.mem_unit
           , static_cast<std::size_t>(info.totalswap) * info.mem_unit };
}

auto get_avail_mem() -> mem_info
{
    struct sysinfo info;
    _call_sysinfo(info);
    
    static_assert(std::numeric_limits<unsigned long>::max() 
        * std::numeric_limits<unsigned int>::max() 
            <= std::numeric_limits<std::size_t>::max(), 
        "Possible overflow.");
    return { static_cast<std::size_t>(info.freeram) * info.mem_unit
           , static_cast<std::size_t>(info.freeswap) * info.mem_unit };
}


#elif defined(__unix__) || defined(__unix) \
    || (defined(__APPLE__) && defined(__MACH__))
// UNIX
#include <unistd.h>
#include <sys/param.h>


#   if defined(_POSIX_VERSION)
// POSIX
// use sysconf()

auto _call_sysconf(auto const name) 
{
    auto const answer = sysconf(name);
    if (answer == -1) {
        throw std::system_error{errno, std::generic_category()};
    }

    BOOST_ASSERT_MSG(answer >= 0, "sysconf() is broken.");
    return static_cast<std::size_t>(answer);
}


auto get_total_mem() -> mem_info
{
#if defined(_SC_PHYS_PAGES) // non-standard extension
    std::size_t const phys_pages = _call_sysconf(_SC_PHYS_PAGES);
    std::size_t const page_size  = _call_sysconf(_SC_PAGESIZE);

    BOOST_ASSERT_MSG(std::numeric_limits<std::size_t>::max() / page_size 
        >= phys_pages, "Overflow detected.");
    return { phys_pages * page_size, std::size_t{} };
#else
#       error "Do not know how to get total physical RAM on this platform."
#endif
}


#       if defined(BSD) || (defined(__APPLE__) && defined(__MACH__))

auto get_avail_mem() -> mem_info
{
    int               free_mem  = 0;
    std::size_t const len       = sizeof(free_mem);
    int               name[2];  = { CTL_HW, HW_USERMEM };

    if (sysctl(name, 2, &free_mem, &len, nullptr, 0) == -1)
        throw std::system_error{errno, std::generic_category()};

    BOOST_ASSERT_MSG(free_mem >= 0, "sysctl() is broken.");
    return { static_cast<std::size_t>(free_mem), std::size_t{} };
}

#       elif defined(_SC_AVPHYS_PAGES)

auto get_avail_mem() -> mem_info
{
    std::size_t const free_pages = _call_sysconf(_SC_AVPHYS_PAGES);
    std::size_t const page_size  = _call_sysconf(_SC_PAGESIZE);

    BOOST_ASSERT_MSG(std::numeric_limits<std::size_t>::max() / page_size 
        >= free_pages, "Overflow detected.");
    return { free_pages * page_size, std::size_t{} };
}

#       else
#           error "Don't know how to get free physical RAM on this platform."
#       endif

#   else
#       error "Do not know how to get free physical RAM on this platform."
#   endif

#else
#   error "Don't know how to get free physical RAM on this platform."
#endif



struct mem_usage_accumulator {
    using duration   = std::chrono::milliseconds;
    using data_point = std::tuple<duration, mem_info>;

private:
    template <class TimePoint>
    auto _interval_to(TimePoint&& t)
    {
        return std::chrono::duration_cast<duration>(t - _start);
    }

    template <class TimePoint>
    auto _measure_point(TimePoint&& t)
    {
        return std::make_tuple(_interval_to(std::forward<TimePoint>(t)),
            _total - get_avail_mem() / 1024);
    }

public:

    mem_usage_accumulator()
        : _child{}
        , _data{}
        , _start{}
        , _total{ get_total_mem() / 1024 }
        , _ready{ false }
    {
    }

    template <class Duration>
    auto start(Duration&& time_step)
    {
        _data.clear();
        _child = std::thread([this](auto&& time_step) {
            _start = std::chrono::steady_clock::now();
            _data.push_back(this->_measure_point(_start));

            do {
                std::this_thread::sleep_for(time_step);
                _data.push_back(this->_measure_point(
                    std::chrono::steady_clock::now()));
            }
            while (!_ready);
        }, std::forward<Duration>(time_step));
    }

    auto stop()
    {
        _ready = true;
        _child.join();
    }

    auto const& result() const { return _data; }
    auto &      result()       { return _data; }

private:
    std::thread                               _child;
    std::vector<data_point>                   _data;
    std::chrono::steady_clock::time_point     _start;
    mem_info                                  _total;
    bool                                      _ready;

};


template <class DataPoints>
auto avg_and_stddev_Kb(DataPoints const& data)
{
    // Average memory usage in Kb
    auto const avg_x_Kb  = std::accumulate(
        std::begin(data), std::end(data),
        mem_info{ 0, 0 },
        [](auto const& acc, auto const& x) {
            return acc + std::get<1>(x);
        }) / data.size();

    // Average squared memory usage in Kb^2
    auto const avg_x2_Kb2 = std::accumulate(
        std::begin(data), std::end(data),
        mem_info{ 0, 0 },
        [](auto const& acc, auto const& x) {
            return acc + std::get<1>(x) * std::get<1>(x);
        }) / data.size();

    // Variance in Kb^2
    auto const var_x_Kb2 = avg_x2_Kb2 - avg_x_Kb * avg_x_Kb;

    // Calculate a square root of size_t using long double precision
    auto const _sqrt = [](std::size_t const x) -> std::size_t {
        return static_cast<std::size_t>(std::sqrt(static_cast<long double>(x)));
    };

    // Standard deviation in Kb
    auto const stddev_x_Kb = 
        mem_info{ _sqrt(var_x_Kb2.ram), _sqrt(var_x_Kb2.swap) };

    return std::make_pair(avg_x_Kb, stddev_x_Kb);
}


int main(int argc, char** argv)
{
    auto        const time_step     = std::chrono::milliseconds(2);
    std::size_t const prefix        = 1000;
    auto        const command       = std::accumulate(
        argv + 1, argv + argc, std::string{},
        [](auto x, auto y) { x.push_back(' '); return x.append(y); });
    
    mem_usage_accumulator acc;
    acc.start(time_step);
    std::this_thread::sleep_for(prefix * time_step);
    auto const return_code = std::system(command.c_str());
    acc.stop();
    auto const results = std::move(acc.result());


    std::cout << "# command `" << command << "`\n"
              << "# time(ms)\tram(Kb)\tswap(Kb)\n";
    std::for_each(std::begin(results), std::end(results),
        [](auto const& x) {
            std::cout << std::get<0>(x).count() << '\t'
                      << std::get<1>(x).ram     << '\t'
                      << std::get<1>(x).swap    << '\n';
        });

    return return_code;
}


