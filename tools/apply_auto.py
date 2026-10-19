#!/usr/bin/env python3
"""Turn tools/autodecomp.py matches into C files and dd64.yaml subsegments.

Usage: python3 tools/apply_auto.py [build/auto]

Consecutive matched functions go into one file, src/auto/<ROM offset>.c. When
m2c's declarations in a group clash, the group is split into one file per
function. A function that an existing C file pulls in with INCLUDE_ASM is put
in place of that line instead. Existing c subsegments in dd64.yaml are kept, so
it can be run again after more functions match. A function with constants in
.rodata gets a file of its own and a .rodata subsegment at the place
check_c.py found. Run make extract and make afterwards; the build's SHA1 check
is the final proof.
"""
import glob
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ROM_BASE = 0x80000400 - 0x1000
TEXT_START, TEXT_END = 0x1050, 0x7B9D0
RODATA_START, RODATA_END = 0xB8960, 0xBD4A0
KMC = os.path.join(ROOT, "tools", "kmc")
HEADER = '#include "common.h"\n#include "m2c.h"\n\n'


def compiles(src):
    with tempfile.TemporaryDirectory() as work:
        c = os.path.join(work, "f.c")
        open(c, "w").write(src)
        r = subprocess.run([os.path.join(KMC, "gcc"), "-S", "-G0", "-mips3", "-mgp32", "-mfp32", "-O2", "-B", KMC + "/",
                            "-I", os.path.join(ROOT, "include"), "-I", ROOT, "-o", os.path.join(work, "f.s"), c],
                           capture_output=True, text=True, env=dict(os.environ, COMPILER_PATH=KMC))
        return r.returncode == 0


def merge(codes):
    """Join m2c outputs, dropping repeated declaration lines."""
    seen, out = set(), []
    for code in codes:
        for line in code.splitlines():
            if line.strip().endswith(";") and not line.startswith(" ") and line in seen:
                continue
            seen.add(line)
            out.append(line)
        out.append("")
    return "\n".join(out).rstrip() + "\n"


