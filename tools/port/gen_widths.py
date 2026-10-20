#!/usr/bin/env python3
"""Generate the table of static data the PC port converts to host order at boot.

The game's static data (.data/.rodata, and the blocks Boot_LoadData loads) is
big-endian, and is normally converted where the game first reads it
(port/src/shadow.c). That cannot work for data the game copies into a local
first (`Vec3f up = D_800B8C10;`): GCC does not instrument a function's own
accesses to its locals. So every global the game's C names gets converted at
boot by its declared type: this reads the types from the DWARF of the compiled
game objects, lays each global out down to its scalar fields, and writes the
fields' addresses and widths. Bytes that declarations disagree about (two
files giving different fields, unions with fields of different widths,
bitfields) are left out and stay converted lazily.

Usage: gen_widths.py --elf build/dd64.elf --objdump objdump --out dd64_widths.c objects...
(object lists may be given as @file). The output holds only addresses and widths.
"""
import argparse
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from gen_data import HOST_RENAMES, read_symbols  # noqa: E402

RAM_LO, RAM_HI = 0x8007ADD0, 0x80400000
UNKNOWN = 0  # a byte declarations disagree about

DIE = re.compile(r'^\s*<(\d+)><([0-9a-f]+)>: Abbrev Number: (\d+)(?: \((\w+)\))?')
ATTR = re.compile(r'^\s*<[0-9a-f]+>\s+(DW_AT_\w+)\s*:\s*(.*)$')
REF = re.compile(r'<0x([0-9a-f]+)>')


def parse(text):
    """DIEs by offset: {off: [tag, attrs, children]}; returns (dies, top-level offsets)."""
    dies, stack, top = {}, [], []
    cur = None
    for line in text.splitlines():
        m = DIE.match(line)
        if m:
            depth, off, abbrev, tag = int(m.group(1)), int(m.group(2), 16), m.group(3), m.group(4)
            if abbrev == '0':  # end of children
                cur = None
                continue
            cur = [tag, {}, []]
            dies[off] = cur
            del stack[depth:]
            if depth >= 1 and len(stack) >= depth:
                dies[stack[depth - 1]][2].append(off)
            if depth == 1:
                top.append(off)
            stack.append(off)
            continue
        m = ATTR.match(line)
        if m and cur is not None:
            cur[1][m.group(1)] = m.group(2).strip()
    return dies, top


def attr_name(a):
    v = a.get('DW_AT_name')
    if v is None:
        return None
    return v.rsplit(': ', 1)[-1].strip() if v.startswith('(') else v


def attr_int(a, key):
    v = a.get(key)
    if v is None:
        return None
    m = re.match(r'(0x[0-9a-f]+|\d+)', v.rsplit(': ', 1)[-1].strip() if v.startswith('(') else v)
    return int(m.group(1), 0) if m else None


def attr_ref(a, key='DW_AT_type'):
    v = a.get(key)
    if v is None:
        return None
    m = REF.search(v)
    return int(m.group(1), 16) if m else None


class Layout:
    def __init__(self, dies):
        self.dies = dies

    def size(self, off, depth=0):
        if off is None or depth > 40:
            return None
        tag, a, kids = self.dies.get(off, (None, {}, []))
        if tag is None:
            return None
        n = attr_int(a, 'DW_AT_byte_size')
        if n is not None:
            return n
        if tag in ('DW_TAG_typedef', 'DW_TAG_const_type', 'DW_TAG_volatile_type', 'DW_TAG_restrict_type',
                   'DW_TAG_atomic_type'):
            return self.size(attr_ref(a), depth + 1)
        if tag == 'DW_TAG_pointer_type':
            return 4
        if tag == 'DW_TAG_array_type':
            count = self.count(kids)
            es = self.size(attr_ref(a), depth + 1)
            return None if count is None or es is None else count * es
        return None

    def count(self, kids):
        total = 1
        subs = [k for k in kids if self.dies[k][0] == 'DW_TAG_subrange_type']
        if not subs:
            return None
        for k in subs:
            a = self.dies[k][1]
            n = attr_int(a, 'DW_AT_count')
            if n is None:
                ub = attr_int(a, 'DW_AT_upper_bound')
                if ub is None:
                    return None
                n = ub + 1
            total *= n
        return total

    def leaves(self, off, base, out, depth=0):
        """Append (offset, width) for each scalar; width UNKNOWN for bytes to leave alone."""
        if off is None or depth > 40:
            return
        tag, a, kids = self.dies.get(off, (None, {}, []))
        if tag in ('DW_TAG_base_type', 'DW_TAG_enumeration_type'):
            n = attr_int(a, 'DW_AT_byte_size')
            if n:
                out.append((base, n))
        elif tag == 'DW_TAG_pointer_type':
            out.append((base, 4))
        elif tag in ('DW_TAG_typedef', 'DW_TAG_const_type', 'DW_TAG_volatile_type', 'DW_TAG_restrict_type',
                     'DW_TAG_atomic_type'):
            self.leaves(attr_ref(a), base, out, depth + 1)
        elif tag in ('DW_TAG_structure_type', 'DW_TAG_union_type'):
            if 'DW_AT_declaration' in a:
                return
            for k in kids:
                ktag, ka, _ = self.dies[k]
                if ktag != 'DW_TAG_member':
                    continue
                mo = attr_int(ka, 'DW_AT_data_member_location') or 0
                if 'DW_AT_bit_size' in ka or 'DW_AT_data_bit_offset' in ka:
                    n = self.size(attr_ref(ka)) or 4
                    for i in range(n):
                        out.append((base + mo + i, UNKNOWN))
                    continue
                self.leaves(attr_ref(ka), base + mo, out, depth + 1)
        elif tag == 'DW_TAG_array_type':
            count = self.count(kids)
            et = attr_ref(a)
            es = self.size(et)
            if count is None or not es:
                return
            one = []
            self.leaves(et, 0, one, depth + 1)
            for i in range(count):
                out.extend((base + i * es + o, w) for o, w in one)


