#!/usr/bin/env python3
"""Match functions between the prototype and retail by their code with addresses masked.

Usage: python3 tools/proto/funcmatch.py baserom_proto.z64 baserom.z64 funcmap.txt

Each line of funcmap.txt is: retail address, size, then the prototype address(es) with
the same masked code, or "-" when there is none.
"""
import sys, struct, hashlib
from collections import defaultdict

BASE = 0x80000400 - 0x1000
RSPBOOT = bytes.fromhex("0900041920010fc0")

def text(d):
    end = d.index(RSPBOOT, 0x1050)
    return 0x1050, end

def mask(w):
    op = w >> 26
    if op in (2, 3):                       # j, jal
        return w & 0xFC000000
    if op in (0x0F, 0x09, 0x0D, 0x23, 0x2B, 0x21, 0x29, 0x20, 0x28, 0x24, 0x25, 0x31, 0x39, 0x35, 0x3D, 0x37, 0x3F):
        return w & 0xFFFF0000              # lui, addiu, ori, loads, stores: may hold addresses
    return w

def funcs(d):
    lo, hi = text(d)
    ws = struct.unpack(">%dI" % ((hi - lo) // 4), d[lo:hi])
    starts, i = [0], 0
    while i < len(ws):
        if ws[i] == 0x03E00008:
            j = i + 2
            while j < len(ws) and ws[j] == 0:
                j += 1
            if j < len(ws):
                starts.append(j)
            i = j
        else:
            i += 1
    out = []
    for a, b in zip(starts, starts[1:] + [len(ws)]):
        body = ws[a:b]
        while body and body[-1] == 0:
            body = body[:-1]
        if body:
            h = hashlib.sha1(struct.pack(">%dI" % len(body), *map(mask, body))).hexdigest()
            out.append((lo + a * 4 + BASE, len(body) * 4, h))
    return out, (lo, hi)

p = open(sys.argv[1], "rb").read()
r = open(sys.argv[2], "rb").read()
pf, pr = funcs(p)
rf, rr = funcs(r)
byh = defaultdict(list)
for f in pf:
    byh[f[2]].append(f)
uniq = sum(1 for f in rf if len(byh[f[2]]) == 1)
same = sum(1 for f in rf if byh[f[2]])
print(f"proto text 0x{pr[0]:X}-0x{pr[1]:X} ({len(pf)} funcs), retail text 0x{rr[0]:X}-0x{rr[1]:X} ({len(rf)} funcs)")
print(f"retail functions with identical code in proto: {same} ({100*same/len(rf):.0f}%), unique match: {uniq}")
bytes_same = sum(f[1] for f in rf if byh[f[2]])
print(f"by size: {100*bytes_same/sum(f[1] for f in rf):.0f}% of retail code")
with open(sys.argv[3], "w") as fh:
    for f in rf:
        m = byh[f[2]]
        fh.write(f"{f[0]:08X} {f[1]:5X} " + (" ".join(f"{x[0]:08X}" for x in m) if m else "-") + "\n")
