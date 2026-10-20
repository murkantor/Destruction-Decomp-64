#!/usr/bin/env python3
"""Find functions whose struct return or struct-by-value parameters differ in size
between the files that declare or define them.

m2c types each file's declarations separately, so a caller may declare
`Pos2 f()` (8 bytes) for a function defined `Vec3f f()` (12 bytes). On the N64
and on 32-bit Linux every struct comes back through a hidden pointer, so the
caller only loses or overflows a few bytes; on Windows (MinGW) structs of 1, 2,
4 or 8 bytes come back in registers, so the two sides disagree about the whole
call. Struct parameters of different sizes shift every later argument on all PCs.

Reads the types from the DWARF of the compiled game objects (the PC build).
Usage: check_struct_sizes.py [--objdump objdump] @objects.txt | objects...
"""
import argparse
import os
import sys
from collections import defaultdict
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import subprocess

sys.path.insert(0, str(Path(__file__).parent))
from gen_widths import parse, Layout, attr_name, attr_ref  # noqa: E402


def struct_size(lay, off):
    """Byte size if off is (a typedef of) a struct or union, else None."""
    depth = 0
    while off is not None and depth < 40:
        tag, a, _ = lay.dies.get(off, (None, {}, []))
        if tag in ('DW_TAG_typedef', 'DW_TAG_const_type', 'DW_TAG_volatile_type'):
            off = attr_ref(a)
            depth += 1
            continue
        if tag in ('DW_TAG_structure_type', 'DW_TAG_union_type'):
            return lay.size(off)
        return None
    return None


def signatures(obj, objdump):
    text = subprocess.run([objdump, '--dwarf=info', obj], capture_output=True, text=True, check=False).stdout
    dies, top = parse(text)
    lay = Layout(dies)
    out = []
    for off in top:
        tag, a, kids = dies[off]
        if tag != 'DW_TAG_subprogram':
            continue
        name = attr_name(a)
        if not name:
            continue
        ret = struct_size(lay, attr_ref(a))
        # the argument area as the caller lays it out: each argument's size, rounded
        # to 4 bytes, and where structs start (only the byte layout matters on x86)
        params = []
        for k in kids:
            ktag, ka, _ = dies[k]
            if ktag == 'DW_TAG_formal_parameter':
                t = attr_ref(ka)
                size = lay.size(t) or 4
                params.append((size + 3) & ~3)
        out.append((name, ret, tuple(params), 'DW_AT_declaration' not in a, obj))
    return out


def _prefix_bytes(sizes, limit):
    """Whether the first `limit` bytes of the argument area are whole arguments only."""
    total, cuts = 0, []
    for s in sizes:
        if total >= limit:
            break
        total += s
        cuts.append(total)
    return total == limit


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--objdump', default='objdump')
    ap.add_argument('objects', nargs='+')
    args = ap.parse_args()
    objects = []
    for o in args.objects:
        objects += Path(o[1:]).read_text().split() if o.startswith('@') else [o]

    seen = defaultdict(list)
    with ThreadPoolExecutor(os.cpu_count()) as ex:
        for sigs in ex.map(lambda o: signatures(o, args.objdump), objects):
            for name, ret, params, is_def, obj in sigs:
                seen[name].append((ret, params, is_def, obj))

    problems = 0
    for name, entries in sorted(seen.items()):
        defs = [e for e in entries if e[2]]
        ref = defs[0] if defs else entries[0]
        for ret, params, is_def, obj in entries:
            issues = []
            if ret != ref[0] and (ret is not None or ref[0] is not None):
                issues.append(f'returns struct of {ret} bytes vs {ref[0]}')
            # compare the argument bytes the shorter list covers (extra arguments are
            # harmless with cdecl; missing ones are check_protos.py --calls)
            a, b = sum(params), sum(ref[1])
            if params and ref[1] and min(a, b) and \
                    _prefix_bytes(params, min(a, b)) != _prefix_bytes(ref[1], min(a, b)):
                issues.append(f'argument layout {list(params)} vs {list(ref[1])}')
            if issues:
                problems += 1
                where = Path(obj).name.replace('.c.o', '.c').replace('.c.obj', '.c')
                print(f'{where}: {name}: {", ".join(issues)}  [{"definition" if defs else "first declaration"} in '
                      f'{Path(ref[3]).name.replace(".c.o", ".c")}]')
    print(f'{problems} mismatches', file=sys.stderr)
    return 1 if problems else 0


if __name__ == '__main__':
    sys.exit(main())
