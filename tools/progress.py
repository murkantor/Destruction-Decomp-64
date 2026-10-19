#!/usr/bin/env python3
"""Print how much of the game's code is in C.

Usage: python3 tools/progress.py

Counts the functions and bytes still in asm/ (after make extract) against the
whole main code segment. A lone nop that pads the end of a file is not counted.
"""
import glob
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import autodecomp  # noqa: E402

TEXT_BYTES = 0x7B9D0 - 0x1050  # main segment code, without the entry stub and microcode
TOTAL_FUNCTIONS = 1548


def main():
    funcs, size = 0, 0
    for path in glob.glob(os.path.join(ROOT, "asm", "**", "*.s"), recursive=True):
        if "/data/" in path or path.endswith("entry.s"):
            continue
        for _, _, s, _ in autodecomp.functions(path):
            if s > 4:
                funcs += 1
                size += s
    done = TOTAL_FUNCTIONS - funcs
    print(f"functions in C: {done} of {TOTAL_FUNCTIONS} ({100 * done / TOTAL_FUNCTIONS:.1f}%)")
    print(f"code in C:      {TEXT_BYTES - size:#x} of {TEXT_BYTES:#x} bytes ({100 * (TEXT_BYTES - size) / TEXT_BYTES:.1f}%)")


if __name__ == "__main__":
    main()
