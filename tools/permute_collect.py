#!/usr/bin/env python3
"""Collect decomp-permuter results that match and get them ready for apply_auto.py.

Usage: python3 tools/permute_collect.py [--permute build/permute] [--near build/auto/near] [--out build/permuted]
       python3 tools/apply_auto.py build/permuted

For every directory with a zero-score output, the permuted function replaces
the one in the m2c file it came from, so the includes and declarations stay
readable. The result is checked again with the same comparison as
autodecomp.py before it is kept.
"""
import argparse
import glob
import os
import re
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import autodecomp  # noqa: E402


def function_span(code, name):
    """Start and end of the definition of name in code."""
    m = re.search(r"^[^\n;]*\b" + re.escape(name) + r"\([^;{]*\)\s*\{", code, re.M)
    if not m:
        return None
    depth, i = 0, m.end() - 1
    while i < len(code):
        if code[i] == "{":
            depth += 1
        elif code[i] == "}":
            depth -= 1
            if depth == 0:
                return m.start(), i + 1
        i += 1
    return None


def carry_declarations(code, perm_head):
    """The permuter may change extern types and prototypes; take its versions."""
    out = []
    for line in code.splitlines():
        m = re.match(r"^(?:extern\s+)?[^;(]*?\b(\w+)\s*(\(.*\))?;", line)
        if m and not line.startswith(" "):
            name = m.group(1)
            found = re.findall(r"^[^\n;]*\b" + re.escape(name) + r"\b[^\n]*;", perm_head, re.M)
            if found:
                line = found[-1]
        out.append(line)
    return "\n".join(out) + "\n"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--permute", default=os.path.join(ROOT, "build", "permute"))
    ap.add_argument("--near", default=os.path.join(ROOT, "build", "auto", "near"))
    ap.add_argument("--out", default=os.path.join(ROOT, "build", "permuted"))
    args = ap.parse_args()

    info = {}
    for path in glob.glob(os.path.join(ROOT, "asm", "**", "*.s"), recursive=True):
        for name, vram, size, _ in autodecomp.functions(path):
            info[name] = (vram, size)
    autodecomp.init(open(os.path.join(ROOT, "baserom.z64"), "rb").read(), autodecomp.load_symbols(), None)

    os.makedirs(args.out, exist_ok=True)
    found = []
    for d in sorted(glob.glob(os.path.join(args.permute, "*"))):
        name = os.path.basename(d)
        outs = sorted(glob.glob(os.path.join(d, "output-0-*", "source.c")))
        near = glob.glob(os.path.join(args.near, "*_" + name + ".c"))
        if not outs or not near or name not in info:
            continue
        orig = open(near[0]).read()
        a = function_span(orig, name)
        for out in outs:
            perm = open(out).read()
            b = function_span(perm, name)
            if not a or not b:
                continue
            code = orig[:a[0]] + perm[b[0]:b[1]] + orig[a[1]:]
            code = carry_declarations(code, perm[:b[0]])
            vram, size = info[name]
            with tempfile.TemporaryDirectory() as work:
                ok, why, code, _ = autodecomp.attempt(code, work, name, vram, size)
            if ok:
                open(os.path.join(args.out, name + ".c"), "w").write(code)
                found.append((vram, size, name))
                break
            print(f"{name}: permuted output does not match after splicing ({why})")
    found.sort()
    with open(os.path.join(args.out, "matches.txt"), "w") as f:
        for vram, size, name in found:
            f.write(f"{vram:08X} {size:#06x} {name}\n")
    print(f"{len(found)} matching functions in {args.out}")


if __name__ == "__main__":
    main()
