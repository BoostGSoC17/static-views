#!/usr/bin/python3

#          Copyright Tom Westerhout 2017.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)


import sys, os, re, argparse, pathlib, subprocess
from pathlib import Path
from CountOpcodes import count_opcodes


DEBUG = False

def _mk_f(format_string : str):
    return lambda s1, s2 : format_string.format(s1, s2)

_disassemble_info_ = \
    { 'posix' : (_mk_f("objdump -rd {} > {}"))
    , 'nt'    : (_mk_f("dumpbin /disasm {} > {}"))
    }


def disassemble(exe_file : str) -> str:
    if exe_file is None:
        return None
    DEBUG and print("[*] Disassembling " + repr(exe_file) + "...",
        file=sys.stderr, end='')

    exe = Path(exe_file)
    asm = exe.parent / (exe.stem + '.s')
    command = _disassemble_info_[os.name](str(exe), str(asm))

    try:
        subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True)
    except subprocess.CalledProcessError as e:
        print("\n[-] Error while dissasembling: " + e.output.decode('utf-8'), 
            file=sys.stderr)
        return None

    DEBUG and print("Done!", file=sys.stderr)
    return str(asm)


def check(test_name : str, exe_file : str, func_name : str,
    limit : int, toolset : str):

    asm_file  = disassemble(exe_file)
    count, opcodes = count_opcodes(asm_file, func_name)
    if count == -1:
        print('No call to ' + repr(func_name) + ' found.', file=sys.stdout)
        sys.exit(1)
   
    passed  = True
    message = 'Success'
    if limit != -1 and count > limit:
        passed  = False
        message = 'Opcodes generated ' + str(count) \
            + ' exceeds limit'

    xml_string = '\n'.join(
        [ '<testcase name={test_name}>'
        , '  <os>{os}</os>'
        , '  <toolset>{toolset}</toolset>'
        , '  <status>'
        , '    <passed>{passed}</passed>'
        , '    <message>{message}</message>'
        , '  </status>'
        , '  <opcodes count={count}>'
        , '<![CDATA['
        ] + opcodes + \
        [ ']]>'
        , '  </opcodes>'
        , '</testcase>'
        ]).format(
            test_name=test_name,
            os=os.name,
            toolset=toolset,
            passed=passed,
            message=message,
            count=count)
   
    return (passed, xml_string)


def save_xml(xml_string : str, name : str, toolset : str,
    output : str):

    xml_file = Path(output) / '{name}.{cc}.xml'.format(name=name,
        cc=toolset)
    with xml_file.open('wt') as fh:
        fh.write(xml_string + '\n')


def parse_command_line():
    parser = argparse.ArgumentParser()
    parser.add_argument('--name', required=True,
        help='Testcase.')
    parser.add_argument('--file', required=True,
        help='Executable file.')
    parser.add_argument('--function', required=True,
        help='Function name.')
    parser.add_argument('--limit', required=True, type=int,
        help='Upper bound on the number of opcodes.')
    parser.add_argument('--toolset', required=True,
        help='Toolset.')
    parser.add_argument('--output', required=True,
        help='Output directory.')
    parser.add_argument('--debug', type=bool, default=False,
        help='Turn debug mode ON.')
    args = parser.parse_args()

    global DEBUG
    DEBUG = args.debug
    return args


def main():
    args = parse_command_line() 
    (passed, xml_string) = check(args.name, args.file, args.function,
        args.limit, args.toolset)
    save_xml(xml_string, args.name, args.toolset, args.output)

    error_code = 0 if passed else 1
    DEBUG and print(xml_string, file=sys.stderr)
    sys.exit(error_code)


if __name__ == '__main__':
    main()
