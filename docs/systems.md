# Systems

What each part of the game does, as far as it is known, and where to find it. Addresses are retail VRAM. For every function see [functions.md](functions.md); for files see [files.md](files.md).

## Boot

| Symbol | File | Notes |
|---|---|---|
| `entrypoint` | `asm/entry.s` | Clears bss, sets the stack, jumps to `boot` |
| `boot`, `idle`, `mainproc` | `src/main/boot.c` | Starts the idle and main threads, picks NTSC/PAL/MPAL, calls game init |
| `gInitConfig` | | Heap start (0x80137170, first free RAM after bss) and video mode |

## Memory

| Symbol | File | Notes |
|---|---|---|
| `gMainHeap` | | The heap everything uses (`Heap`, `include/heap.h`) |
| `Heap_Alloc`, `Heap_AllocAlt`, `Heap_Free`, `Heap_Shrink` | `src/heap.c` | First fit over a free list sorted by address. A block handle keeps a 24-bit size and 8-bit alignment padding |
| `Mem_Alloc`, `Mem_AllocAlt`, `Mem_AllocAligned`, `Mem_AllocAlignedAlt`, `Mem_Free` | `src/mem.c` | Wrappers that store the block handle in the word before the pointer |
| `Pool_Init`, `Pool_Alloc`, `Pool_Free`, `Pool_GetFree` | `src/pool.c` | Fixed-size blocks linked through their first word |

For a port, `Mem_Alloc`/`Mem_Free` map straight to `malloc`/`free`.

## Files and track data

| Symbol | File | Notes |
|---|---|---|
| `RomFile_Open`, `Read`, `Seek`, `Tell`, `Close` | `src/romfile.c` | One read-only "file": the `zippeddata.zip` area of ROM. DMA needs aligned buffers, so odd reads go through an 8 KB buffer |
| `Zip_Open`, `Zip_ReadEndOfCentralDir` | asm | Info-ZIP style reader; inflate uses `gInflateMaskBits` |
| `Elf_FindSymbol`, `Elf_ResolveSymbols`, `Elf_Relocate` | asm | Load the track `.o` files from the zip and link them at run time |

Each track is two MIPS ELF objects (`<track>.o`, `<track>_sdl.o`) with symbol tables, for example `pit_TrackInfo`, `pit_Physics_Polys`, `pit_Physics_Verts`, `pit_Textures`. A port can read the zip and the ELF objects directly on the host instead of emulating ROM DMA.

## Game state and cars

| Symbol | Header | Notes |
|---|---|---|
| `gGame` (0x8011EEE4) | `include/game.h` | Most used global. `gGame->mode` at 0xC, `gGame->race` at 0x1100 |
| `Race` | `include/game.h` | `cars[12]` at 0x74, `numCars` at 0xA4 |
| `Car` (0xEA0 bytes) | `include/car.h` | Health, model, AI pointer, `RigidBody` at 0x1EC, damage zones at 0xA6C, `CarStats` at 0xBEC |
| `Car_Load`, `Car_Create` | asm | Allocate a car and copy its `CarStats` record in |
| `RigidBody_Init` | asm | Mass, inverse mass, box inertia and radius from `CarStats` |

`CarStats` holds the handling numbers the prototype's debug menu edits (traction, torque, gear ratios, mass, steer angles, springs); see [prototype.md](prototype.md#car-fields).

## Time

These names are being added to `symbol_addrs.txt`; until then the functions are `func_80063738` and `func_80064B30`, and the clock is `D_800D31D0`.

| Symbol | Notes |
|---|---|
| `gGameTime` (0x800D31D0) | The game clock, a `u64` |
| `Time_Get` | Reads a `u64` time |
| `Timer_Start(obj, ticks)` | Sets `obj` + 0x18 to now plus `ticks`; callers pass 300,000,000 |

## 2D screens

| Symbol | Notes |
|---|---|
| `gUi` (0x8011EEF4) | Owns the sprites and text on screen |
| `Ui_AddElement(ui, kind, id)`, `Ui_FindElement`, `Ui_RemoveElement` | Kind 2 is a sprite or text string; `id` is a `SpriteId` from `include/ids.h` |
| `PakMenu_*` | Controller Pak prompts (insert, no note, full, overwrite) |
| `Debug_AssertScreen`, `Gfx_BusyScreen`, `Debug_DrawPerfStats` | Debug screens left in retail |
| `DebugMenu_*` | Helpers for the debug menu; retail only builds "Video Setup" |

## Graphics

Display lists are built with the standard F3DEX 2.07 and S2DEX 2.07 microcode (text at ROM 0x7B9D0). Many small functions write GBI commands directly, for example `gSPDisplayList` (0xDE), `gDPFullSync` (0xE9) and `gSPEndDisplayList` (0xDF). A port needs an F3DEX-compatible renderer.

## SDK

libultra 2.0I, libc and the audio library. Named functions are in `symbol_addrs.txt`; decompiled ones are in `src/libultra/`. Some, such as `osGetCount` and `__osDisableInt`, were hand-written assembly in the SDK and stay as assembly.
