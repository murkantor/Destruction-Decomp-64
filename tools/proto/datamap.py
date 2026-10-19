#!/usr/bin/env python3
"""Map prototype data addresses to retail ones through matched functions.

Usage: python3 tools/proto/datamap.py baserom_proto.z64 baserom.z64 funcmap.txt > datamap.txt

funcmap.txt comes from funcmatch.py. Matched functions have the same code
apart from addresses, so where both builds build an address with lui and a
following addiu, load or store, the two addresses are the same variable.
Each output line is: proto address, retail address, number of times seen.
"""
import struct
import sys
from collections import Counter, defaultdict

BASE = 0x80000400 - 0x1000
LO_OPS = {0x09, 0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2B, 0x31, 0x35, 0x39, 0x3D}


def addresses(rom, vram, size):
    """Yield (instruction index, address) for each lui/lo pair in a function."""
    his = {}
    for k in range(0, size, 4):
        w = struct.unpack(">I", rom[vram - BASE + k:vram - BASE + k + 4])[0]
        op, rs, rt, imm = w >> 26, (w >> 21) & 31, (w >> 16) & 31, w & 0xFFFF
        if op == 0x0F:
            his[rt] = imm
        elif op in LO_OPS and rs in his:
            lo = imm - 0x10000 if imm & 0x8000 else imm
            yield k, ((his[rs] << 16) + lo) & 0xFFFFFFFF
            if op == 0x09 and rt == rs:
                his.pop(rs)


def main():
    proto = open(sys.argv[1], "rb").read()
    retail = open(sys.argv[2], "rb").read()
    pairs = defaultdict(Counter)
    for line in open(sys.argv[3]):
        r, size, *p = line.split()
        if len(p) != 1 or p[0] == "-":
            continue
        r, p, size = int(r, 16), int(p[0], 16), int(size, 16)
        ra = dict(addresses(retail, r, size))
        for k, pa in addresses(proto, p, size):
            if k in ra and 0x80000000 <= pa < 0x80400000:
                pairs[pa][ra[k]] += 1
    for pa in sorted(pairs):
        ra, n = pairs[pa].most_common(1)[0]
        print(f"{pa:08X} {ra:08X} {n}")


if __name__ == "__main__":
    main()
