# Porting notes

Notes for a native PC port, planned as a `port-pc` branch once the matching decompilation reaches 100%. The matching build on `main` stays as it is; the port branch changes the build and replaces the N64 layer.

## What stays in assembly, and what a port does with it

All game code is C. What is left in `asm/` is runtime and hardware code that a port leaves out of the build and replaces.

| Group | Functions | Port |
|---|---|---|
| libgcc 64-bit and float helpers | `__divdi3`, `__udivdi3`, `__umoddi3`, `__muldi3`, `__cmpdi2`, `__fixunsdfdi`, `__floatdisf`, `__floatdidf`, `func_8007A278`, `func_8007A374`, `func_8007A830` | Drop. The PC compiler does `u64` and float conversions itself |
| libc | `bzero`, `bcopy`, `bcmp`, `sqrtf` | `memset`, `memmove`, `memcmp`, `sqrtf` |
| CPU, cache and TLB | `osInvalDCache`, `osInvalICache`, `osWritebackDCache`, `osWritebackDCacheAll`, `osMapTLBRdb`, `__osProbeTLB`, `osSetIntMask`, `__osDisableInt`, `__osRestoreInt`, `__osGetSR`, `__osSetSR`, `__osSetFpcCsr`, `osGetCount`, `__osSetCompare` | No-ops, or a host timer for `osGetCount` |
| Threads and exceptions | `__osDispatchThread`, `__osEnqueueAndYield`, `__osEnqueueThread`, `__osPopThread`, `__osCleanupThread`, `func_80070420` (exception handler), `func_80070994` | Replaced by the host runtime's threads and message queues |
| Devices | `__osSpRawStartDma`, `__osMotorAccess`, `entrypoint` | Replaced by the renderer, audio, input and boot code of the host runtime |
| Padding | About 200 four-byte `nop` words between objects | Nothing |

The libultra functions that are already C (`src/libultra/`: message queues, controllers, video, PI, controller pak) are replaced the same way, as a whole layer, not one by one.

## Host runtime

Write the libultra replacement from scratch or reuse an existing one:

| Option | Notes |
|---|---|
| [libultraship](https://github.com/Kenix3/libultraship) | Used by the Ship of Harkinian and Starship ports. Includes the Fast3D renderer (OpenGL, DirectX 11 on Windows), audio output, controller input and a resource system |
| ultramodern, from [N64Recomp](https://github.com/N64Recomp/N64Recomp) | libultra threads, message queues, VI and timing for recompiled games. Pairs with the RT64 renderer |

Either one supplies the parts in the table above.

## Graphics

- The game builds RSP display lists as `Gfx { u32 w0; u32 w1; }` words. The opcodes are F3DEX2 style: `0x01` is G_VTX (for example in `src/auto/15BF4.c`), `0xDE` is G_DL and `0xDF` is G_ENDDL (see `Gfx_StripCommands`). A port hands these lists to a renderer that interprets them (Fast3D, RT64).
- The game also writes to the framebuffer with the CPU: `Font_BlitString` copies glyph bytes straight into it, then calls `osWritebackDCacheAll`. The framebuffer is 320x240 at 16 bits (`func_80056408` allocates 0x25800 bytes). The renderer must read those CPU writes back, or the text must be drawn another way.

## Audio

The libultra audio library is in C (`alAdpcmPull`, `alEnvmixerPull`, `alFxPull` and the rest). Its output is a list of RSP audio commands (`Acmd`), not samples. A port needs an audio ABI interpreter (both libultraship and N64Recomp have one) and then sends the mixed samples to the host audio device.

## Pointers and data layout

This is the largest source change for a 64-bit build.

- `Gfx.w1` is a `u32` that often holds an address. Many structs also store pointers in 4-byte fields and the code relies on their offsets.
- The track data are MIPS ELF objects (`<track>.o`, `<track>_sdl.o`) inside `zippeddata.zip`. `Elf_ResolveSymbols` and `Elf_Relocate` write 32-bit addresses into the loaded data.
- Some globals sit at fixed addresses (`linker_syms.txt`, and the `D_80xxxxxx` names placed by `dd64.yaml`).

Two ways to handle it:

| Approach | Cost |
|---|---|
| Build the port as 32-bit (x86 Windows) | Least source change. Pointer sizes match the N64. Still needs the byte order handled |
| 64-bit build | Widen pointer fields and display list words, or translate 32-bit offsets to host pointers when the ELF data is loaded and when lists are built |

A 32-bit build first, then 64-bit later, keeps the first working version small.

## Byte order

The N64 is big-endian and PC is little-endian.

- Data read from ROM or from the track ELF objects is big-endian: models, textures, sequences, sound banks and the ELF headers. Swap it on load, or read it through byte-order helpers.
- ZIP headers are little-endian by format. Check how `Zip_Open` and `Zip_ReadEndOfCentralDir` read them before changing them.
- Code that reads a word as bytes, or bytes as a word, through a pointer cast needs checking one case at a time.

The `port-pc` branch converts data as the game reads it and logs reads at the wrong width; `CLAUDE.md` lists the patterns that break and how each is fixed.

## Files and saves

- `RomFile_*` (`src/romfile.c`) reads the `zippeddata.zip` area of the ROM through PI DMA. A port can open the user's own ROM and read the same area, or read an extracted copy of the zip. Neither the ROM nor data from it goes in the repository.
- Saves use the controller pak (`Pak_*`, `__osPfs*`). Map that to a file on disk.
- Rumble (`osMotorInit`, `__osMotorAccess`) maps to host controller rumble or a no-op.

## Matching workarounds in the C

The C on `main` keeps some forms that are there only to match the original bytes: `volatile` accesses, `do { } while (0)` around single statements, asm-label aliases such as `gUiArr` in `src/auto/47F6C.c`, and odd temporaries. They all compile on a PC compiler. The port branch can clean them up whenever convenient, since it no longer has to match.

## Suggested order

1. Branch `port-pc` from `main` at 100%.
2. Pick the host runtime and add a CMake build that compiles `src/` without `asm/`.
3. Stub libultra until it links, as 32-bit Windows.
4. Boot to the first frame: video, threads and file reads.
5. Graphics through the renderer, then input, audio and saves.
6. Byte order fixes for each asset type as they come up.
7. 64-bit build and the clean-up of matching workarounds.
