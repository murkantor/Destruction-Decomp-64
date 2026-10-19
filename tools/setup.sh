#!/bin/sh
# Download the KMC GCC 2.7.2 compiler used by the game into tools/kmc.
set -e
cd "$(dirname "$0")"
mkdir -p kmc
curl -sSfL https://github.com/decompals/mips-gcc-2.7.2/releases/download/main/gcc-2.7.2-linux.tar.gz | tar xz -C kmc
echo "KMC GCC installed in tools/kmc"
