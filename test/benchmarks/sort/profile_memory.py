#!/usr/bin/env python3

#          Copyright Tom Westerhout 2017.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Configuration file for running memory usage benchmarks for
# sort_array.cpp

import sys
import os
import subprocess
import itertools
import random
import re
from pathlib import Path

sys.path.append(os.getcwd())
import ProfileMemory

DEBUG = False

#
# Asks b2 the path prefix for rule sort_array.
#
def _get_sort_prefix(b2 : str):
    output = None
    func   = r'sort/get_prefix'
    try:
        DEBUG and print('[*] Asking b2 for path to sort_array...',
            file=sys.stderr)
        output = subprocess.check_output(
            ' '.join([b2, func]), shell=True).decode('utf-8')
    except subprocess.CalledProcessError as e:
        print('[-] Error occured: ' + e.output.decode('utf-8'),
            file=sys.stderr)
        raise e
    output = list(filter(lambda l: l.startswith(func), 
        output.splitlines()))
    assert len(output) == 1
    return Path(re.match(func + r':\s*(.*)$',
        output[0]).group(1))
    
#
# Generates a string of size ints separated by commas.
#
def _generate_data(size : int) -> str:
    random.seed(12345 + size)
    return ', '.join(map(str, map(lambda _: random.randint(0, 10000),
        itertools.repeat(0, size))))

def main():
    args  = ProfileMemory.parse_command_line()
    start = 10
    end   = 511
    step  = 50
    b2_command = ' '.join(
        [ args.b2
        , 'toolset=' + args.toolset if args.toolset else ''
        , 'variant=' + args.variant if args.variant else ''
        ]).strip()

    global DEBUG
    DEBUG = ProfileMemory.DEBUG

    prefix  = _get_sort_prefix(b2_command)
    configs = { 'generator'      : _generate_data
              , 'sizes'          : list(range(start, end, step))
              , 'b2'             : b2_command
              , 'gnuplot'        : args.gnuplot
              , 'prefix'         : prefix
              , 'include_file'   : Path('sort/sort_array_data.txt')
              , 'memory_file'    : lambda n: \
                    prefix / 'memory_usage_{}.dat'.format(n)
              , 'results_file'   : prefix / 'space_space.dat'
              , 'target'         : 'sort/sort_array'
              , 'measure_memory' : 
                    ProfileMemory.get_measure_memory_exe(b2_command)
              }

    ProfileMemory.measure(configs)
    if args.plot_time:
        ProfileMemory.plot_space_time(configs)
    if args.plot_space:
        ProfileMemory.plot_space_space(configs)
    return 0


if __name__ == '__main__':
    main()
