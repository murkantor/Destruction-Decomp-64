#!/usr/bin/env python3
"""Find calls that only work because of the MIPS calling convention.

m2c output declares callees per file, and a declaration that disagrees with the
definition still works on the N64 when the arguments sit in the same registers:
a callee declared `void f(void)` but defined `void f(s32)` reads the caller's
a0. On x86 arguments are on the stack and floats return in st(0), so such
mismatches break. This compiles every game file with GCC's -aux-info, then
compares every declaration with the definition by ABI class: argument count,
int / s64 / float / double / struct by value, and the return class.

Usage: tools/port/check_protos.py [--cc gcc] (from the repo root)
"""
import argparse
import os
import re
import subprocess
import sys
import tempfile
from collections import defaultdict
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

FLAGS = ['-m32', '-fsyntax-only', '-w', '-fpermissive', '-fcommon', '-fgnu89-inline', '-malign-double',
         '-DTARGET_PC', '-DNON_MATCHING', '-Iinclude', '-I.', '-Iport/include']

AUX = re.compile(r'/\* (.+?):(\d+):([A-Z]+) \*/ (.*?);')


def abi_class(t, named=False):
    """Rough ABI class of a parameter or return type string from -aux-info."""
    t = t.strip()
    t = re.sub(r'\b(const|volatile|register|extern|static|inline|__inline__)\b', '', t).strip()
    if named and '(' not in t:
        t = re.sub(r'\b[A-Za-z_]\w*(\s*\[[^]]*\])*$', lambda m: '*' if '[' in m.group(0) else '', t).strip()
    if t in ('', 'void'):
        return 'v'
    if '*' in t or '[' in t or '(' in t:
        return 'i'
    base = ' '.join(t.split())
    if base in ('float', 'f32'):
        return 'f'
    if base in ('double', 'f64', 'long double'):
        return 'd'
    if base in ('long long int', 'long long unsigned int', 'long long', 'unsigned long long',
                's64', 'u64', 'M2C_UNK64', 'OSTime'):
        return 'l'
    if base.startswith(('struct', 'union')) or base in STRUCT_TYPEDEFS:
        return 's'
    return 'i'


STRUCT_TYPEDEFS = set()


def split_params(s):
    out, depth, cur = [], 0, ''
    for ch in s:
        if ch == ',' and depth == 0:
            out.append(cur)
            cur = ''
            continue
        if ch == '(':
            depth += 1
        elif ch == ')':
            depth -= 1
        cur += ch
    if cur.strip():
        out.append(cur)
    return out


def parse_decl(decl, named):
    """'void f (int, float)' -> ('f', ret_class, [param classes] or None if unprototyped)."""
    # Find the function name: the identifier before the first top-level '('
    m = re.match(r'(.*?)\b([A-Za-z_]\w*)\s*\((.*)\)\s*$', decl)
    if not m:
        return None
    ret, name, params = m.group(1), m.group(2), m.group(3)
    if '(' in ret:  # function returning a function pointer; treat as pointer return
        return name, 'i', None
    params = params.strip()
    if params == '':
        plist = None  # unprototyped: ()
    elif params == 'void':
        plist = []
    else:
        plist = [abi_class(p, named) for p in split_params(params) if p.strip() != '...']
        if '...' in params:
            plist.append('...')
    return name, abi_class(ret), plist


def strip_comments(text):
    text = re.sub(r'/\*.*?\*/', lambda m: re.sub(r'[^\n]', ' ', m.group(0)), text, flags=re.S)
    text = re.sub(r'//[^\n]*', '', text)
    return re.sub(r'"(\\.|[^"\\])*"', '""', text)


def call_args(text, start):
    """Count the arguments of the call whose '(' is at text[start]."""
    depth, n, empty = 0, 0, True
    for i in range(start, len(text)):
        ch = text[i]
        if ch in '([{':
            depth += 1
            if depth == 1:
                continue
        elif ch in ')]}':
            depth -= 1
            if depth == 0:
                return 0 if empty else n + 1
        elif ch == ',' and depth == 1:
            n += 1
        if depth >= 1 and not ch.isspace():
            empty = False
    return None


