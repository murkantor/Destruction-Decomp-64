#!/usr/bin/env python3
"""Try m2c on every function left in asm/ and keep the ones that compile to matching code.

Usage: python3 tools/autodecomp.py M2C_DIR [--max-size SIZE] [--out build/auto]

For each function in the splat output under asm/ (including the ones a C file
pulls in with INCLUDE_ASM from asm/nonmatchings/), this
runs m2c, compiles the result with the same compiler and flags as the Makefile,
and compares it with the ROM. Address fields are compared through the
relocations, so a wrong symbol or offset counts as a mismatch. Matches are
written to OUT/matches.txt and their C to OUT/<name>.c. The closest miss for
each function that compiles to the right size goes to OUT/near/<diffs>_<name>.c.
"""
import argparse
import glob
import os
import re
import struct
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ROM_BASE = 0x80000400 - 0x1000  # main segment: vram - rom
KMC = os.path.join(ROOT, "tools", "kmc")
CFLAGS = ["-G0", "-mips3", "-mgp32", "-mfp32", "-O2", "-B", KMC + "/", "-I", os.path.join(ROOT, "include"), "-I", ROOT]
ASFLAGS = ["-EB", "-march=vr4300", "-mabi=32", "-G0", "--no-pad-sections"]


def functions(path):
    """Yield (name, vram, size, asm text) for every function block in a splat asm file."""
    block, name = None, None
    for line in open(path):
        m = re.match(r"nonmatching (\w+), 0x([0-9A-F]+)", line)
        if m:
            name, size, vram, block = m.group(1), int(m.group(2), 16), None, [line]
            continue
        if block is None:
            continue
        block.append(line)
        if vram is None:
            m = re.search(r"/\* [0-9A-F]+ ([0-9A-F]{8}) [0-9A-F]{8} \*/", line)
            if m:
                vram = int(m.group(1), 16)
        if line.startswith("endlabel " + name):
            yield name, vram, size, "".join(block)
            block = None


def load_symbols():
    """Address of every symbol splat knows, from the asm labels and the auto files."""
    syms = {}
    for d, _, files in os.walk(os.path.join(ROOT, "asm")):
        if "nonmatchings" in d:
            continue
        for f in files:
            label = None
            for line in open(os.path.join(d, f)):
                m = re.match(r"\s*(?:glabel|dlabel|jlabel|alabel)\s+(\w+)", line)
                if m:
                    label = m.group(1)
                    continue
                if label:
                    m = re.search(r"/\* [0-9A-F]+ ([0-9A-F]{8})", line)
                    if m:
                        syms[label] = int(m.group(1), 16)
                        label = None
    for m in re.finditer(r"^(\w+)\s*=\s*(0x[0-9A-Fa-f]+);", open(os.path.join(ROOT, "symbol_addrs.txt")).read(), re.M):
        syms.setdefault(m.group(1), int(m.group(2), 16))
    for f in ("build/undefined_syms_auto.txt", "build/undefined_funcs_auto.txt", "linker_syms.txt", "include/hardware_syms.ld"):
        p = os.path.join(ROOT, f)
        if os.path.exists(p):
            for m in re.finditer(r"(\w+)\s*=\s*(0x[0-9A-Fa-f]+)", open(p).read()):
                syms[m.group(1)] = int(m.group(2), 16)
    return syms


def compile_c(src, workdir):
    """Compile like the Makefile does for files without INCLUDE_ASM: KMC GCC and its assembler."""
    c = os.path.join(workdir, "f.c")
    o = os.path.join(workdir, "f.o")
    open(c, "w").write(src)
    env = dict(os.environ, COMPILER_PATH=KMC)
    r = subprocess.run([os.path.join(KMC, "gcc"), "-c", *CFLAGS, "-o", o, c], capture_output=True, text=True, env=env)
    if r.returncode:
        return None, r.stderr
    r = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "elf_trim.py"), o], capture_output=True, text=True)
    if r.returncode:
        return None, r.stderr
    return o, ""


def section_sizes(o):
    out = subprocess.run(["mips-linux-gnu-objdump", "-h", o], capture_output=True, text=True).stdout
    sizes = {}
    for m in re.finditer(r"^\s*\d+\s+(\.\S+)\s+([0-9a-f]+)", out, re.M):
        sizes[m.group(1)] = int(m.group(2), 16)
    return sizes


