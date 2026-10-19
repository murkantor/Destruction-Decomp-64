#!/usr/bin/env python3
"""Set up decomp-permuter directories for the near misses autodecomp.py saved.

Usage: python3 tools/permute_setup.py [--max-diffs 3] [--near build/auto/near] [--out build/permute]

Each directory gets base.c (preprocessed), target.o (the original function,
assembled from asm/), compile.sh (the Makefile's pipeline for files that are all C) and
settings.toml. Then run, for example:

    python3 path/to/decomp-permuter/permuter.py build/permute/func_80012345 --stop-on-zero
"""
import argparse
import glob
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import autodecomp  # noqa: E402

COMPILE_SH = """#!/bin/sh
# Usage: compile.sh input.c -o output.o
set -e
ROOT={root}
KMC=$ROOT/tools/kmc
COMPILER_PATH=$KMC $KMC/gcc -c -G0 -mips3 -mgp32 -mfp32 -O2 -B $KMC/ -I $ROOT/include -I $ROOT -o "$3" "$1"
python3 $ROOT/tools/elf_trim.py "$3"
"""


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--max-diffs", type=int, default=3)
    ap.add_argument("--near", default=os.path.join(ROOT, "build", "auto", "near"))
    ap.add_argument("--out", default=os.path.join(ROOT, "build", "permute"))
    args = ap.parse_args()

    blocks = {}
    for path in glob.glob(os.path.join(ROOT, "asm", "*.s")):
        for name, vram, size, text in autodecomp.functions(path):
            blocks[name] = text

    made = 0
    for near in sorted(glob.glob(os.path.join(args.near, "*.c"))):
        diffs, name = os.path.basename(near)[:-2].split("_", 1)
        if int(diffs) > args.max_diffs or name not in blocks:
            continue
        d = os.path.join(args.out, name)
        os.makedirs(d, exist_ok=True)

        src = '#include "common.h"\n#include "m2c.h"\n\n' + open(near).read()
        pre = subprocess.run(["cpp", "-P", "-undef", "-D__GNUC__=2", "-D_LANGUAGE_C", "-I", os.path.join(ROOT, "include"), "-I", ROOT],
                             input=src, capture_output=True, text=True)
        if pre.returncode:
            print(f"{name}: cpp failed")
            continue
        # drop the INCLUDE_ASM machinery and empty lines the headers leave behind
        body = "\n".join(l for l in pre.stdout.splitlines() if l.strip() and "__asm__" not in l)
        open(os.path.join(d, "base.c"), "w").write(body + "\n")

        asm = os.path.join(d, "target.s")
        open(asm, "w").write('.include "macro.inc"\n.set noat\n.set noreorder\n.set gp=64\n.section .text, "ax"\n\n' + blocks[name])
        r = subprocess.run(["mips-linux-gnu-as", "-EB", "-march=vr4300", "-mabi=32", "-G0", "--no-pad-sections",
                            "-I", os.path.join(ROOT, "include"), "-o", os.path.join(d, "target.o"), asm], capture_output=True, text=True)
        if r.returncode:
            print(f"{name}: target did not assemble: {r.stderr.strip()[:100]}")
            continue
        sh = os.path.join(d, "compile.sh")
        open(sh, "w").write(COMPILE_SH.format(root=ROOT))
        os.chmod(sh, 0o755)
        open(os.path.join(d, "settings.toml"), "w").write(f'func_name = "{name}"\ncompiler_type = "gcc"\n')
        made += 1
    print(f"{made} directories in {args.out}")


if __name__ == "__main__":
    main()
