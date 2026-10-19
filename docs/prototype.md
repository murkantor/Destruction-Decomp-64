# Prototype

Notes on a prototype build of Destruction Derby 64 and what it gives the retail decomp. The prototype is not needed to build the ROM.

| ROM | Value |
|---|---|
| SHA1 (big-endian .z64) | `6c20de318bd6c96409858be3c09e14ce01b829a9` |
| Header name and game code | Blank |
| Track zip date | 27 Jun 1999 (retail: 25 Aug 1999) |
| libultra | Same build as retail (31 Jul 1998), but without the asserts |
| Compiler | Same as retail: KMC GCC 2.7.2, `-O2` |
| Code | ROM 0x1050 to 0x80790, about 1620 functions (retail: 1560) |
| Main entry | `boot` at 0x80065EB0, bss 0x800C5350, 0x826C0 bytes |

Put it at `baserom_proto.z64` in the repo root if you want to run the tools below. Like the retail ROM, it must never be committed.

## What it has that retail does not

- No symbols for the game code, and the code is optimised the same way as retail.
- A table of names for sound, sprite and text IDs.
- A full debug menu whose labels name car and AI fields.

## Function matching

80% of retail functions (1251 of 1560) have the same code as a prototype function once addresses are masked, and 1005 of those match exactly one function. By size that is 59% of retail code, since the large gameplay functions are the ones that changed. Matched pairs use the same struct offsets throughout, so names and struct layouts carry over.

```sh
python3 tools/proto/funcmatch.py baserom_proto.z64 baserom.z64 build/funcmap.txt
```

## ID names

The debug menu's list items take a pointer into a pool of strings at 0x800B4BC4 (ROM 0xB57C4). Each list starts at a fixed index in the pool. `tools/proto/names.py` turns them into `include/ids.h`.

| Enum | Pool index | Count | Menu item | Notes |
|---|---|---|---|---|
| `SfxId` | 15 | 39 | SFX Name | `FX_NULL` to `FX_3SELECT` |
| `VoiceId` | 53 | 145 | VOICE Name | `FX_VOICE_NULL` to `FX_TIME2` |
| `SpriteId` | 222 | 527 | Sprite ID | Sprites and text share one ID space, see below |
| `DamageSpriteId` | 749 | 16 | Damage Sprites > Sprite ID | |
| `Difficulty` | 198 | 4 | World Tour | |
| `AiTargetBias` | 214 | 8 | A Base Tenac | |

- `SpriteId` 0 is `EMPTY_LIST`. Sprites run from 1 to 376, then `START_STR_LIST` (377), the text IDs, and `END_STR_LIST` (526). The `UNUSEDnnn_SP` and `UNUSEDnnn_STR` placeholders always sit at index 222 + nnn, which confirms the numbering.
- The pool has `"FX_TRAV_DIRT_SNOW1FX_HIT_SIDE_SIDE1"` as one string. A missing comma in the original source joined two literals, so the menu shows the wrong name for every sound after it. `names.py` splits it, which makes 39 sound IDs. The split is an inference.
- The car name list (pool index 202) has 12 placeholder names, `Coupe A` to `Coupe L`, so there are 12 car models.
- The menu's maximum values for SFX (97) and voice (244) are larger than their name lists, so only the named entries are in the enums.
- These values are the prototype's. Retail may have added or removed IDs, so check against retail code before relying on them.

```sh
python3 tools/proto/names.py baserom_proto.z64 > include/ids.h
```

## Debug menu

| Function | Address (proto) | Address (retail) | What it does |
|---|---|---|---|
| Menu builder | 0x8004BD50 | Removed | Builds every submenu |
| `DebugMenu_Init` | 0x800554A0 | 0x80053F00 | Starts a submenu |
| `DebugMenu_AddToggle` | 0x800554AC | 0x80053F0C | On/off item |
| `DebugMenu_AddInt` | 0x800554FC | 0x80053F5C | `(menu, label, int *value, min, max, char **names)` |
| `DebugMenu_AddFloat` | 0x80055548 | 0x80053FA8 | `(menu, label, f32 *value, f32 step)` |
| `DebugMenu_AddSubmenu` | 0x8005558C | 0x80053FEC | `(menu, label, submenu)` |
| Copy car to menu | 0x8004FD00 | Removed | Copies the selected car's fields into a tuning block at 0x800B58F0 |
| Copy menu to car | 0x8004FF7C | Removed | Writes changed values back |