def relocs(o):
    out = subprocess.run(["mips-linux-gnu-readelf", "-rW", o], capture_output=True, text=True).stdout
    rel = {}
    in_text = False
    for line in out.splitlines():
        if line.startswith("Relocation section"):
            in_text = "'.rel.text'" in line
            continue
        if not in_text:
            continue
        p = line.split()
        if len(p) >= 5 and re.fullmatch(r"[0-9a-f]{8}", p[0]) and p[2].startswith("R_MIPS"):
            rel[int(p[0], 16)] = (p[2], p[4])
    return rel


# (vram, size) of the .rodata of the last function matches() accepted, or None
last_rodata = None
RODATA_SECTIONS = (".rodata", ".rdata")


def matches(o, name, vram, size, rom, syms):
    """Compare the compiled function with the ROM, resolving relocations.

    Constants in .rodata are allowed when every reference points at the same
    place in the ROM's rodata and the bytes there are the same. That place is
    left in last_rodata so the caller can give the file its own .rodata
    subsegment.
    """
    global last_rodata
    last_rodata = None
    sizes = section_sizes(o)
    if any(sizes.get(s, 0) for s in (".data", ".bss", ".sdata", ".sbss")):
        return False, "uses data sections"
    rodata = [s for s in RODATA_SECTIONS if sizes.get(s, 0)]
    if len(rodata) > 1:
        return False, "two rodata sections"
    rodata_base = None
    nm = subprocess.run(["mips-linux-gnu-nm", "-S", o], capture_output=True, text=True).stdout
    m = re.search(r"^([0-9a-f]+) ([0-9a-f]+) T " + re.escape(name) + "$", nm, re.M)
    if not m or int(m.group(1), 16) != 0 or sizes.get(".text") != size:
        return False, f"size {sizes.get('.text', 0):#x} != {size:#x}"
    text = subprocess.run(["mips-linux-gnu-objcopy", "-O", "binary", "-j", ".text", o, "/dev/stdout"], capture_output=True).stdout
    rel = relocs(o)
    hi = {}
    diffs = 0
    for k in range(0, size, 4):
        a = struct.unpack(">I", text[k:k + 4])[0]
        b = struct.unpack(">I", rom[vram - ROM_BASE + k:vram - ROM_BASE + k + 4])[0]
        r = rel.get(k)
        if r is None:
            if a != b:
                diffs += 1
            continue
        kind, sym = r
        if kind == "R_MIPS_26" and sym == ".text":
            # jump inside the function: the instruction holds the offset
            if (a & 0xFC000000) != (b & 0xFC000000) or ((b & 0x3FFFFFF) << 2) != ((vram + ((a & 0x3FFFFFF) << 2)) & 0x0FFFFFFC):
                return False, f"+{k:#x} jump"
            continue
        if sym in RODATA_SECTIONS and kind in ("R_MIPS_HI16", "R_MIPS_LO16"):
            if (a & 0xFFFF0000) != (b & 0xFFFF0000):
                return False, f"+{k:#x}"
            if kind == "R_MIPS_HI16":
                hi[sym] = (b & 0xFFFF, a & 0xFFFF)
                continue
            if sym not in hi:
                return False, f"+{k:#x} lo without hi"
            lo = b & 0xFFFF
            rom_addr = ((hi[sym][0] << 16) + (lo - 0x10000 if lo & 0x8000 else lo)) & 0xFFFFFFFF
            add = a & 0xFFFF
            base = rom_addr - (add - 0x10000 if add & 0x8000 else add)
            if rodata_base is not None and base != rodata_base:
                return False, f"+{k:#x} rodata address"
            rodata_base = base
            continue
        if sym not in syms:
            m = re.fullmatch(r"(?:D|func|jtbl)_(?:[A-Z]+_)?([0-9A-F]{8})", sym)
            if not m:
                return False, f"unknown symbol {sym}"
            syms[sym] = int(m.group(1), 16)
        if kind == "R_MIPS_26":
            # the instruction may hold an addend (KMC's assembler does this for local jumps)
            target = (syms[sym] + ((a & 0x3FFFFFF) << 2)) & 0x0FFFFFFC
            if (a & 0xFC000000) != (b & 0xFC000000) or (b & 0x3FFFFFF) << 2 != target:
                return False, f"+{k:#x} call"
        elif kind in ("R_MIPS_HI16", "R_MIPS_LO16"):
            if (a & 0xFFFF0000) != (b & 0xFFFF0000):
                return False, f"+{k:#x}"
            if kind == "R_MIPS_HI16":
                hi[sym] = (b & 0xFFFF, a & 0xFFFF)
            else:
                # lo16 holds the addend; check the full address against the ROM's hi/lo pair
                if sym not in hi:
                    return False, f"+{k:#x} lo without hi"
                rom_hi, _ = hi[sym]
                lo = b & 0xFFFF
                rom_addr = ((rom_hi << 16) + (lo - 0x10000 if lo & 0x8000 else lo)) & 0xFFFFFFFF
                add = a & 0xFFFF
                add = add - 0x10000 if add & 0x8000 else add
                if rom_addr != (syms[sym] + add) & 0xFFFFFFFF:
                    return False, f"+{k:#x} address"
        else:
            return False, f"reloc {kind}"
    if diffs:
        return False, f"+{diffs} words differ"
    if rodata:
        if rodata_base is None:
            return False, "rodata not referenced"
        data = subprocess.run(["mips-linux-gnu-objcopy", "-O", "binary", "-j", rodata[0], o, "/dev/stdout"], capture_output=True).stdout
        off = rodata_base - ROM_BASE
        if data != rom[off:off + len(data)]:
            return False, "rodata differs"
        last_rodata = (rodata_base, len(data))
    return True, size


