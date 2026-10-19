#!/usr/bin/env python3
"""Check hand-written functions against the ROM and get them ready for apply_auto.py.

Usage: python3 tools/check_c.py [--opt -O1] [--flags -ffast-math] DIR
       python3 tools/apply_auto.py [--dir libc] DIR

DIR holds one <function name>.c per function, without the includes. Each is
compiled and compared the same way as in autodecomp.py, and the ones that
match are listed in DIR/matches.txt, with the place of their .rodata if
they have any. --opt sets the optimization flag (libc and libm are -O1;
put those in src/libc/ with apply_auto.py --dir libc, which the Makefile
builds with -O1). --flags adds compiler flags; a file that needs
-ffast-math gets a line in the Makefile's FASTMATH list.
"""
import glob
import os
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import autodecomp  # noqa: E402


def main():
    args = sys.argv[1:]
    while args[0] in ("--opt", "--flags"):
        if args[0] == "--opt":
            autodecomp.CFLAGS[autodecomp.OPT_INDEX] = args[1]
        else:
            autodecomp.CFLAGS.extend(args[1].split())
        args = args[2:]
    d = args[0]
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
            found.append((vram, size, name, autodecomp.last_rodata))
    with open(os.path.join(d, "matches.txt"), "w") as f:
        for vram, size, name, rodata in sorted(found):
            extra = f" {rodata[0]:08X} {rodata[1]:#06x}" if rodata else ""
            f.write(f"{vram:08X} {size:#06x} {name}{extra}\n")


if __name__ == "__main__":
    main()
