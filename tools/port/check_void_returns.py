#!/usr/bin/env python3
"""Find functions the C defines as void whose callers use v0 after the call.

m2c types a function void when it cannot see a return value, but on MIPS the
caller can still read v0, left by the callee's own last call (a pass-through)
or computation. Such callers break on x86. This disassembles the matching build
and, for every jal to a void-defined function, checks whether the caller reads
v0 before writing it.

Usage: tools/port/check_void_returns.py (from the repo root, after make)
"""
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import check_protos  # noqa: E402


def void_functions():
    """Names of functions whose C definition returns void."""
    import tempfile
    import os
    from concurrent.futures import ThreadPoolExecutor
    files = sorted(str(p) for p in Path('src').rglob('*.c'))
    result = {}
    with tempfile.TemporaryDirectory() as tmp, ThreadPoolExecutor(os.cpu_count()) as ex:
        for f, aux in ex.map(lambda f: check_protos.run_aux(f, tmp, 'gcc'), files):
            if not aux.exists():
                continue
            for line in aux.read_text().splitlines():
                m = check_protos.AUX.match(line)
                if not m or 'F' not in m.group(3):
                    continue
                p = check_protos.parse_decl(m.group(4), True)
                if p and p[1] == 'v':
                    result[p[0]] = f'{m.group(1)}:{m.group(2)}'
    return result


def main():
    voids = void_functions()
    dis = subprocess.run(['mips-linux-gnu-objdump', '-d', '--no-show-raw-insn', 'build/dd64.elf'],
                         capture_output=True, text=True).stdout
    func = None
    insns = defaultdict(list)
    for line in dis.splitlines():
        m = re.match(r'^[0-9a-f]+ <(\w+)>:', line)
        if m:
            func = m.group(1)
            continue
        m = re.match(r'\s*([0-9a-f]+):\s+(\S+)\s*(.*)', line)
        if m and func:
            insns[func].append((int(m.group(1), 16), m.group(2), m.group(3)))

    used = defaultdict(set)
    indirect = set()
    for caller, code in insns.items():
        for i, (addr, op, args) in enumerate(code):
            if op == 'jalr':
                callee = '(indirect)'
            elif op != 'jal':
                continue
            else:
                m = re.search(r'<(\w+)>', args)
                if not m or m.group(1) not in voids:
                    continue
                callee = m.group(1)
            # scan after the delay slot until v0 is written or read
            for addr2, op2, args2 in code[i + 2:i + 40]:
                regs = [r.strip() for r in re.split(r'[,()]', args2) if r.strip()]
                if not regs:
                    if op2 in ('jr',) or op2.startswith('b'):
                        break
                    continue
                reads = regs[1:] if op2 not in ('sw', 'sh', 'sb', 'swc1', 'sdc1', 'beq', 'bne', 'beqz', 'bnez',
                                                'bgez', 'bltz', 'blez', 'bgtz', 'jr', 'mtc1') else regs
                if op2 in ('jr',) and regs == ['ra']:
                    # tail position: the caller returns v0 unchanged
                    used[callee].add(f'{caller}@{addr:08X} (returned)')
                    break
                if 'v0' in reads:
                    used[callee].add(f'{caller}@{addr:08X}')
                    break
                if regs and regs[0] == 'v0' and op2 not in ('sw', 'sh', 'sb', 'beq', 'bne', 'beqz', 'bnez'):
                    break
                if op2 == 'jal' or op2 == 'jalr' or op2 == 'j':
                    break
    # A tail call only matters if the caller's own result is used, so follow
    # "returned" chains up to a real use.
    def really_used(name, seen=()):
        if name in seen:
            return False
        for site in used.get(name, ()):
            caller = site.split('@')[0]
            if not site.endswith('(returned)'):
                return True
            if caller in voids and really_used(caller, seen + (name,)):
                return True
        return False

    n = 0
    for callee in sorted(used):
        if callee == '(indirect)':
            continue
        if really_used(callee):
            n += 1
            print(f'{callee} [{voids[callee]}]: {", ".join(sorted(used[callee]))}')
    print('\nIndirect calls whose result is used, by caller:')
    for site in sorted(used.get('(indirect)', ())):
        if not site.endswith('(returned)'):
            print('   ', site)
    print(f'{n} void functions whose result is used', file=sys.stderr)


if __name__ == '__main__':
    main()