HEADER_LINES = 3  # the two #include lines and a blank line put in front of m2c's output


def repair(code, err):
    """Fix common m2c slips that stop the output compiling."""
    # a dereference of an integer sum: m2c means a byte address, so cast it to a pointer
    bad = {int(n) - HEADER_LINES - 1 for n in re.findall(r"f\.c:(\d+): invalid type argument of `unary \*'", err)}
    if bad:
        lines = code.splitlines()
        for i in bad:
            if 0 <= i < len(lines):
                lines[i] = re.sub(r"(^|[^\w)\]])\*\(", r"\1*(M2C_UNK *)(", lines[i])
        code = "\n".join(lines) + "\n"
    # a callee prototype guessed from one call site that another call site contradicts
    for callee in set(re.findall(r"too few arguments to function `(\w+)'", err)):
        code = re.sub(r"^(.*\b" + callee + r")\(.*\);(.*)$", r"\1();\2", code, count=1, flags=re.M)
    # a stack variable that m2c uses but never declares
    missing = sorted(set(re.findall(r"`(sp[0-9A-F]+)' undeclared", err)))
    if missing:
        decl = "".join(f"    M2C_UNK {v};\n" for v in missing)
        code = re.sub(r"^(\w[^\n]*\)\s*\{\n)", lambda m: m.group(1) + decl, code, count=1, flags=re.M)
    return code


def byte_offsets(code):
    """m2c writes byte offsets from a symbol as (&sym + n); make the addition bytewise."""
    out, i = [], 0
    pat = re.compile(r"\(&(\w+) \+ ")
    while True:
        m = pat.search(code, i)
        if not m:
            out.append(code[i:])
            # the same with the symbol second: n + &sym
            return re.sub(r"\+ &(\w+)\b", r"+ (s8 *)&\1", "".join(out))
        depth, j = 0, m.start()
        while j < len(code):
            if code[j] == "(":
                depth += 1
            elif code[j] == ")":
                depth -= 1
                if depth == 0:
                    break
            j += 1
        sym, rest = m.group(1), code[m.end():j]
        out.append(code[i:m.start()])
        out.append(f"((__typeof__({sym}) *)((s8 *)&{sym} + {byte_offsets(rest)}))")
        i = j + 1


def leading_args(code):
    """m2c drops parameters a function never uses; put back the ones before the first it does."""
    def fix(m):
        params = m.group(2)
        used = [int(n) for n in re.findall(r"\barg(\d+)\b", params)]
        if not used or min(used) == 0:
            return m.group(0)
        extra = ", ".join(f"M2C_UNK arg{i}" for i in range(min(used)))
        return f"{m.group(1)}({extra}, {params})" + m.group(3)
    return re.sub(r"^(\w[\w \*]*?\b\w+)\(([^()]*)\)(\s*\{)", fix, code, count=0, flags=re.M)


