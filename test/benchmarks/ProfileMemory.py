#!/usr/bin/env python3

#          Copyright Tom Westerhout 2017.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

import argparse
import itertools
import os
import random
import re
import subprocess
import sys
import time
from pathlib import Path

DEBUG = False

def parse_command_line():
    parser = argparse.ArgumentParser()
    parser.add_argument('--b2', type=str, default='b2',
        help='Specify path to b2 executable. It it not assumed that '
            + 'this path contains only one word. You may thus include '
            + 'options')
    parser.add_argument('--gnuplot', type=str, default='gnuplot',
        help='Specify path to Gnuplot executable.')
    parser.add_argument('--root', type=str, required=True,
        help='Path to the static-views/test/benchmarks/ directory.')
    parser.add_argument('--output', type=str, required=True,
        help='Specify output directory.')
    parser.add_argument('--toolset', type=str, required=True,
        help='Choose toolset.')
    parser.add_argument('--debug', action='store_true', default=False,
        help='Turn debug mode ON.')
    args = parser.parse_args()

    # Immediately update the debug level for this file
    global DEBUG 
    DEBUG = args.debug
    return args

#
# Writes data to include_file.
#
def _save_input(data : str, include_file):
    DEBUG and print('[*] Saving data to ' + repr(str(include_file)) \
        + '... ', end='', file=sys.stderr)
    with include_file.open('wt') as fh:
        fh.write(data + '\n')
    DEBUG and print('Done!', file=sys.stderr)

#
# Writes data to memory_file as tab-separated columns. Data is assumed
# to be a list of tuples of three elements: time, ram usage, and swap
# usage.
#
def _save_memory_usage(data : list, memory_file):
    DEBUG and print('[*] Saving data to ' + repr(memory_file) \
        + '... ', end='', file=sys.stderr)
    with memory_file.open('wt') as fh:
        fh.write('# Memory usage during command execution.\n')
        fh.write('#\n')
        fh.write('time\tram\tswap\n')
        fh.write('\n'.join(
            map(lambda x: '\t'.join(map(str, x)), data) ))
        fh.write('\n')
    DEBUG and print('Done!', file=sys.stderr)

#
# Reads contents of filename, writes them to STDERR and removed filename.
#
def _read_and_remove(filename):
    assert filename.exists()
    with filename.open('rt') as fh:
        for line in fh:
            sys.stderr.write(line)
    os.remove(str(filename))

#
# * configs['measure_memory'] -- Path to measure_memory executable.
# * configs['b2']             -- Path to b2 executable.
# * configs['target']         -- Target to build.
# * configs['toolset']        -- Tooset to use for building.
# * configs['root']           -- Path to the directory the Jamfile in
#                                which defines the configs['target']
#                                target.
# * configs['temp_file']      -- Temporary file to use for storing the
#                                output of b2. This file is removed
#                                afterwards.
#
# This function executes measure_memory which in turn builds target
# using b2 and records memory usage. Result is returned as an iterable
# of tuples (time, ram, swap).
#
def _compile_recording(configs : dict):
    # Call `b2 target`, ignore its output. In the meanwhile, measure
    # the memory usage.
    escape = { 'nt'    : '^'
             , 'posix' : '\\' }
    cmd = '{mm} {b2} -a {root}//{target} {esc}>{temp}'.format(
        mm=configs['measure_memory'],
        b2=configs['b2'],
        root=configs['root'],
        target=configs['target'],
        esc=escape[os.name],
        temp=configs['temp_file'])
    output = None
    try:
        DEBUG and print('[*] Executing `' + cmd + '`...', 
            file=sys.stderr)
        output = subprocess.check_output(
            cmd, shell=True).decode('utf-8')
        _read_and_remove(configs['temp_file'])
    except subprocess.CalledProcessError as e:
        _read_and_remove(configs['temp_file'])
        print('[-] Error occured: ' 
            + e.output.decode('utf-8'), file=sys.stderr)
        return (None, None)
    output = output.splitlines()
    duration  = float(output[-1])
    mem_usage = map(lambda l: tuple(map(int, l.split('\t'))), 
        output[2:-1])
    return (duration, mem_usage)

