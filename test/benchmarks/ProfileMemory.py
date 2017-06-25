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


DEBUG = False

def parse_command_line():
    parser = argparse.ArgumentParser()
    parser.add_argument('--b2', type=str, default='b2',
        help='Specify path to b2 executable.')
    parser.add_argument('--gnuplot', type=str, default='gnuplot',
        help='Specify path to Gnuplot executable.')
    parser.add_argument('--toolset', type=str,
        help='Choose toolset. Equivalent to "toolset=..." ' \
        + 'when executing b2.')
    parser.add_argument('--variant', type=str,
        help='Choose variant. Equivalent to "variant=..." ' \
        + 'when executing b2.')
    parser.add_argument('--debug', action='store_true', default=False,
        help='Turn debug mode ON.')
    parser.add_argument('--plot-time', action='store_true',
        default=False, help='Make space-time plots.')
    parser.add_argument('--plot-space', action='store_true',
        default=False, help='Make space-space plots.')
    args = parser.parse_args()
    global DEBUG 
    DEBUG = args.debug
    return args

#
# Asks b2 to compile measure_memory.cpp and returns the path to the
# created executable.
#
def get_measure_memory_exe(b2 : str):
    output = None
    func   = r'get_measure_memory_bin'
    try:
        DEBUG and print('[*] Asking b2 for path to measure_memory...',
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
    return re.match(func + r':\s*(.*)$', output[0]).group(1)

#
# Writes data to include_file.
#
def _save_input(data : str, include_file):
    DEBUG and print('[*] Saving data to ' + repr(include_file) \
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
# * configs['measure_memory'] -- path to measure_memory executable as a
# string.
# * configs['b2'] -- "path" to b2 executable as a string.
# * configs['target'] -- target to build as a string.
#
# This function executes measure_memory which in turn builds target
# using b2 and records memory usage. Result is returned as an iterable
# of tuples (time, ram, swap).
#
def _execute_recording(configs : dict):
    dev_null = { 'nt'    : 'NULL'
               , 'posix' : '/dev/null'
               }
    # Call `b2 target`, ignore its output. In the meanwhile, measure
    # the memory usage.
    cmd      = '{} \'{} -a {} > {}\''.format(configs['measure_memory'],
        configs['b2'], configs['target'], dev_null[os.name])
    output   = None
    try:
        DEBUG and print('[*] Executing `' + cmd + '`...', 
            file=sys.stderr)
        output = subprocess.check_output(
            cmd, shell=True).decode('utf-8')
        DEBUG and print('[+] Done!', file=sys.stderr)
    except subprocess.CalledProcessError as e:
        print('[-] Error occured: ' 
            + e.output.decode('utf-8'), file=sys.stderr)
        raise e
    return map(lambda l: tuple(map(int, l.split('\t'))), 
        output.splitlines()[2:])

#
# Given a list of tuples [(time, ram, swap)], returns a tuple of two
# elements: peak ram usage and peak swap usage.
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
    DEBUG and print('[*] Test: size = ' + str(size), file=sys.stderr)
    _save_input(configs['generator'](size), configs['include_file'])
    results = list(_execute_recording(configs))
    _save_memory_usage(results, configs['memory_file'](size))
    return _get_stats(results)

#
# Measure memory usage while compiling target.
#
def measure(configs : dict):
    DEBUG and print('[*] Measuring for ' + repr(configs['target']) \
        + '...', file=sys.stderr)
    if not configs['prefix'].exists():
        configs['prefix'].mkdir(parents=True)
    with configs['results_file'].open('wt') as fh:
        fh.write('# Peak memory usage during function execution\n')
        fh.write('# as a function of input size.\n')
        fh.write('#\n')
        fh.write('size\tram\tswap\n')
    for n in configs['sizes']:
        result = (n,) + _measure_one(n, configs)
        with configs['results_file'].open('at') as fh:
            fh.write('\t'.join(map(str, result)))
            fh.write('\n')
    DEBUG and print('[*] Done!', file=sys.stderr)


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
    cmd = ('{} -e \'testcase="{}"; input_file="{}"; ' \
        + 'output_file="{}"\' {}').format( configs['gnuplot'],
        configs['target'], configs['results_file'], 
        configs['results_file'].with_suffix('.png'),
        'plot-space-space.gnu' )
    try:
        DEBUG and print('[*] Executing `' + cmd + '`... ', 
            end='', file=sys.stderr)
        subprocess.check_call(cmd, shell=True)
        DEBUG and print('Done!', file=sys.stderr)
    except subprocess.CalledProcessError as e:
        print('[-] Error occured: ' + e.output.decode('utf-8'),
            file=sys.stder)
        raise e


