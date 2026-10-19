#!/bin/sh
# Download the KMC GCC 2.7.2 compiler and assembler used by the game into tools/kmc.
set -e
cd "$(dirname "$0")"
mkdir -p kmc
curl -sSfL https://github.com/decompals/mips-gcc-2.7.2/releases/download/main/gcc-2.7.2-linux.tar.gz | tar xz -C kmc
# KMC's assembler (binutils 2.6), used for files that are all C
curl -sSfL https://github.com/decompals/mips-binutils-2.6/releases/download/main/binutils-2.6-linux.tar.gz | tar xz -C kmc as
echo "KMC GCC and assembler installed in tools/kmc"
