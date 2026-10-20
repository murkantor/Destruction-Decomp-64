#!/bin/sh
# Build a test ROM with the near-miss drafts in nonmatching/ linked in place of
# their asm, to check that they behave like the original in an emulator.
#
# Usage: tools/build_nonmatching.sh
#
# The drafts to use are listed in nonmatching/test_build.txt. The build runs in
# a temporary copy of the repo, so the matching build is not touched. The ROM
# is written to build/dd64_nonmatching.z64 with its boot checksum fixed. It
# differs from the original only inside the listed functions.
set -e
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
cd "$ROOT"
tar cf - --exclude=./build --exclude=./.git . | tar xf - -C "$TMP"
cd "$TMP"
mkdir -p build/nm
grep -v '^#' nonmatching/test_build.txt > build/nm/matches.txt
for name in $(awk '{print $3}' build/nm/matches.txt); do
    cp "nonmatching/$name.c" build/nm/
done
python3 tools/apply_auto.py build/nm
make extract >/dev/null
make build/dd64.z64 >/dev/null
python3 tools/n64crc.py build/dd64.z64
mkdir -p "$ROOT/build"
cp build/dd64.z64 "$ROOT/build/dd64_nonmatching.z64"
echo "Wrote build/dd64_nonmatching.z64"
