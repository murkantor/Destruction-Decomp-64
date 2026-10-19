#!/usr/bin/env python3
"""Replace func_XXXXXXXX and D_XXXXXXXX placeholders in src/ with names from symbol_addrs.txt.

Usage: python3 tools/rename.py

Run it after naming symbols, then make extract and make. splat renames the
assembly itself; this keeps the C in step.
"""
import os
import re

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

names = {}
for m in re.finditer(r"^(\w+)\s*=\s*0x([0-9A-Fa-f]+);", open(os.path.join(ROOT, "symbol_addrs.txt")).read(), re.M):
    names[int(m.group(2), 16)] = m.group(1)

pattern = re.compile(r"\b(?:func|D)_([0-9A-F]{8})\b")
changed = 0
for d, _, files in os.walk(os.path.join(ROOT, "src")):
    for f in files:
        if not f.endswith((".c", ".h")):
            continue
        path = os.path.join(d, f)
        text = open(path).read()
        new = pattern.sub(lambda m: names.get(int(m.group(1), 16), m.group(0)), text)
        if new != text:
            open(path, "w").write(new)
            changed += 1
print(f"{changed} files updated")