Retail keeps only "Video Setup" > "Frame Setup" (Interlace, Dither, FIFO Shift), built by `DebugMenu_BuildVideo` and `DebugMenu_BuildFrameSetup`.

The prototype's submenus are Audio, Sfx, Voice and Music options, World Tour settings, Car Options (with AI Driver, physics and Car Damage), Camera Options (with a camera Placer Tool), Graphic Options, Sprite Options and Object Options.

To list every item with its target address and list arguments:

```sh
python3 -m spimdisasm.singleFileDisasm baserom_proto.z64 build/proto --start 0x1050 --end 0x80790 --vram 0x80000450 --data-start 0x80790 --data-end 0xC6000
python3 tools/proto/menuparse.py build/proto/baserom_proto_80000450.text.s baserom_proto.z64
```

## Car fields

The selected car is a pointer at 0x800E3408. The copy functions read these offsets from it. They are in `include/car.h`.

| Offset | Field | Menu label |
|---|---|---|
| 0x010 | `health` | Health (0 to 200) |
| 0x034 | `model` | Name (0 to 11) |
| 0x038 | `ai` (pointer) | AI Driver submenu |
| 0x308 | `viewingScale` | Viewing Scale |
| 0xA6C | `damage[8]`, 0x14 each | Car Damage: `defence` at +0, `offence` at +4 |
| 0xBF8 | `traction` | traction |
| 0xBFC | `braking` | braking |
| 0xC00 | `engineBraking` | engine braking |
| 0xC04 | `offroadHandling` | offroad handle |
| 0xC08 | `steerResponse` | steer response |
| 0xC18 | `torqueAt0Rpm` | torque 0 rpm |
| 0xC1C | `maxTorque` | max torque |
| 0xC2C | `gearRatios[6]` | gear ratio R, 1 to 5 |
| 0xC44 | `finalRatio` | final ratio |
| 0xC64 | `mass` | mass (clamped to 500 to 2000) |
| 0xC7C | `slowSteerAngle` | slow steer angle |
| 0xC80 | `fastSteerAngle` | fast steer angle |
| 0xC9C | `springK` | spring K |
| 0xCA0 | `springB` | spring B |
| 0xCA4 | `equilibrium` | equilibrium |
| 0xCA8 | `minSpringLength` | min spring len |
| 0xCAC | `maxSpringLength` | max spring len |
| 0xCB0 | `springYPos` | spring ypos |

Damage zones, in array order: front, right front, right side, right back, left front, left side, left back, back.

The AI fields live in the struct at `car->ai`: `tendency` +0x20, `newTarget` +0x34, `hitTarget` +0x38, `defence` +0x3C, `boolClose` +0x40, `boolDeath` +0x44, `boolLead` +0x48, `targetBias` +0x4C, `baseTenacity` +0x50, `baseSearch` +0x58.

How far these hold in retail:

- Retail code uses the damage zones at the same offsets.
- Matched functions use identical offsets from 0xA54 to 0xC50, so the layout did not move.
- No game code in either build reads the physics fields (0xBF8 to 0xCB0) by these offsets. Only the debug copy functions do, so the physics code must reach them another way. That is still to be found.
- Both copy functions use 0xABC for the left side defence, which is the left front zone. Going by the 0x14 stride the real field is at 0xAD0. This is a bug in the original code.

## Track objects

This applies to retail too. Every `.o` in `zippeddata.zip` keeps its symbol table and source path, for example `Assets/Models/Arenas/01_ThePit/pit.c` with `pit_TrackInfo`, `pit_Physics_Polys`, `pit_Physics_Verts`, `pit_Physics_SectorTable`, `pit_Physics_SectorArray`, `pit_Physics_PolyIndexArray` and `pit_Textures`. The prototype and retail have the same 24 files.
