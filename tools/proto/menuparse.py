#!/usr/bin/env python3
"""List the prototype's debug menu items: function, call address, adder, label, target and list args.

Usage: python3 tools/proto/menuparse.py proto_text.s baserom_proto.z64

proto_text.s comes from spimdisasm, see docs/prototype.md.
"""
import re, sys
from collections import Counter

rom = open(sys.argv[2], "rb").read()
BASE = 0x80000400 - 0x1000
ins = re.compile(r"/\* \w+ (\w+) \w+ \*/\s+(\S+)\s*(.*)")

def sym_addr(s):
    m = re.fullmatch(r"D_(?:STR_|FLT_)?([0-9A-F]{8})(?:\+0x([0-9A-F]+))?", s)
    return int(m.group(1), 16) + int(m.group(2) or "0", 16) if m else None

def cstr(addr):
    o = addr - BASE
    return rom[o:rom.index(b"\0", o)].decode("ascii", "replace")

lines = []
func = None
for line in open(sys.argv[1]):
    if line.startswith("glabel "):
        func = line.split()[1]
        lines.append(("glabel", func, None, None))
        continue
    m = ins.search(line)
    if m:
        lines.append((func, m.group(1), m.group(2), [x.strip() for x in m.group(3).split(",")] if m.group(3) else []))

regs = {}

def step(op, a, raw):
    if op == "lui" and "%hi(" in raw:
        regs[a[0]] = sym_addr(re.search(r"%hi\(([^)]+)\)", raw).group(1).replace(" ", ""))
    elif op == "addiu" and "%lo(" in raw:
        regs[a[0]] = sym_addr(re.search(r"%lo\(([^)]+)\)", raw).group(1).replace(" ", ""))
    elif op == "addiu" and len(a) == 3 and a[1] in regs and isinstance(regs[a[1]], int):
        regs[a[0]] = regs[a[1]] + int(a[2], 0)
    elif op == "addiu" and len(a) == 3 and a[1] == "$zero":
        regs[a[0]] = ("const", int(a[2], 0))
    elif op == "sw" and len(a) == 2 and a[1] in ("0x10($sp)", "0x14($sp)"):
        regs[a[1]] = 0 if a[0] == "$zero" else regs.get(a[0])
    elif op == "addu" and len(a) == 3 and a[2] == "$zero" and a[1] in regs:
        regs[a[0]] = regs[a[1]]
    elif a and a[0].startswith("$") and op not in ("sw", "sh", "sb", "swc1", "sdc1", "jr", "beq", "bne", "beqz", "bnez", "j", "b", "mtc1"):
        regs.pop(a[0], None)

items = []
i = 0
while i < len(lines):
    func, vaddr, op, a = lines[i]
    if func == "glabel":
        regs = {}
    elif op == "jal":
        if i + 1 < len(lines) and lines[i + 1][0] != "glabel":
            _, _, op2, a2 = lines[i + 1]
            step(op2, a2, ",".join(a2))
            i += 1
        lab, tgt = regs.get("$a1"), regs.get("$a2")
        tgt = tgt if isinstance(tgt, int) else None
        extra = " ".join(str(regs.get(k)) for k in ("0x10($sp)", "0x14($sp)"))
        if isinstance(lab, int) and 0x80000000 <= lab < 0x80100000:
            s = cstr(lab)
            if s and all(32 <= ord(c) < 127 for c in s):
                items.append((func, vaddr, a[0], s, (f"0x{tgt:08X}" if tgt else "-") + " " + extra))
        regs.pop("0x10($sp)", None); regs.pop("0x14($sp)", None)
        for r in ("$a0", "$a1", "$a2", "$a3", "$v0", "$v1", "$at", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9"):
            regs.pop(r, None)
    elif op:
        step(op, a, ",".join(a))
    i += 1

count = Counter(x[2] for x in items)
for f, v, callee, label, target in items:
    if count[callee] >= int(sys.argv[3]) if len(sys.argv) > 3 else count[callee] >= 5:
        print(f"{f} {v} {callee} {label!r:28} {target}")
