# ROM notes

## Layout

| ROM start | ROM end | VRAM | Contents |
|---|---|---|---|
| 0x000000 | 0x000040 | | Header |
| 0x000040 | 0x001000 | | IPL3 (CIC 6102) |
| 0x001000 | 0x001050 | 0x80000400 | Entry: clears bss, sets the stack to 0x80123320, jumps to `boot` |
| 0x001050 | 0x07B9D0 | 0x80000450 | Game code, libultra, libc |
| 0x07B9D0 | 0x07F510 | 0x8007ADD0 | RSP microcode text: rspboot, F3DEX 2.07, S2DEX 2.07 |
| 0x07F510 | 0x0B8960 | 0x8007E910 | Data (includes microcode data) |
| 0x0B8960 | 0x0BD4A0 | 0x800B7D60 | Read-only data (start is splat's estimate) |
| | | 0x800BC8A0 | bss, 0x756F0 bytes |
| 0x0BD4A0 | 0xDCAF50 | | Assets, format not yet known |
| 0xDCAF50 | 0xF94834 | | `zippeddata.zip`, track data |
| 0xF94834 | 0x1000000 | | Padding |

The main segment is about 0.75 MB, so IPL3 loads all of it. The extra DMA in `boot` never runs.

## Boot

| Function | Address | What it does |
|---|---|---|
| `boot` | 0x80064B90 | `osInitialize`, starts the idle thread (id 1, priority 10) |
| `idle` | 0x80064C70 | Starts the main thread (id 4, priority 2), drops to idle priority |
| `mainproc` | 0x80064CDC | Picks the video mode from `osTvType`, fills `gInitConfig`, calls the game's init at 0x80058800, 0x8005B880 and 0x80000470 |

`gInitConfig` (0x801311C0) holds the heap start (0x80137170, the first free RAM after bss), a value of 1500 whose use is unknown, and the video mode (0 NTSC, 1 PAL, 2 MPAL).

## Track data

`zippeddata.zip` is a standard zip with 24 files, all deflate compressed. Each one is a big-endian MIPS ELF relocatable object (`ET_REL`). The game has its own zip reader (its error strings are in rodata), and the `.rel.data`, `*_TrackInfo` and `*_sdl_info` strings suggest it links these objects at load time.

| Track | Objects |
|---|---|
| airport, city, coastal, death, desert, glacier, ground_zero, industrial, jungle, pit, sewer, swamp | `<track>.o`, `<track>_sdl.o` |

The zip entries are dated 25 Aug 1999.

## Main structures

| Symbol | Type | Notes |
|---|---|---|
| `gGame` (0x8011EEE4) | `Game *` | The most used global. `gGame->race` (0x1100) is read over 300 times |
| `Race` | struct | `cars[12]` at 0x74, `numCars` at 0xA4 |
| `Car` | struct, 0xEA0 bytes | Allocated by `Car_Create`; see [prototype.md](prototype.md#car-fields) |

Headers: `include/game.h`, `include/car.h`.
