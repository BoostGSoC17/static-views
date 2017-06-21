//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)



#include <iostream>
#include <cstring>
#include <cstdlib>
#include <exception>
#include <limits>
#include <system_error>
#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>

#include <boost/config.hpp>
#include <boost/assert.hpp>



struct mem_info_type {
    std::size_t total;
    std::size_t in_use;
};


#if defined(BOOST_WINDOWS)


auto get_memory_usage() -> mem_info_type
{
    MEMORYSTATUSEX ms;
    std::memset(&ms, 0, sizeof(ms));
    ms.dwLength = sizeof(ms);
    
    if (GlobalMemoryStatusEx(&ms) == 0) {
        throw std::system_error{std::error_code(GetLastError())};
    }
    BOOST_ASSERT_MSG(ms.ullTotalPhys >= ms.ullTotalAvail,
        "GlobalMemoryStatusEx() is broken.");

    return { ms.ullTotalPhys, ms.ullTotalPhys - ms.ullTotalAvail };
}


#else // assume POSIX

#include <sys/param.h>
#include <sys/sysctl.h>
#include <unistd.h>

auto _call_sysconf(auto const name) 
{
    auto const answer = sysconf(name);
    if (answer == -1) {
        // invalid name
        throw std::system_error{errno, std::generic_category()};
    }
    // sysconf only ever returns a positive answer or -1.
    return static_cast<std::size_t>(answer);
};

auto get_memory_usage() -> mem_info_type
{
#if defined(_SC_PHYS_PAGES) // non-standard extension
    std::size_t const phys_pages = _call_sysconf(_SC_PHYS_PAGES);
    std::size_t const page_size  = _call_sysconf(_SC_PAGESIZE);
    std::size_t       free_pages;

    // sysconf's man page warns that _SC_PHYS_PAGES * _SC_PAGESIZE may
    // overflow.
    BOOST_ASSERT_MSG(std::numeric_limits<std::size_t>::max() / page_size 
        >= phys_pages, "Overflow detected.");

#   if defined(_SC_AVPHYS_PAGES)
        free_pages = _call_sysconf(_SC_AVPHYS_PAGES);
        BOOST_ASSERT_MSG(phys_pages >= free_pages, "sysconf() is broken.");
#   elif defined(HW_USERMEM)
        int               free_mem = 0;
        std::size_t const len      = sizeof(free_mem);
        int const         name[2]  = { CTL_HW, HW_USERMEM };
        if (sysctl(name, 2, &free_mem, &len, nullptr, 0) == -1)
            throw std::system_error{std::error_code(errno)};
        
        free_pages = static_cast<std::size_t>(freemem) / page_size;
        BOOST_ASSERT_MSG(phys_pages >= free_pages, "sysctl() is broken.");
#   else
#       error Do not know how to get free physical RAM on this platform
#   endif

    mem_info_type mem_info;
    return { phys_pages * page_size, (phys_pages - free_pages) * page_size };
#else
#   error Do not know how to get free physical RAM on this platform
#endif
}


#endif // POSIX / WINDOWS



struct mem_usage_accumulator {
    using duration = std::chrono::milliseconds;

    mem_usage_accumulator()
        : _child{}
        , _data{}
        , _start{}
        , _ready{ false }
    {
    }

    template <class Duration>
    auto start(Duration&& time_step)
    {
        _child = std::thread([this](auto&& time_step) {
            _start = std::chrono::steady_clock::now();
            _data.push_back(std::make_pair(this->_interval_to(_start),
                get_memory_usage()));

            do {
                std::this_thread::sleep_for(time_step);
                _data.push_back( std::make_pair(
                    this->_interval_to(std::chrono::steady_clock::now()),
                    get_memory_usage()) );
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
    std::thread                                        _child;
    std::vector<std::pair<duration, mem_info_type>>    _data;
    std::chrono::steady_clock::time_point              _start;
    bool                                               _ready;

    template <class TimePoint>
    auto _interval_to(TimePoint&& t)
    {
        return std::chrono::duration_cast<duration>(t - _start);
    }
};



int main(int argc, char** argv)
{
    auto const time_step = std::chrono::milliseconds(50);

    std::string command;
    std::for_each(argv + 1, argv + argc,
        [&command](auto s) { command.append(s).push_back(' '); });

    mem_usage_accumulator acc;
    acc.start(time_step);
    auto const return_code = std::system(command.c_str());
    acc.stop();
  
    auto results = std::move(acc.result());
    BOOST_ASSERT_MSG(results.size() > 0, "No results accumulated.");
    auto const min = std::min_element(std::begin(results), std::end(results),
        [](auto const& x, auto const& y) { 
            return x.second.in_use < y.second.in_use; 
        })->second.in_use;
    std::for_each(std::begin(results), std::end(results),
        [min](auto& p) { p.second.in_use -= min; });

    std::cout << "# Statistics for running\n"
              << "#     `" << command << "`\n"
              << "#\n"
              << "#t (ms)\tmem_usage (b)\n";
    std::for_each(std::begin(results), std::end(results),
        [](auto&& p) {
            std::cout << p.first.count() << '\t' 
                      << p.second.in_use << '\n';
        });

    return return_code;
}