#
# Given a list of tuples [(time, ram, swap)], returns a tuple of two
# elements (peak-ram-usage, peak-swap-usage).
#
def _get_stats(data : list):
    _ram  = lambda x: x[1]
    _swap = lambda x: x[2]
    min_ram_usage  = min(map(_ram, data))
    max_ram_usage  = max(map(_ram, data))
    min_swap_usage = min(map(_swap, data))
    max_swap_usage = max(map(_swap, data))
    return (max_ram_usage - min_ram_usage, 
        max_swap_usage - min_swap_usage)

#
# Measures memory usage while compiling a target for size n.
#
def _measure_one(size : int, configs : dict):
    sys.stderr.flush()
    DEBUG and print('[*] Test: size = ' + str(size), file=sys.stderr)
    _save_input(configs['generator'](size), configs['include_file'])
    (duration, mem_usage) = _compile_recording(configs)
    if duration is None:
        return None
    return (size, duration,) + _get_stats(list(mem_usage))

#
# Measure memory usage while compiling target.
#
def measure(configs : dict):
    DEBUG and print('[*] Measuring for ' + repr(configs['test-name']) \
        + '...', file=sys.stderr)
    if not configs['prefix'].exists():
        configs['prefix'].mkdir(parents=True)
    with configs['results_file'].open('wt') as fh:
        fh.write('# Peak memory usage during function execution\n')
        fh.write('# as a function of input size.\n')
        fh.write('#\n')
        fh.write('size\ttime\tram\tswap\n')
    # Travis has a time limit of 10 min. Leave 30 seconds for b2 and
    # Gnuplot
    time_limit = time.time() + 9.5 * 60.0
    memory_limit = 4 * 1024 * 1024
    for n in configs['sizes']:
        t1 = time.time()
        result = _measure_one(n, configs)
        t2 = time.time()
        DEBUG and print('[*] Time: ' + str(t2 - t1), file=sys.stderr)
        if result is None:
            DEBUG and print('[-] WARNING: Measurement for n = {} ' \
                + 'failed'.format(n), file=sys.stderr)
            break
        with configs['results_file'].open('at') as fh:
            fh.write('\t'.join(map(str, result)))
            fh.write('\n')
        if result[2] > memory_limit:
            DEBUG and print('[-] WARNING: I\'m afraid we\'re ' \
                + 'running out of memory.', file=sys.stderr)
            break
        # if (time_limit - time.time() <= t2 - t1):
        #     DEBUG and print('[-] WARNING: I\'m afraid we\'re ' \
        #         + 'running out of time.', file=sys.stderr)
        #     break
    DEBUG and print('[+] Done!', file=sys.stderr)


def plot_space_time(configs : dict):
    cmd = lambda n : ('{} -e \'testcase="{}"; input_file="{}"; ' \
        + 'output_file="{}"\' {}').format( configs['gnuplot'],
        configs['target'], configs['memory_file'](n), 
        configs['memory_file'](n).with_suffix('.png'),
        'plot-space-time.gnu' )
    for n in configs['sizes']:
        try:
            DEBUG and print('[*] Executing `' + cmd(n) + '`... ', 
                end='', file=sys.stderr)
            subprocess.check_call(cmd(n), shell=True)
            DEBUG and print('Done!', file=sys.stderr)
        except subprocess.CalledProcessError as e:
            print('[-] Error occured: ' + e.output.decode('utf-8'),
                file=sys.stderr)
            raise e

def plot_space_space(configs : dict):
    cmd = ('{gnuplot} -e "testcase=\'{test}\'; ' \
        + 'input_file=\'{input}\'; output_file=\'{output}\'" ' \
        + '{script}').format(
        gnuplot=configs['gnuplot'],
        test=configs['test-name'],
        input=configs['results_file'], 
        output=configs['results_file'].with_suffix('.png'),
        script=configs['root'] / 'plot-space-space.gnu' )
    try:
        DEBUG and print('[*] Executing `' + cmd + '`... ', 
            end='', file=sys.stderr)
        subprocess.check_output(cmd, stderr=subprocess.STDOUT,
            shell=True)
        DEBUG and print('Done!', file=sys.stderr)
    except subprocess.CalledProcessError as e:
        print('\n[-] Error occured: ' + e.output.decode('utf-8'),
            file=sys.stderr)
        raise e

#
# Measure and create a plot of results.
#
def run(configs : dict):
    measure(configs)
    plot_space_space(configs)

