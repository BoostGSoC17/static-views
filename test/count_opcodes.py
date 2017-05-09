#!/usr/bin/python3
# Parse x64 assembler dumps and figure out how many opcodes something takes
#
# File created: (C) 2015 Niall Douglas http://www.nedprod.com/
# File created: June 2015
# File edited by Tom Westerhout in May 2017

import sys, os


debug = False
function = '_Z5test1'


# if len(sys.argv) < 2:
#     print('Usage: ' + sys.argv[0] + ' <assembler file containing test1()>', 
#         file=sys.stderr)
#     sys.exit(1)
# input_file = sys.argv[1]


# objdump format example:
# 0000000000000000 <_Z5test1v>:
#    0:	55                   	push   %rbp
#    1:	48 89 e5             	mov    %rsp,%rbp
#    4:	be 00 00 00 00       	mov    $0x0,%esi
#    9:	bf 00 00 00 00       	mov    $0x0,%edi
#    e:	e8 00 00 00 00       	callq  13 <_Z5test1v+0x13>
#   13:	be 0a 00 00 00       	mov    $0xa,%esi
#   18:	48 89 c7             	mov    %rax,%rdi
#   1b:	e8 00 00 00 00       	callq  20 <_Z5test1v+0x20>
#   20:	90                   	nop
#   21:	5d                   	pop    %rbp
#   22:	c3                   	retq   


def parse_objdump(inputfile : str):
    functions = {}
    this_function=None
    this_function_opcodes=None

    is_new_function = lambda l: l[:4] == '0000'
    is_operation = lambda l: l[0] == ' ' and l[-1] != ':'
    get_function_name = lambda l: l[18:-2]

    for i, line in filter( lambda t: len(t[1]) > 0, 
                   map( lambda t: (t[0], t[1].rstrip()), 
                   enumerate(inputfile, start=1))):

        if debug:
            print("[*] Line " + str(i) + " is '" + line + "'", 
                file=sys.stderr)

        if this_function is None:
            if is_new_function(line):
                this_function = get_function_name(line)
                this_function_opcodes = []
                if debug:
                    print("[*] Line " + str(i) + ": new objdump function: "
                        + this_function, file=sys.stderr)
        elif is_operation(line):
            this_function_opcodes.append(line)
        else:
            if debug:
                print("[*] Storing function " + str(this_function)
                    + " with " + str(len(this_function_opcodes)) 
                    + " lines", file=sys.stderr)
            functions[this_function] = this_function_opcodes
            this_function = None
            if is_new_function(line):
                this_function = get_function_name(line)
                this_function_opcodes = []
                if debug:
                    print("[*] Line " + str(i) + ": new objdump function: "
                        + this_function, file=sys.stderr)
    if this_function is not None:
        if debug:
            print("[*] Storing function " + str(this_function)
                + " with " + str(len(this_function_opcodes)) + " lines",
                file=sys.stderr)
        functions[this_function] = this_function_opcodes
        this_function=None
    return functions


def find_opcodes(name : str, functions : dict):
    is_match = lambda s: s.startswith(name) and ('-0x' not in s)

    _matches = list(filter(lambda t: is_match(t[0]), functions.items()))
    if len(_matches) == 0:
        return name, None
    if len(_matches) > 1 and debug:
        print("[*] Matching functions: ", list(map(lambda t: t[0], _matches)), 
            file=sys.stderr)
    return _matches[-1]


def inline_all_impl(operation : str, functions : dict, black_list : list):
    is_a_call = lambda s: "callq " in s

    def get_name(s : str):
        b = s.find("<")
        e = s.find("+", b)
        if e == -1:
            e = s.find(">", b)
        return s[b+1:e]

    if not is_a_call(operation):
        if debug:
            print("[*] '" + operation + "' is not a call operation.",
                file=sys.stderr)
        return [operation]

    name = get_name(operation) 
    if debug:
        print("[*] Name: " + name, file=sys.stderr)
    
    if (name not in functions) or (name in black_list):
        return [operation]

    black_list.append(name)
    opcodes = [";Expansion of '" + name + "'"] +functions[name]

    return sum(map(lambda op: inline_all_impl(op, functions, black_list), 
        opcodes), [])


def inline_all(name : str, functions : dict):
    return sum(map(lambda op: inline_all_impl(op, functions, [name]), 
        functions[name]), [])


def count_opcodes(input_file : str, function : str):
    #
    # Read all the functions
    #
    functions = {}
    with open(input_file, "rt") as ih:
        functions = parse_objdump(ih)

    #
    # Find the one we're interested in
    #
    name, opcodes = find_opcodes(function, functions)

    #
    # Inline as much as possible
    #
    if opcodes is None:
        print(-1)
        sys.exit(0)
    opcodes = inline_all(name, functions)

    #
    # Save results
    #
    output_file = input_file + '.test1.s'
    try:
        os.remove(output_file)
    except:
        pass
    with open(output_file, "wt") as oh:
        oh.write('\n'.join(opcodes) + '\n')

    #
    # Calculate number of operations
    #
    is_normal_instruction = lambda s: s.startswith('  ') and \
        "data32" not in s and "nopw" not in s and "retq" not in s
    count = sum(map(is_normal_instruction, opcodes))

    return count, opcodes
