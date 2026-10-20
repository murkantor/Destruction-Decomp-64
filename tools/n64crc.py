#!/usr/bin/env python3
"""Recompute the boot checksum (CRC1/CRC2 at 0x10) of an N64 ROM, CIC-6102 variant.

Usage: python3 tools/n64crc.py ROM [--check]

The boot code checks these two words against a sum over 0x1000-0x101000,
so a ROM with changed code needs them updated to boot on hardware or on
emulators that verify them. --check only prints whether they are right.
"""
import struct
import sys

SEED = 0xF8CA4DDC  # CIC-6102
M = 0xFFFFFFFF


def rol(v, n):
    n &= 31
    return ((v << n) | (v >> (32 - n))) & M


def crc(rom):
    t1 = t2 = t3 = t4 = t5 = t6 = SEED
    for i in range(0x1000, 0x101000, 4):
        d, = struct.unpack(">I", rom[i:i + 4])
        if (t6 + d) & M < t6:
            t4 = (t4 + 1) & M
        t6 = (t6 + d) & M
        t3 ^= d
        r = rol(d, d & 0x1F)
        t5 = (t5 + r) & M
        t2 = t2 ^ r if t2 > d else t2 ^ (t6 ^ d)
        t1 = (t1 + (t5 ^ d)) & M
    return (t6 ^ t4 ^ t3) & M, (t5 ^ t2 ^ t1) & M


def main():
    path = sys.argv[1]
    rom = bytearray(open(path, "rb").read())
    c1, c2 = crc(rom)
    old = struct.unpack(">II", rom[0x10:0x18])
    if "--check" in sys.argv:
        print("OK" if old == (c1, c2) else "BAD (header %08X %08X, computed %08X %08X)" % (*old, c1, c2))
        return
    rom[0x10:0x18] = struct.pack(">II", c1, c2)
    open(path, "wb").write(rom)
    print("CRC %08X %08X" % (c1, c2))


if __name__ == "__main__":
    main()
