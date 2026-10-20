# Destruction Derby 64: decompilation and PC port

`main` is a matching decompilation (`make extract && make` must print `build/dd64.z64: OK`).
`port-pc` adds a native 32-bit PC port (SDL3, OpenGL, Windows and Linux) built from the same C;
see `docs/porting.md` for the plan and `port/` for the host runtime.

Rules:

- On `main`, the N64 build must keep matching. `main` is finished, and `port-pc` does not take
  merges from it any more.
- On `port-pc`, the C does not have to rebuild the matching ROM, but keep it as close to 1:1
  with the original code as possible. Change it only slightly, to make it run (or run better)
  on a PC, and never in a way that changes how the game plays or feels on hardware. The PC
  build still reads symbol addresses from `build/dd64.elf`, so keep that N64 build compiling
  (`make`); it need not match.
- No ROM data in the repository: the port reads the user's own ROM at run time.
- N64 bugs stay. Where one would crash on PC (a stack overflow the N64 layout absorbs, say),
  give it room on PC and keep the behaviour.

## Byte order: read this before writing or changing C

The N64 is big-endian; the PC is little-endian. Most decompiled C was written against N64
memory, and much of it is correct only there. The port does not swap data with type
knowledge up front; it tracks it:

- Data from the ROM (DMA, the static `.data`/`.rodata` image, inflated zip entries) is
  marked big-endian in a shadow map, a byte per byte of the RAM image (`port/src/shadow.c`).
- The game code is built with `-fsanitize=kernel-address`, so every load and store calls a
  hook. The first 2/4/8-byte load of a marked value swaps it in place and clears the mark;
  stores record the width each byte was written at. Single bytes are the same either way.
- Globals the C names are converted at boot by their declared type
  (`tools/port/gen_widths.py`, from the game objects' DWARF), because GCC does not
  instrument a function's direct accesses to its own locals.
- Host code reading game memory (the renderer, the pak) reads by those recorded widths
  (`Port_RdN64`, `Port_ReadN64Bytes`).

So data is right when it is read **at the width it was written** (or the width it has in
the ROM). What breaks is code that relies on N64 byte layout. Each pattern below has hit
the port; write new C so it does not depend on byte order, and fix old C like this:

| Pattern on the N64 | Why it breaks on PC | Fix (under `TARGET_PC`) |
|---|---|---|
| Reading one byte or halfword of a wider value: `((u8 *)o)[0x37]` for the low byte of the `s32` at `0x34`; `bg->alpha` (`u8` at `0x4F`) where the setters store an `s32` at `0x4C` | The low byte is at the lowest address on PC | Read the whole value and take the part: `(u8)*(s32 *)((u8 *)o + 0x34)`. Prefer this to `BE_OFF8`/`BE_OFF16`, which are only right once the word has been converted |
| Writing part of a word, reading it whole (or the reverse) | Same | Same: access at one width; bitfields sharing a word are declared in reverse order under `HOST_LE` (`include/heap.h`) |
| A double written as two `u32` words, `{ hi, lo }`, or read through `(u32 *)&d` | Word order of an `f64` is reversed | `DU_WORDS(hi, lo)` for initialisers (`include/ultra64.h`); `(u32 *)&u + 1` for the high word under `HOST_LE` |
| Copying ROM data into a local, then reading it: `Vec3f up = D_800B8C10;` | The copy into a local is not instrumented, so nothing converts it | Nothing to do if the global is declared in C with its real type (the boot conversion covers it); if it is only reached through a pointer, convert before copying or read it in place |
| DMA or `Res_Load` into a stack buffer | The shadow map does not cover stacks | `PORT_DMA_BUF(type, name, offset)`: a slice of RAM-image scratch (`include/ultra64.h`) |
| Game code unpacking data itself (inflate) | Written byte by byte, so it looks like host data | `PORT_MARK_BE(p, len)` after it, as in `Inflate_FlushWindow` |
| Saving game memory (Controller Pak notes) from a stack copy | Widths are unknown outside the RAM image | Write from the global itself (`Pak_SaveGame`) |

How to find them: the port logs every access that reads a value at another width than it
was written, once per code site, to `Data/Logs/dd64.log`:

    shadow: byte load of a wider value, 1 bytes at 801269A7, from code 0x10090158 [...]
    shadow: load at another width than stored, 4 bytes at 800D8E7C, from code 0x100a5ab5 [...]

`addr2line -f -i -e DestructionDerby64 <code address>` gives the source line. A report
whose bytes are all equal (zero) is not logged. Treat each remaining report as a bug until
shown otherwise; most come from m2c types that do not match how the data is written.

## Calling convention: MIPS-only habits

m2c output often only works because of MIPS registers. On x86 arguments go on the stack.

- A call passing fewer arguments than the definition takes (it relied on `a0`-`a3` still
  holding them): `tools/port/check_protos.py --calls`.
- A `void` function whose caller uses `v0`: `tools/port/check_void_returns.py`.
- A function returning a struct by value, declared by callers as `void f(T *out, ...)`
  (the hidden result pointer goes in `a0` on MIPS; on i386 Linux the callee pops it, and
  MinGW returns 8-byte structs in registers): `tools/port/check_protos.py` reports
  "struct return"; callers include `include/port_sret.h` on PC.
- Floats returned or passed where the declaration says `int` (or no prototype): reported
  by `check_protos.py`.

Run the checkers from the repo root after `make`; they must report nothing new.

## Building

- N64: `make extract && make` (splat in `.venv`).
- PC, Linux: `cmake -B build_linux -G Ninja && cmake --build build_linux` (needs the N64 build's `build/dd64.elf`).
- PC, Windows (cross from Linux): `cmake -B build_win -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-i686.cmake`.
- Put the ROM in `Data/` next to the executable. Testing aids (environment variables):
  `DD64_SHOT=n,m` screenshots, `DD64_EXIT_AFTER=n`, `DD64_TAP=a:60` taps A every 60 frames,
  `DD64_PRESS=frame:input ...`, `DD64_DLDUMP=n` logs a display list with its matrices and
  vertices, `DD64_WATCH=addr` logs stores to an address.