def main():
    auto = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "build", "auto")
    matches = []
    rodata = {}  # function name -> (rom start, rom end) of its .rodata
    for line in open(os.path.join(auto, "matches.txt")):
        p = line.split()
        matches.append((int(p[0], 16), int(p[1], 16), p[2]))
        if len(p) == 5:
            rodata[p[2]] = (int(p[3], 16) - ROM_BASE, int(p[3], 16) - ROM_BASE + int(p[4], 16))
    new_rodata = []  # (rom start, rom end, path)

    # functions an existing C file pulls in with INCLUDE_ASM: replace that line
    inline = {}
    for path in glob.glob(os.path.join(ROOT, "asm", "nonmatchings", "**", "*.s"), recursive=True):
        rel = os.path.relpath(os.path.dirname(path), os.path.join(ROOT, "asm", "nonmatchings"))
        inline[os.path.basename(path)[:-2]] = rel
    replaced = 0
    for vram, size, name in [m for m in matches if m[2] in inline]:
        cfile = os.path.join(ROOT, "src", inline[name] + ".c")
        text = open(cfile).read()
        line = f'INCLUDE_ASM("asm/nonmatchings/{inline[name]}", {name});'
        if line in text:
            code = open(os.path.join(auto, name + ".c")).read().strip()
            text = text.replace(line, code)
            if '#include "m2c.h"' not in text:
                text = text.replace('#include "common.h"\n', '#include "common.h"\n#include "m2c.h"\n', 1)
            open(cfile, "w").write(text)
            replaced += 1
            if name in rodata:
                new_rodata.append((*rodata[name], inline[name]))
    matches = [m for m in matches if m[2] not in inline]
    if replaced:
        print(f"{replaced} INCLUDE_ASM lines replaced")

    # skip anything that already lies inside a C subsegment (decompiled earlier)
    starts = []
    for line in open(os.path.join(ROOT, "dd64.yaml")):
        m = re.match(r"\s*- \[0x([0-9A-F]+), (\w+)", line)
        if m:
            starts.append((int(m.group(1), 16), m.group(2)))
    def in_c(rom):
        kind = None
        for start, k in starts:
            if start > rom:
                break
            kind = k
        return kind == "c"
    already = [m for m in matches if in_c(m[0] - ROM_BASE)]
    for vram, size, name in already:
        print(f"{name}: already in a C file, skipped")
    matches = [m for m in matches if m not in already]

    runs = []
    for vram, size, name in matches:
        alone = name in rodata or (runs and runs[-1][-1][2] in rodata)
        if runs and not alone and runs[-1][-1][0] + runs[-1][-1][1] == vram:
            runs[-1].append((vram, size, name))
        else:
            runs.append([(vram, size, name)])

    files = []  # (rom start, rom end, path)
    for run in runs:
        codes = [open(os.path.join(auto, n + ".c")).read() for _, _, n in run]
        groups = [run] if compiles(HEADER + merge(codes)) else [[f] for f in run]
        for g in groups:
            start, end = g[0][0] - ROM_BASE, g[-1][0] + g[-1][1] - ROM_BASE
            path = f"auto/{start:X}"
            src = HEADER + merge([open(os.path.join(auto, n + ".c")).read() for _, _, n in g])
            os.makedirs(os.path.join(ROOT, "src", "auto"), exist_ok=True)
            open(os.path.join(ROOT, "src", path + ".c"), "w").write(src)
            files.append((start, end, path))
            if g[0][2] in rodata:
                new_rodata.append((*rodata[g[0][2]], path))

    yaml_path = os.path.join(ROOT, "dd64.yaml")
    lines = open(yaml_path).read().splitlines()
    sub = []  # (index, start, type, name) of the text subsegments
    for i, line in enumerate(lines):
        m = re.match(r"\s*- \[0x([0-9A-F]+), (asm|c)(?:, ([\w/]+))?\]", line)
        if m and TEXT_START <= int(m.group(1), 16) < TEXT_END:
            sub.append((i, int(m.group(1), 16), m.group(2), m.group(3)))
    indent = re.match(r"(\s*)", lines[sub[0][0]]).group(1)

    # existing c files end where the next subsegment starts
    cfiles = list(files)
    for k, (_, start, kind, name) in enumerate(sub):
        if kind == "c":
            end = sub[k + 1][1] if k + 1 < len(sub) else TEXT_END
            cfiles.append((start, end, name))
    cfiles.sort()

    out, pos = [], TEXT_START
    for start, end, name in cfiles:
        if start > pos:
            out.append(f"{indent}- [0x{pos:X}, asm]")
        out.append(f"{indent}- [0x{start:X}, c, {name}]")
        pos = end
    if pos < TEXT_END:
        out.append(f"{indent}- [0x{pos:X}, asm]")
    lines[sub[0][0]:sub[-1][0] + 1] = out

    if new_rodata:
        sub = []  # (index, start, name) of the rodata subsegments
        for i, line in enumerate(lines):
            m = re.match(r"\s*- \[0x([0-9A-F]+), \.?rodata(?:, ([\w/]+))?\]", line)
            if m and RODATA_START <= int(m.group(1), 16) < RODATA_END:
                sub.append((i, int(m.group(1), 16), m.group(2)))
        parts = list(new_rodata)
        for k, (_, start, name) in enumerate(sub):
            if name:
                parts.append((start, sub[k + 1][1] if k + 1 < len(sub) else RODATA_END, name))
        parts.sort()
        for a, b in zip(parts, parts[1:]):
            if a[1] > b[0]:
                sys.exit(f"rodata of {a[2]} and {b[2]} overlap")
        out, pos = [], RODATA_START
        for start, end, name in parts:
            if start > pos:
                out.append(f"{indent}- [0x{pos:X}, rodata]")
            out.append(f"{indent}- [0x{start:X}, .rodata, {name}]")
            pos = end
        if pos < RODATA_END:
            out.append(f"{indent}- [0x{pos:X}, rodata]")
        lines[sub[0][0]:sub[-1][0] + 1] = out
    open(yaml_path, "w").write("\n".join(lines) + "\n")
    print(f"{len(matches)} functions in {len(files)} files")


if __name__ == "__main__":
    main()
