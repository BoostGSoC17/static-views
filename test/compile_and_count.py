#!/usr/bin/python3
# File created by Tom Westerhout in May 2017

import sys
import os
import subprocess

import count_opcodes


def _mk_f(format_string : str):
    return lambda s : format_string.format(s)

def _mk_f2(format_string : str):
    return lambda s1, s2 : format_string.format(s1, s2)

def _mk_o(initial_ext: str, final_ext : str):
    return lambda s : s.replace("." + initial_ext, "." + final_ext)



compilers = \
    { "posix"   : [ "gcc", "clang" ]
    , "windows" : ["msvc", "msvc_clang" ]
    }

compile_info = \
    { "gcc"        : (_mk_f("g++ -c -std=c++14 -O3 {}"), _mk_o("cpp", "o"))
    , "clang"      : (_mk_f("~/Downloads/clang+llvm-4.0.0-x86_64-linux-gnu-debian8/bin/clang++ -c -std=c++14 -O3 {}"), _mk_o("cpp", "o"))
    , "msvc"       : (_mk_f("cl /EHsc /c /O2 /GS- /GR /Gy /Zc:inline "
                           + "/D_UNICODE=1 /DUNICODE=1 %%{}"), _mk_o("cpp", "obj"))
    , "msvc_clang" : (_mk_f("clang -std=c++14 -c -O3 -fexceptions "
                           + "-D_UNICODE=1 -DUNICODE=1 %%{}"), _mk_o("cpp", "o"))
    }

disassemble_info = \
    { "gcc"        : (_mk_f2("objdump -d -S {} > {}"), _mk_o("o", "gcc.s"))
    , "clang"      : (_mk_f2("objdump -d -S {} > {}"), _mk_o("o", "clang.s"))
    , "msvc"       : ( _mk_f2("dumpbin /disasm {} > {}"), _mk_o("obj", "msvc.s"))
    , "msvc_clang" : ( _mk_f2("dumpbin /disasm {} > {}"), _mk_o("o", "clang_msvc.s"))
    }

limits = {}






def compile(src_file : str, compiler : str) -> str:
    if src_file is None:
        return None
    print("[*] Compiling '" + src_file + "' with " + compiler + "...", 
        file=sys.stderr)

    command, output = compile_info[compiler]
    try:
        subprocess.check_output(command(src_file), stderr=subprocess.STDOUT, 
            shell=True)
    except subprocess.CalledProcessError as e:
        print("[-] Error while compiling: " + e.output.decode('utf-8'), 
            file=sys.stderr)
        return None
    return output(src_file)


def disassemble(obj_file : str, compiler : str) -> str:
    if obj_file is None:
        return None
    print("[*] Disassembling '" + obj_file + "' with " + compiler + "...", 
        file=sys.stderr)

    command, output = disassemble_info[compiler]
    try:
        subprocess.check_output(command(obj_file, output(obj_file)), 
            stderr=subprocess.STDOUT, shell=True)
    except subprocess.CalledProcessError as e:
        print("[-] Error while dissasembling: " + e.output.decode('utf-8'), 
            file=sys.stderr)
        return None

    try:
        os.remove(obj_file)
    except OSError as e:
        print("[-] Error removing file: " + e.strerror)
        return None
    return output(obj_file)


def test_single(src_file : str, compiler : str, indent : int):
    asm_file = disassemble(compile(src_file, compiler), compiler)
    assert asm_file is not None 

    test_name = src_file.replace(".cpp", "")
    count, opcodes = count_opcodes.count_opcodes(asm_file, count_opcodes.function)
    try:
        os.remove(asm_file)
    except OSError as e:
        print("[-] Error removing file: " + e.strerror)
    
    output = "<![CDATA[\n" + "\n".join(opcodes) + "\n]]"
    xml_string = '  '*indent + '<testcase name="' + test_name + '.' + \
        compiler + '">\n'
    if test_name in limits and limits[test_name] < count:
        xml_string += '  '*(indent+1) + '<failure message="Opcodes generated ' + \
            count + ' exceeds limit"/>\n'
    xml_string += '  '*(indent+2) + '<system-out>\n' + output + '\n' + \
                  '  '*(indent+2) + '</system-out>\n' + \
                  '  '*indent + '</testcase>\n'

    return (test_name, count, xml_string)


def list_src_files():
    return filter(lambda src_file: os.path.isfile(src_file), 
           filter(lambda s: s.endswith(".cpp"), 
               os.listdir()))


def test_all():
    xml_string = '<?xml version="1.0" encoding="UTF-8"?>\n' + \
                 '<testsuite name="constexpr">\n'
    # holds (compiler, name, count) tuples
    csv_data = []
    for src_file in list_src_files():
        print(src_file)
        for compiler in compilers[os.name]:
            name, count, xml_output = test_single(src_file, compiler, 1)
            csv_data.append((compiler, name, count))
            xml_string += xml_output

    with open("results.xml", "wt") as xml_file:
        xml_file.write(xml_string)

    for compiler in compilers[os.name]:
        with open(compiler + ".csv", "wt") as csv_file:
            csv_file.write(','.join( 
                map(lambda t: t[1], 
                filter(lambda t: t[0] == compiler, 
                    csv_data))))
            csv_file.write('\n')
            csv_file.write(','.join( 
                map(lambda t: str(t[2]), 
                filter(lambda t: t[0] == compiler, 
                    csv_data))))


test_all()