def check_calls(files, defs):
    """Report each call that passes fewer arguments than the callee's definition."""
    count = 0
    names = {n for n, (r, p, loc) in defs.items() if p}
    pat = re.compile(r'\b(' + '|'.join(sorted(names, key=len, reverse=True)) + r')\s*\(')
    for f in files:
        text = strip_comments(Path(f).read_text())
        for m in pat.finditer(text):
            name = m.group(1)
            # skip declarations and the definition: a type word right before the name
            before = text[max(0, m.start() - 40):m.start()]
            if re.search(r'(\b(void|s32|u32|s16|u16|s8|u8|f32|f64|int|char|M2C_UNK\w*|\w+_t|[A-Z]\w*)\s*\**\s*|\*\s*)$', before) \
                    and not re.search(r'(return|=|\(|,|[-+*/&|!<>?:])\s*$', before):
                continue
            n = call_args(text, m.end() - 1)
            ret, params, loc = defs[name]
            need = len([c for c in params if c != '...'])
            if n is not None and n < need and '...' not in params:
                line = text.count('\n', 0, m.start()) + 1
                print(f'{f}:{line}: {name}: {n} args, takes {need}  [def {loc}]')
                count += 1
    print(f'{count} short calls', file=sys.stderr)
    return 0


def run_aux(f, outdir, cc):
    aux = Path(outdir) / (f.replace('/', '_') + '.aux')
    extra = ['-include', 'common.h'] if f.startswith('nonmatching/') else []
    subprocess.run([cc, *FLAGS, *extra, '-aux-info', str(aux), f], check=False,
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return f, aux


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--cc', default='gcc')
    ap.add_argument('--files', help='file with the list of game sources (default: all of src/)')
    ap.add_argument('--calls', action='store_true',
                    help='list call sites passing fewer arguments than the definition takes')
    args = ap.parse_args()

    if args.files:
        files = [l.strip() for l in open(args.files) if l.strip()]
    else:
        files = sorted(str(p) for p in Path('src').rglob('*.c'))

    # Typedef names of structs, so struct-by-value parameters are recognised
    for f in files:
        for m in re.finditer(r'typedef\s+(?:struct|union)\b[^;]*?\}\s*(\w+)\s*;', Path(f).read_text(), re.S):
            STRUCT_TYPEDEFS.add(m.group(1))
    for h in Path('include').glob('*.h'):
        for m in re.finditer(r'typedef\s+(?:struct|union)\b[^;]*?\}\s*(\w+)\s*;', h.read_text(), re.S):
            STRUCT_TYPEDEFS.add(m.group(1))

    defs = {}
    decls = defaultdict(list)
    with tempfile.TemporaryDirectory() as tmp, ThreadPoolExecutor(os.cpu_count()) as ex:
        for f, aux in ex.map(lambda f: run_aux(f, tmp, args.cc), files):
            if not aux.exists():
                print(f'warning: {f} did not compile', file=sys.stderr)
                continue
            for line in aux.read_text().splitlines():
                m = AUX.match(line)
                if not m:
                    continue
                src, lineno, flags, decl = m.groups()
                if not src.startswith(('src/', 'nonmatching/', 'include/')):
                    continue
                src = f if src.startswith('include/') else src
                p = parse_decl(decl, 'F' in flags)
                if p is None:
                    continue
                name, ret, plist = p
                if 'F' in flags:
                    defs[name] = (ret, plist, f'{src}:{lineno}')
                else:
                    decls[name].append((ret, plist, f'{src}:{lineno}', flags))

    if args.calls:
        return check_calls(files, defs)

    problems = 0
    for name, ds in sorted(decls.items()):
        if name not in defs:
            continue
        dret, dparams, dloc = defs[name]
        for ret, plist, loc, flags in ds:
            issues = []
            if ret != dret and not (ret == 'v' or {ret, dret} <= {'i'}):
                issues.append(f'returns {ret} vs {dret}')
            if ret == 'v' and dret in ('f', 'd', 'l', 's'):
                pass  # result ignored by the caller: harmless
            if plist is not None and dparams is not None:
                a = [c for c in plist if c != '...']
                b = [c for c in dparams if c != '...']
                if len(a) != len(b) and '...' not in dparams:
                    issues.append(f'{len(a)} args vs {len(b)}')
                for i, (x, y) in enumerate(zip(a, b)):
                    if x != y and not ({x, y} <= {'i', 'f'}):
                        issues.append(f'arg {i + 1} {x} vs {y}')
                    elif x != y:
                        issues.append(f'arg {i + 1} {x} vs {y} (bits reinterpreted)')
            elif plist is None and dparams:
                if any(c in ('f', 'l', 'd', 's') for c in dparams):
                    issues.append(f'unprototyped, definition takes {dparams}')
            if issues:
                problems += 1
                print(f'{loc}: {name}: {", ".join(issues)}  [def {dloc}]')
    print(f'{problems} suspicious declarations', file=sys.stderr)


if __name__ == '__main__':
    main()
