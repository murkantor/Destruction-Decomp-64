#!/usr/bin/env python3
"""Show a hand-written function's compiled code next to the original.

Usage: python3 tools/diff_func.py FILE.c [NAME]

FILE.c holds the function without the includes, as for check_c.py. NAME
defaults to the file name. Lines that differ are marked with *.
"""
import difflib
import glob
import os
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import autodecomp  # noqa: E402


def disasm(blob, vram):
    with tempfile.NamedTemporaryFile(suffix=".bin") as f:
        f.write(blob)
        f.flush()
        out = subprocess.run(["mips-linux-gnu-objdump", "-D", "-b", "binary", "-m", "mips:4300", "-EB",
                              f"--adjust-vma={vram:#x}", "-M", "gpr-names=32", f.name],
                             capture_output=True, text=True).stdout
    lines = []
    for line in out.splitlines():
        parts = line.split("\t")
        if len(parts) >= 3 and parts[0].strip().endswith(":"):
            lines.append(" ".join(parts[2:]).strip())
    return lines


def main():
    path = sys.argv[1]
    name = sys.argv[2] if len(sys.argv) > 2 else os.path.basename(path)[:-2]
    info = {}
    for p in glob.glob(os.path.join(ROOT, "asm", "*.s")):
        for n, vram, size, _ in autodecomp.functions(p):
            info[n] = (vram, size)
    vram, size = info[name]
    rom = open(os.path.join(ROOT, "baserom.z64"), "rb").read()
    target = disasm(rom[vram - autodecomp.ROM_BASE:vram - autodecomp.ROM_BASE + size], vram)
    with tempfile.TemporaryDirectory() as work:
        o, err = autodecomp.compile_c('#include "common.h"\n#include "m2c.h"\n\n' + open(path).read(), work)
        if not o:
            sys.exit(err)
        text = subprocess.run(["mips-linux-gnu-objcopy", "-O", "binary", "-j", ".text", o, "/dev/stdout"], capture_output=True).stdout
    ours = disasm(text, vram)
    # relocated fields are zero in ours; compare mnemonics and registers only for those
    def norm(l):
        return l.split("0x")[0] if any(op in l for op in ("lui", "jal", "%")) else l
    sm = difflib.SequenceMatcher(a=[norm(l) for l in ours], b=[norm(l) for l in target], autojunk=False)
    print(f"{'ours':44} original")
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        for k in range(max(i2 - i1, j2 - j1)):
            a = ours[i1 + k] if i1 + k < i2 else ""
            b = target[j1 + k] if j1 + k < j2 else ""
            mark = " " if tag == "equal" else "*"
            print(f"{mark} {a:42} {b}")
    print(f"ours {len(ours) * 4:#x} bytes, original {size:#x}")


if __name__ == "__main__":
    main()
