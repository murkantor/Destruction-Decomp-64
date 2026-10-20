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
| `Zip_Open`, `Zip_ReadEndOfCentralDir` | `src/auto/53470.c`, `src/auto/53328.c` | Info-ZIP style reader; inflate uses `gInflateMaskBits` |
| `Elf_FindSymbol`, `Elf_ResolveSymbols`, `Elf_Relocate` | `src/auto/54170.c`, `src/auto/5438C.c`, `src/auto/54500.c` | Load the track `.o` files from the zip and link them at run time |

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

| Symbol | Notes |
|---|---|
| `gGameTime` (0x800D31D0) | The game clock, a `u64` |
| `Time_Get` | Reads a `u64` time |
| `Timer_Start(obj, ticks)` | Sets `obj` + 0x18 to now plus `ticks`; callers pass 300,000,000 |

## Controllers and rumble

| Symbol | Notes |
|---|---|
| `gControllers` (0x80125C80) | Four 0xB0-byte states: buttons, pressed, stick and stick delta, rumble timer, Pak handle at 0x48 |
| `gContPads` | The `OSContPad` array `osContGetReadData` fills |
| `Pad_Read`, `Pad_ReadAll` | Wait for the SI read, update each plugged-in controller, start the next read. `Pad_ReadAll` also turns the D-pad into stick values |
| `Pad_Update`, `Pad_Clear`, `Pad_Init` | Per-controller state |
| `Pad_Rumble(channel, frames, strength)` | Strength 0 to 1 sets the on/off duty cycle of the motor |
| `Rumble_Restart`, `Rumble_Stop` | `osMotorInit` and `__osMotorAccess`; re-initialised about once a second while rumbling |

## Controller Pak

| Symbol | Notes |
|---|---|
| `Pak_SetGameInfo`, `Pak_Init` | Company and game code, note names; `osPfsInitPak` with retries |
| `Pak_FindFile`, `Pak_AllocateFile`, `Pak_DeleteFile`, `Pak_ReadFile`, `Pak_WriteFile`, `Pak_FreeBlocks`, `Pak_Check` | One-line wrappers over `osPfs*` for a channel, -1 if that Pak is not ready |
| `SaveMenu_PrevSlot`, `SaveMenu_NextSlot` | Step through the 16 save slots in `gSaveSlots`, skipping empty ones |

## Sound

| Symbol | Notes |
|---|---|
| `gVoices`, `gNumVoices` | 0x120-byte voices; `id` at 0x40; `sfxId` at 0x9E (the sound effect id, 0 for a music voice) |
| `Sound_FadeOut(id, frames)`, `Sound_FadeOutAll(mask, frames)` | Mask bit 0 is sound effects, bit 1 is music (the same bits as `Sound_SetMasterVolume`) |
| `Sound_CountPlaying(mask)`, `Sound_CountVoices(id)` | |
| `Sound_SetVolume`, `Sound_SetPitch`, `Sound_SetPan` | Set a field on every voice with that id |

## Maths

| Symbol | Notes |
|---|---|
| `Vec3f_*` | Copy, set, add, subtract, scale, dot, cross, length, normalize, rotate about X, Y or Z |
| `Math_Atan2`, `Math_SolveQuadratic` | |
| `rand` | The usual LCG (1103515245, 12345) |


| Symbol | Notes |
|---|---|
| `gUi` (0x8011EEF4) | Owns the sprites and text on screen |
| `Ui_AddElement(ui, kind, id)`, `Ui_FindElement`, `Ui_RemoveElement` | Kind 2 is a sprite or text string; `id` is a `SpriteId` from `include/ids.h` |
| `PakMenu_*` | Controller Pak prompts (insert, no note, full, overwrite) |
| `Debug_AssertScreen`, `Gfx_WaitTask`, `Gfx_SubmitTask` | Debug screens left in retail |
| `DebugMenu_*` | Helpers for the debug menu; retail only builds "Video Setup" |

## Graphics

Display lists are built with the standard F3DEX 2.07 and S2DEX 2.07 microcode (text at ROM 0x7B9D0). Many small functions write GBI commands directly, for example `gSPDisplayList` (0xDE), `gDPFullSync` (0xE9) and `gSPEndDisplayList` (0xDF). A port needs an F3DEX-compatible renderer.

## SDK

libultra 2.0I, libc and the audio library. Named functions are in `symbol_addrs.txt`; decompiled ones are in `src/libultra/`, `src/libc/` and `src/auto/`. Some, such as `osGetCount` and `__osDisableInt`, were hand-written assembly in the SDK and stay as assembly. The game uses the SDK scheduler (`osCreateScheduler`, `__scMain`) and the Rumble Pak (`osMotorInit`).

## Build flags

| Code | Flags |
|---|---|
| Game and libultra | `-O2` |
| libc and libm (`src/libc/`) | `-O1` |
| Some game files (Makefile `FASTMATH`) | `-O2 -ffast-math`: `sqrtf` becomes a bare `sqrt.s` |

## Matching notes

GCC 2.7.2 habits that decide whether a function matches:

- A store through a pointer into a struct field is assumed not to alias a plain global scalar, so loads of the global move above the store. When the original reloads the global after the store, declare the global as an array or struct member (`extern s32 D_x[];` and `D_x[0]`).
- Writing each `switch` case as its own store (`o->x = 1; break;`) keeps cases with equal values apart. Assigning a local and storing once lets them merge.
- A rodata symbol used by another file can still be a literal in C when the literal lands at the original address. The other users then find it through an address in `linker_syms.txt`, which `tools/apply_auto.py` adds itself. Two files sharing one literal were usually one source file.
- A file's rodata must end on an 8-byte boundary. When the literals end 4 bytes short and no symbol starts there, define a zero word after the function (`const s32 D_x = 0;`); `tools/apply_auto.py` does this too.
- KMC's old assembler can shift a function by 4 bytes (a zero word before it) when its name is long, depending on where it falls in the file. Car_CollideTrack (func_800301E4) did this with a 19-character name; a shorter name fixed it. If check_c reports a size 4 bytes too big for code that matched under its func_ name, try a shorter name.
- A local bound to a hard register (`register s32 yy asm("$2");`) gives the allocator a hint that can settle a register tie the source order cannot. Several of the last functions needed it (Font_BlitString, Spline_ComputeCoeffs, Race_FreeItemsCheckQualify); each file notes why.
- Dead code with a branch (for example a debug print compiled out) still splits a basic block before flow deletes it, so combine cannot fold moves across it. Pak_Check keeps its `move v1,v0; move v0,v1` pair this way.
- A function that reads a global both as a scalar and as an array can use an alias: `extern void *gUiArr[] __asm__("gUi");`.
- Some constants load into registers only when they arrive through a local or an inline parameter, for example a `u64` timer value or a struct field copied to a local before a call.
