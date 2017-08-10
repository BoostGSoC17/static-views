#!/usr/bin/env python3

import re
import os
import sys
from pathlib import Path
import itertools


# Public configs

DEBUG          = True
INCLUDE_DIRS   = [ '../include' ]
ROOT_HEADER    = 'boost/static_views.hpp'
SINGLE_HEADER_GUARD = 'BOOST_STATIC_VIEWS_ALL_IN_ONE_HPP'
LICENSE_STRING = \
"""\
//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)\
"""



# Internal configs

_simple = lambda name: r'^\s*#\s*' + name + r'\s+(.*)$'
_ifndef_regex     = _simple('ifndef')
_define_regex     = _simple('define')
_include_regex    = _simple('include')
_endif_regex      = _simple('endif')
_general_if_regex = r'^\s*#\s*if.*$'

_is_general_if = lambda l: re.match(_general_if_regex, l) is not None
_is_endif      = lambda l: re.match(_endif_regex, l) is not None

_is_ifndef      = lambda l: re.match(_ifndef_regex, l) is not None
_get_ifndef_arg = lambda l: re.match(_ifndef_regex, l).group(1).strip()

_is_define      = lambda l: re.match(_define_regex, l) is not None
_get_define_arg = lambda l: re.match(_define_regex, l).group(1).strip()

_parse_include   = lambda s: re.match(r'^["<](.+)[">]$', s).group(1)
_is_include      = lambda l: re.match(_include_regex, l) is not None
_get_include_arg = lambda l: re.match(_include_regex, l).group(1).strip()



# cd to `dirname` before calling func and cd back after the call
def push_pop_dir(dirname, func, *args, **kwargs):
    old_dir = os.getcwd()
    os.chdir(dirname)
    DEBUG and print('[+] cd to ' + repr(dirname), file=sys.stderr)
    x = func(*args, **kwargs)
    os.chdir(old_dir)
    DEBUG and print('[+] cd back to ' + repr(old_dir), file=sys.stderr)
    return x


# find the header file in the include path
def find_file(file_name : str):
    DEBUG and print('[*] Searching for ' + repr(file_name) + '...',
        file=sys.stderr)
    matches = list(filter(
        lambda path: (Path(path) / file_name).exists(), INCLUDE_DIRS))
    if len(matches) == 0:
        DEBUG and print('[-] Nothing found.', file=sys.stderr)
        return (None, None)
    elif len(matches) == 1:
        DEBUG and print('[-] Found ' + repr(str(Path(matches[0]) /
            file_name)), file=sys.stderr)
        return (matches[0], file_name)
    raise Exception('Go change header names! '
        + ', '.join(map(lambda f: repr(str(f)), matches))
        + ' all match...')


def include_file(arg, predefs, headers):
    file_name = _parse_include(arg)
    dirname   = None

    if os.path.exists(file_name):
        dirname = os.getcwd()
    else:
        dirname, _ = find_file(file_name)
        if not dirname:
            headers.add(arg)
            return []
    
    full_path = os.path.join(dirname, file_name)
    dirname   = os.path.dirname(full_path)
    file_name = os.path.basename(full_path)
    return push_pop_dir(dirname, expand_file, file_name, predefs,
        headers)


def _extract_body(it):
    class IfsCounter:
        def __init__(self, n : int):
            assert n >= 0
            self.n = n
        def __call__(self, line : str) -> bool:
            assert self.n >= 0
            if _is_general_if(line):
                self.n += 1
            elif _is_endif(line):
                self.n -= 1
            return self.n > 0
    p = IfsCounter(1)
    return list(itertools.takewhile(p, it))


def expand_file(file_name, predefs : set, headers : set):
    assert os.path.exists(file_name)
    f = open(file_name, 'rt')

    # Find the include guard
    f = itertools.dropwhile(lambda l: not _is_ifndef(l), f)
    ifndef_line = next(f)
    define_line = next(f)

    assert _is_ifndef(ifndef_line)
    assert _is_define(define_line)
    assert (_get_define_arg(define_line) ==
        _get_ifndef_arg(ifndef_line))

    x = _get_define_arg(define_line)
    if x in predefs:
        return []
    predefs.add(x)

    # Read everything upto the matching #endif
    body = list(_extract_body(f))

    class Parser:
        def __init__(self):
            self.n = 0
        def __call__(self, line : str):
            if _is_include(line):
                if self.n != 0:
                    return line

                name = _get_include_arg(line)
                dirname = os.path.dirname(file_name)
                return ''.join(include_file(name, predefs, headers))

            if _is_general_if(line):
                self.n += 1
            elif _is_endif(line):
                self.n -= 1
            return line
    p = Parser()
    body = list(map(p, body))
    if len(body) > 0:
        body = \
            [ '\n'
            , '/' * 70 + '\n'
            , '// Expanded from ' + repr(file_name) + '\n'
            , '/' * 70 + '\n'
            ] + body
    return body


if __name__ == '__main__':
    script_dir = os.path.dirname(os.path.realpath(__file__))
    os.chdir(script_dir)

    headers = set()
    predefs = set()

    body = include_file('"' + ROOT_HEADER + '"',
        predefs, headers)

    output = (
        '{license}\n'
      + '\n'
      + '#ifndef {guard}\n'
      + '#define {guard}\n'
      + '\n'
      + '{headers}\n'
      + '\n'
      + '\n'
      + '{body}\n'
      + '\n'
      + '#endif // {guard}\n').format(
        license=LICENSE_STRING,
        guard=SINGLE_HEADER_GUARD,
        headers='\n'.join(map(lambda s: '#include {}'.format(s),
            headers)),
        body=''.join(body))
    
    sys.stdout.write(output)