def call_sites(code, fname):
    """(start, open paren, close paren, callee) of each call in the body of fname."""
    body = re.search(r"^\w[^\n]*\b" + re.escape(fname) + r"\([^;{]*\)\s*\{", code, re.M)
    if not body:
        return []
    out = []
    for m in re.finditer(r"\b(\w+)\(", code[body.end():]):
        callee = m.group(1)
        if callee in ("if", "while", "for", "switch", "return", "sizeof", "M2C_FIELD", "__typeof__") or callee.startswith("M2C_"):
            continue
        start = body.end() + m.start()
        open_i = body.end() + m.end() - 1
        depth, j = 0, open_i
        while j < len(code):
            depth += code[j] == "("
            depth -= code[j] == ")"
            if depth == 0:
                break
            j += 1
        out.append((start, open_i, j, callee))
    return out


def shift_call(code, fname, site, k):
    """Pass fname's first k arguments through as the first k arguments of one call."""
    start, open_i, close_i, callee = site
    inner = code[open_i + 1:close_i].strip()
    lead = ", ".join(f"arg{i}" for i in range(k))
    code = code[:open_i + 1] + lead + (", " + inner if inner else "") + code[close_i:]
    # the callee's prototype no longer fits; make it unprototyped
    code = re.sub(r"^(\S.*\b" + re.escape(callee) + r")\(.*\);(.*)$", r"\1();\2", code, count=1, flags=re.M)

    # make sure fname has parameters arg0 .. arg(k-1)
    def fix(m):
        params = m.group(2).strip()
        have = {int(n) for n in re.findall(r"\barg(\d+)\b", params)}
        if params == "void":
            params, have = "", set()
        top = max(have | set(range(k))) if have or k else -1
        new = []
        for i in range(top + 1):
            pm = re.search(r"[^,]*\barg" + str(i) + r"\b", params)
            new.append(pm.group(0).strip() if pm else f"M2C_UNK arg{i}")
        return f"{m.group(1)}({', '.join(new)})" + m.group(3)
    return re.sub(r"^(\w[\w \*]*?\b" + re.escape(fname) + r")\(([^()]*)\)(\s*\{)", fix, code, count=1, flags=re.M)


def flip_tail_if(code):
    """Rewrite a function ending in `if (c) { body } return y;` as `if (!c) { return y; } body`.

    KMC GCC lays out an if that returns after the code that follows it, so the
    two forms give different code; try whichever m2c did not write.
    """
    m = re.search(r"\n    if \((.*)\) \{\n((?:        .*\n|\n)*?)    \}\n    return (.*);\n\}\n?$", code)
    if not m:
        return code
    cond, body, ret = m.group(1), m.group(2), m.group(3)
    body = "".join(l[4:] + "\n" if l.strip() else "\n" for l in body.splitlines())
    if not body.rstrip().endswith(";") or "return" not in body.splitlines()[-1]:
        body += f"    return {ret};\n"
    return code[:m.start()] + f"\n    if (!({cond})) {{\n        return {ret};\n    }}\n" + body + "}\n"


# m2c option sets to try in turn; different C shapes lead GCC to different code
VARIANTS = [
    [],
    ["--reg-vars", "saved"],
    ["--no-andor"],
    ["--reg-vars", "saved", "--no-andor"],
    ["--void"],
    ["--reg-vars", "saved", "--void"],
]


def attempt(code, work, name, vram, size):
    """Compile code (repairing common slips) and compare it. Returns (matched, reason, code, diffs)."""
    o, err = compile_c('#include "common.h"\n#include "m2c.h"\n\n' + code, work)
    for _ in range(3):
        if o:
            break
        fixed = repair(code, err)
        if fixed == code:
            break
        code = fixed
        o, err = compile_c('#include "common.h"\n#include "m2c.h"\n\n' + code, work)
    if not o:
        why = "compile error"
        if os.environ.get("AUTODECOMP_ERRORS"):
            line = next((l for l in err.splitlines() if ":" in l and "warning" not in l and "In function" not in l), err[:80])
            why += ": " + re.sub(r"^.*?:\d+: ", "", line)[:70]
        return False, why, code, None
    ok, why = matches(o, name, vram, size, ROM, SYMS)
    if ok:
        return True, "", code, 0
    if why.startswith("+") and why.endswith("words differ"):
        return False, "code differs", code, int(why[1:].split()[0])
    return False, why, code, None


