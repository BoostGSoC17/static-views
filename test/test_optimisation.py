#!/usr/bin/python3

#          Copyright Tom Westerhout 2017.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)


#
# Utility to test compiler optimisation
#
# usage: test_optimisation.py [-h] [--debug DEBUG] function filename
# 
# positional arguments:
#   function       Function name.
#   filename       Executable file.
# 
# optional arguments:
#   -h, --help     show this help message and exit
#   --debug DEBUG  Turn debug mode ON.
#
# This program disassembles 'filename' using a platform specific tool,
# then counts the number of operations 'function' takes and checks
# whether it exceeds the limit defined in 'Limits.py'.
#
# **IMPORTANT**: This tool relies on the 'TOOLSET' environment variable!
#




import sys, os, re, argparse, pathlib, subprocess
from CountOpcodes import count_opcodes
from Limits import limits


def _mk_f(format_string : str):
    return lambda s1, s2 : format_string.format(s1, s2)

_disassemble_info_ = \
    { 'posix' : (_mk_f("objdump -rd {} > {}"))
    , 'nt'    : (_mk_f("dumpbin /disasm {} > {}"))
    }


def disassemble(exe_file : str) -> str:
    if exe_file is None:
        return None
    print("[*] Disassembling " + repr(exe_file) + "...", file=sys.stderr, end='')

    exe = pathlib.PurePath(exe_file)
    asm = exe.parent / (exe.stem + '.s')
    command = _disassemble_info_[os.name](str(exe), str(asm))

    try:
        subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True)
    except subprocess.CalledProcessError as e:
        print("\n[-] Error while dissasembling: " + e.output.decode('utf-8'), 
            file=sys.stderr)
        return None

    print("Done!")
    return str(asm)


def test_single(exe_file : str, func : str) -> tuple:
    asm_file = disassemble(exe_file)
    test_name = pathlib.PurePath(asm_file).stem
    count, opcodes = count_opcodes(asm_file, func)
    if count == -1:
        print('No call to ' + repr(func) + ' found.', file=sys.stdout)
        sys.exit(1)
   
    code = 0
    message = 'success'
    if test_name in limits and limits[test_name][os.environ['TOOLSET']] < count:
        code = 1
        message = 'failure message="Opcodes generated ' + str(count) \
            + ' exceeds limit"'
    xml_string = '\n'.join(
        [ '<testcase name={}>'
        , '  <{}/>'
        , '    <opcodes count={}>'
        , '<![CDATA['
        ] + opcodes + \
        [ ']]>'
        , '    </opcodes>'
        , '</testcase>'
        ]).format(test_name, message, count)

    return (test_name, code, xml_string)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('function', help='Function name.')
    parser.add_argument('filename', help='Executable file.')
    parser.add_argument('--debug', type=bool, default=False,
        help='Turn debug mode ON.')
    args = parser.parse_args()
    
    result = test_single(args.filename, args.function)
    print(result[2])
    sys.exit(result[1])


if __name__ == '__main__':
    main()
