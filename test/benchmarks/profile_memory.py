#!/usr/bin/env python3.4

#          Copyright Tom Westerhout 2017.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

import sys, os, subprocess, itertools, random


def generate_data(size : int) -> str:
    random.seed(12345)
    return ','.join(map(str, map(lambda _: random.randint(0, 10000), 
        itertools.repeat(0, size))))

def save_data(data : str, data_file : str):
    with open(data_file, 'wt') as fh:
        fh.write(data + '\n')

def profile_memory(command : str):
    output = None
    try:
        print("[*] " + repr(command) + "...", file=sys.stderr)
        output = subprocess.check_output(
            command,
            shell=True
            ).decode('utf-8')
    except subprocess.CalledProcessError as e:
        print("\n[-] Error while excuting " + repr(command) + ": " 
            + e.output.decode('utf-8'), file=sys.stderr)
        return None
    return list(map(lambda l: tuple(map(int, l.split('\t'))), 
        output.splitlines()[2:]))

def get_stats(data : list):
    _ram  = lambda x: x[1]
    _swap = lambda x: x[2]
    min_ram_usage  = min(map(_ram, data))
    max_ram_usage  = max(map(_ram, data))
    min_swap_usage = min(map(_swap, data))
    max_swap_usage = max(map(_swap, data))
    return (max_ram_usage - min_ram_usage, max_swap_usage - min_swap_usage)

def measure_one(size : int, data_file : str, command : str):
    print('[*] size = ' + str(size), file=sys.stderr)
    save_data(generate_data(size), data_file)
    results = profile_memory(command)
    # with open('sort/sort_array_clang.' + str(size) + '.dat', 'wt') as fh:
    #     for r in results:
    #         fh.write('\t'.join(map(str, r)) + '\n')
    return get_stats(results)

def measure(sizes : list, data_file : str, command : str):
    return list(map(lambda s: (s,) + measure_one(s, data_file, command),
        sizes))


def main():
    measure_command = "bin/gcc-6.3.0/release/threading-multi/measure_memory" \
        + " 'b2 -a toolset=gcc sort_array 1>&2'"
    data_file       = "sort/sort_array_data.txt"
    output_file     = "sort/sort_array_gcc.dat"
    start = 10
    end   = 500
    step  = 10

    results = measure(itertools.takewhile(lambda x: x < end,
        itertools.count(start, step)), data_file, measure_command)

    with open(output_file, 'wt') as fh:
        for r in results:
            fh.write('\t'.join(map(str, r)) + '\n')

    return 0


if __name__ == '__main__':
    main()