def try_one(job):
    name, vram, size, text, m2c = job
    first_why, best = None, None
    with tempfile.TemporaryDirectory() as work:
        asm = os.path.join(work, name + ".s")
        open(asm, "w").write(text)
        seen = set()
        for opts in VARIANTS:
            r = subprocess.run([sys.executable, os.path.join(m2c, "m2c.py"), "--target", "mips-gcc-c", "--valid-syntax", *opts, asm],
                               capture_output=True, text=True)
            if r.returncode or "M2C_ERROR" in r.stdout or not r.stdout.strip():
                first_why = first_why or "m2c failed"
                continue
            base_codes = (r.stdout, byte_offsets(r.stdout), leading_args(r.stdout), leading_args(byte_offsets(r.stdout)))
            for code in dict.fromkeys(base_codes + tuple(flip_tail_if(c) for c in base_codes)):
                if code in seen:
                    continue
                seen.add(code)
                ok, why, code, diffs = attempt(code, work, name, vram, size)
                if ok:
                    return name, vram, size, code, last_rodata
                if diffs is not None and (best is None or diffs < best[0]):
                    best = (diffs, code)
                first_why = first_why or why
        # near misses: try passing the function's own arguments through to each call
        if best and best[0] <= 8:
            base = best[1]
            for site in call_sites(base, name)[:6]:
                for k in (1, 2):
                    code = shift_call(base, name, site, k)
                    if code in seen:
                        continue
                    seen.add(code)
                    ok, why, code, diffs = attempt(code, work, name, vram, size)
                    if ok:
                        return name, vram, size, code, last_rodata
                    if diffs is not None and diffs < best[0]:
                        best = (diffs, code)
    if best and NEAR_DIR:
        open(os.path.join(NEAR_DIR, f"{best[0]:03d}_{name}.c"), "w").write(best[1])
    return name, vram, size, None, first_why


NEAR_DIR = None


def init(rom, syms, near):
    global ROM, SYMS, NEAR_DIR
    ROM, SYMS, NEAR_DIR = rom, syms, near


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("m2c")
    ap.add_argument("--max-size", type=lambda x: int(x, 0), default=0x100000)
    ap.add_argument("--out", default=os.path.join(ROOT, "build", "auto"))
    ap.add_argument("--only", nargs="*")
    ap.add_argument("-j", type=int, default=os.cpu_count())
    args = ap.parse_args()

    rom = open(os.path.join(ROOT, "baserom.z64"), "rb").read()
    syms = load_symbols()
    os.makedirs(args.out, exist_ok=True)
    files = sorted(f for f in glob.glob(os.path.join(ROOT, "asm", "**", "*.s"), recursive=True)
                   if os.path.basename(f) != "entry.s" and "/data/" not in f)
    # a lone nop is padding after the last function of a file, not a function
    jobs = [(n, v, sz, t, args.m2c) for path in files for n, v, sz, t in functions(path)
            if sz <= args.max_size and (not args.only or n in args.only) and sz > 4]
    found, reasons = [], {}
    import multiprocessing
    near = os.path.join(args.out, "near")
    os.makedirs(near, exist_ok=True)
    with multiprocessing.Pool(args.j, initializer=init, initargs=(rom, syms, near)) as pool:
        for name, vram, size, c, why in pool.imap_unordered(try_one, jobs, chunksize=4):
            if c:
                found.append((vram, name, size, why))
                open(os.path.join(args.out, name + ".c"), "w").write(c)
            else:
                reasons[why] = reasons.get(why, 0) + 1
    found.sort()
    with open(os.path.join(args.out, "matches.txt"), "w") as f:
        for vram, name, size, rodata in found:
            extra = f" {rodata[0]:08X} {rodata[1]:#06x}" if rodata else ""
            f.write(f"{vram:08X} {size:#06x} {name}{extra}\n")
    print(f"tried {len(jobs)}, matched {len(found)}")
    for k, v in sorted(reasons.items(), key=lambda x: -x[1])[:int(os.environ.get("AUTODECOMP_TOP", "10"))]:
        print(f"  {v:4d} {k}")


if __name__ == "__main__":
    main()
