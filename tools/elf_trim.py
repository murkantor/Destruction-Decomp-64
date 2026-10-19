#!/usr/bin/env python3
"""Fix up an object written by KMC's assembler so modern ld takes it as the original did.

Usage: python3 tools/elf_trim.py FILE.o

- binutils 2.6 rounds .text up to 16 bytes. The original link did not keep that
  padding between objects, so .text is cut to the end of its last function.
- It also sets the symbol table's sh_info (first global symbol) too low, which
  modern ld rejects. It is set to the real first global.
- The literal pool it makes for li.d (.rodata) is marked writable but not
  allocated, so ld would leave it out of the ROM. It is marked allocated.
- .rodata is also rounded up to 16 bytes. It is cut to the end of the last
  thing the code loads from it (or the last jump table entry), rounded up to
  8 bytes.

The object is edited in place.
"""
import struct
import sys


def main():
    path = sys.argv[1]
    data = bytearray(open(path, "rb").read())
    if data[:4] != b"\x7fELF" or data[5] != 2:
        sys.exit(f"{path}: not a big-endian ELF")
    shoff, = struct.unpack(">I", data[0x20:0x24])
    shentsize, shnum, shstrndx = struct.unpack(">HHH", data[0x2E:0x34])

    def sh(i):
        return list(struct.unpack(">10I", data[shoff + i * shentsize:shoff + i * shentsize + 40]))

    sections = [sh(i) for i in range(shnum)]
    strtab = sections[shstrndx]
    name = lambda off: data[strtab[4] + off:data.index(b"\0", strtab[4] + off)].decode()
    for i, s in enumerate(sections):
        if name(s[0]) in (".rodata", ".rdata") and s[1] == 1 and not s[2] & 2:  # PROGBITS without SHF_ALLOC
            s[2] = 2
            o = shoff + i * shentsize
            data[o:o + 40] = struct.pack(">10I", *s)
    text = next((i for i, s in enumerate(sections) if name(s[0]) == ".text"), None)
    if text is None:
        return
    for i, s in enumerate(sections):
        if s[1] != 2:  # SHT_SYMTAB
            continue
        binds = [data[k + 12] >> 4 for k in range(s[4], s[4] + s[5], 16)]
        first_global = next((n for n, b in enumerate(binds) if b != 0), len(binds))
        if any(b == 0 for b in binds[first_global:]):
            sys.exit(f"{path}: local symbols after globals, cannot fix sh_info")
        s[7] = first_global
        o = shoff + i * shentsize
        data[o:o + 40] = struct.pack(">10I", *s)
    end = 0
    for s in sections:
        if s[1] != 2:  # SHT_SYMTAB
            continue
        for k in range(s[4], s[4] + s[5], 16):
            _, value, size, info, other, shndx = struct.unpack(">IIIBBH", data[k:k + 16])
            if shndx == text and (info & 0xF) == 2:  # STT_FUNC
                end = max(end, value + size)
    if 0 < end < sections[text][5]:
        tail = data[sections[text][4] + end:sections[text][4] + sections[text][5]]
        if tail.strip(b"\0"):
            sys.exit(f"{path}: data after the last function, not trimming")
        sections[text][5] = end
        o = shoff + text * shentsize
        data[o:o + 40] = struct.pack(">10I", *sections[text])
    trim_rodata(data, shoff, shentsize, sections, name)
    open(path, "wb").write(data)


WIDTH = {0x35: 8, 0x3D: 8, 0x31: 4, 0x39: 4, 0x23: 4, 0x2B: 4, 0x21: 2, 0x25: 2, 0x29: 2, 0x20: 1, 0x24: 1, 0x28: 1}


def trim_rodata(data, shoff, shentsize, sections, name):
    """Cut .rodata to the last byte .text or a jump table refers to."""
    ro = next((i for i, s in enumerate(sections) if name(s[0]) == ".rodata" and s[1] == 1), None)
    text = next((i for i, s in enumerate(sections) if name(s[0]) == ".text"), None)
    symtab = next((s for s in sections if s[1] == 2), None)
    if ro is None or text is None or symtab is None or sections[ro][5] == 0:
        return
    def sym_section(k):
        return struct.unpack(">H", data[symtab[4] + k * 16 + 14:symtab[4] + k * 16 + 16])[0]
    end = 0
    for s in sections:
        if s[1] != 9:  # SHT_REL
            continue
        target = s[7]
        for k in range(s[4], s[4] + s[5], 8):
            offset, info = struct.unpack(">II", data[k:k + 8])
            kind = info & 0xFF
            if target == ro and kind == 2:  # R_MIPS_32 in a jump table
                end = max(end, offset + 4)
            elif target == text and kind == 6 and sym_section(info >> 8) == ro:  # R_MIPS_LO16: the load holds the offset
                insn, = struct.unpack(">I", data[sections[text][4] + offset:sections[text][4] + offset + 4])
                imm = insn & 0xFFFF
                width = WIDTH.get(insn >> 26, 4)
                if insn >> 26 == 0x09:  # addiu: the address of a table or string; keep everything
                    return
                end = max(end, imm + width)
    end = (end + 7) & ~7  # the original objects kept .rodata 8-byte aligned
    if 0 < end < sections[ro][5]:
        if data[sections[ro][4] + end:sections[ro][4] + sections[ro][5]].strip(b"\0"):
            return
        sections[ro][5] = end
        o = shoff + ro * shentsize
        data[o:o + 40] = struct.pack(">10I", *sections[ro])


if __name__ == "__main__":
    main()
