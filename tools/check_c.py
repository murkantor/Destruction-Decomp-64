#!/usr/bin/env python3
"""Check hand-written functions against the ROM and get them ready for apply_auto.py.

Usage: python3 tools/check_c.py DIR
       python3 tools/apply_auto.py DIR

DIR holds one <function name>.c per function, without the includes. Each is
compiled and compared the same way as in autodecomp.py, and the ones that
match are listed in DIR/matches.txt.
"""
import glob
import os
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import autodecomp  # noqa: E402


def main():
    d = sys.argv[1]
    autodecomp.init(open(os.path.join(ROOT, "baserom.z64"), "rb").read(), autodecomp.load_symbols(), None)
    info = {}
    for path in glob.glob(os.path.join(ROOT, "asm", "**", "*.s"), recursive=True):
        for name, vram, size, _ in autodecomp.functions(path):
            info[name] = (vram, size)
    found = []
    for path in sorted(glob.glob(os.path.join(d, "*.c"))):
        name = os.path.basename(path)[:-2]
        if name not in info:
            print(f"{name}: not in asm/")
            continue
        vram, size = info[name]
        with tempfile.TemporaryDirectory() as work:
            ok, why, code, diffs = autodecomp.attempt(open(path).read(), work, name, vram, size)
        print(f"{name}: {'match' if ok else why + (f' ({diffs} words)' if diffs else '')}")
        if ok:
            open(path, "w").write(code)
            found.append((vram, size, name))
    with open(os.path.join(d, "matches.txt"), "w") as f:
        for vram, size, name in sorted(found):
            f.write(f"{vram:08X} {size:#06x} {name}\n")


if __name__ == "__main__":
    main()
