#!/usr/bin/env python3
"""Fix up an object written by KMC's assembler so modern ld takes it as the original did.

Usage: python3 tools/elf_trim.py FILE.o

- binutils 2.6 rounds .text up to 16 bytes. The original link did not keep that
  padding between objects, so .text is cut to the end of its last function.
- It also sets the symbol table's sh_info (first global symbol) too low, which
  modern ld rejects. It is set to the real first global.
- The literal pool it makes for li.d (.rodata) is marked writable but not
  allocated, so ld would leave it out of the ROM. It is marked allocated.

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
    open(path, "wb").write(data)


if __name__ == "__main__":
    main()