def globals_of(obj, objdump):
    text = subprocess.run([objdump, '--dwarf=info', obj], capture_output=True, text=True, check=False).stdout
    dies, top = parse(text)
    lay = Layout(dies)
    result = []
    for off in top:
        tag, a, _ = dies[off]
        if tag != 'DW_TAG_variable':
            continue
        name = attr_name(a)
        if not name:
            continue
        out = []
        lay.leaves(attr_ref(a), 0, out)
        if out:
            result.append((name, out))
    return result


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--elf', required=True)
    ap.add_argument('--objdump', default='objdump')
    ap.add_argument('--out', required=True)
    ap.add_argument('objects', nargs='+')
    args = ap.parse_args()

    objects = []
    for o in args.objects:
        if o.startswith('@'):
            objects += Path(o[1:]).read_text().split()
        else:
            objects += o.split(';')
    objects = [o for o in objects if o]

    unrename = {v: k for k, v in HOST_RENAMES.items()}
    addrs = {}
    for name, value, typ, bind in read_symbols(args.elf):
        if name and typ != 2 and RAM_LO <= value < RAM_HI:
            addrs.setdefault(name, value)

    # Every byte: (unit start, width), or UNKNOWN where declarations disagree
    bytemap = {}

    def put(addr, width):
        if width == UNKNOWN:
            bytemap[addr] = UNKNOWN
            return
        unit = (addr, width)
        for b in range(addr, addr + width):
            old = bytemap.get(b)
            if old is None:
                bytemap[b] = unit
            elif old != unit:
                bytemap[b] = UNKNOWN
                if old != UNKNOWN:
                    for c in range(old[0], old[0] + old[1]):
                        bytemap[c] = UNKNOWN
                for c in range(addr, addr + width):
                    bytemap[c] = UNKNOWN
                return

    with ThreadPoolExecutor(os.cpu_count()) as ex:
        for result in ex.map(lambda o: globals_of(o, args.objdump), objects):
            for name, leaves in result:
                addr = addrs.get(unrename.get(name, name))
                if addr is None:
                    continue
                for off, width in leaves:
                    put(addr + off, width)

    units = sorted({v for v in bytemap.values() if v != UNKNOWN and v[1] in (2, 4, 8)})
    units = [u for u in units if all(bytemap.get(b) == u for b in range(u[0], u[0] + u[1]))]
    runs = []
    for addr, width in units:
        if runs and runs[-1][1] == width and runs[-1][0] + runs[-1][2] * width == addr and runs[-1][2] < 0xFFFFFF:
            runs[-1][2] += 1
        else:
            runs.append([addr, width, 1])

    c = ['/* Generated by tools/port/gen_widths.py: static data the port converts at boot. */',
         '#include <stdint.h>', '',
         '/* address, then width << 24 | count of consecutive values */',
         'const uint32_t gPortStaticWidths[][2] = {']
    c += [f'    {{ 0x{a:08X}, 0x{w << 24 | n:08X} }},' for a, w, n in runs]
    c += ['};', 'const uint32_t gPortStaticWidthCount = sizeof(gPortStaticWidths) / sizeof(gPortStaticWidths[0]);', '']
    Path(args.out).write_text('\n'.join(c))
    conflicts = sum(1 for v in bytemap.values() if v == UNKNOWN)
    print(f'gen_widths: {len(units)} values in {len(runs)} runs; {conflicts} bytes left to lazy conversion',
          file=sys.stderr)


if __name__ == '__main__':
    main()
