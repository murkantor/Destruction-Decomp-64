# Bugs in the original game

Defects found by reading the decompiled C. The C compiles to the same bytes as the retail ROM, so each of these is in the shipped game and happens on real hardware.

Status:
- **Checked**: the faulty expression was confirmed in the source.
- **Traced**: found and traced by the code review, not yet re-checked by hand.

Likelihood is for normal play. "Data" means it depends on values in the ROM data (sequences, track meshes) that have not been checked yet.

## Crashes and memory corruption

| # | Function | File | What goes wrong | Trigger | Status |
|---|---|---|---|---|---|
| 1 | `Obj_CreateFromSpawn`, `Obj_Free` | `src/auto/13E5C.c`, `src/auto/141AC.c` | Spawn types 23 and 32 count their shared model on different counters (create: 23 to 0, 32 to 13; free: 32 to 0, 23 to 13). Freeing a type 23 object releases the shared model while other objects still use it; type 32 models leak | Any track with type 23 or 32 objects that get freed. Likely | Checked |
| 2 | `Racer_UpdateLapTiming` | `src/auto/202F8.c:76` | Lap time written to `x60[x88]` with no bound. The 5th lap overwrites the lap counter itself, so the car stops counting laps and jumps to the top of the standings | A car completing 5 laps while the race runs (an AI lapping a slow player, or a finished player's car driving on in 2 player). Uncommon | Checked (write); trigger traced |
| 3 | `Light_UpdateVertexColors` | `src/auto/1369C.c:74` | Loops run to `x <= w`, `z <= h`, and `Grid_PosToCell` can return `w` at the grid's max edge. Reads past the cell array and writes vertex colours through garbage indices | A headlight circle reaching the +X or +Z grid edge | Traced |
| 4 | `Shadow_ClipAndEmitFaces` | `src/auto/15BF4.c` | Shadow buffers are sized for 84 Gfx commands and 120 vertices (`Obj_AllocSubBuffers`), but up to 200 track faces are clipped into them with no check. Overruns into the next heap block | A player car over dense track mesh. Data | Traced |
| 5 | `Shadow_ClipAndEmitFaces`, `Skid_ClipPolys` | `src/auto/15BF4.c` | Clip buffers of 10 vertices after 5 clip planes; a face of 6 or more vertices can grow past 10 on the stack | Track faces with 6+ vertices under the shadow or a skid. Data, probably absent | Traced |
| 6 | `Podium_Init` | `src/auto/4C90C.c:57` | The player's circuit table row indexes 3-entry arrays. A points tie at the qualifying place gives row 3: reads uninitialised stack and spawns an object from a garbage resource id | Tie on points at the cut-off place with a lower-numbered AI car. Specific situation | Traced |
| 7 | `Pause_Init` with `Ui_SaveElements` | `src/auto/476B8.c:123`, `src/auto/440B8.c` | Save blocks are allocated only under some conditions, but the restore path frees all three every time. `Heap_Free` does not clear the handle, so a stale block can go back on the free list | Pause, then Options or Records, then back, with the overlay layer set on one trip and clear on a later one. Frequency not known | Traced |
| 8 | `Res_ReleaseInstance` | `src/auto/646AC.c:40` | Destroys `list->next->value` but unlinks the caller's node. With 2+ instances one is destroyed twice and another leaks; a NULL destroy pointer jumps to address 0 | Only with force-loaded duplicates (`Res_SetForceLoad`, one game mode) | Traced |
| 9 | `Mem_Alloc`, `Mem_AllocAlt` | `src/mem.c:10`, `:20` | No check on the heap result: `*p = blk.id` writes through NULL when the heap is full. Every caller's NULL check is dead code | Heap exhausted. Unlikely | Checked |
| 10 | `List_RemoveById` | `src/auto/5D6CC.c:17` | No not-found case: removes an unrelated entry, and on an empty list sets the count to -1 and copies from `items[-1]` | A stale cached sound id. Specific situation | Traced |
| 11 | `Ui_RemoveElement`, `Ui_InsertElementAfter`, `List_Remove`, `Ui_FindElement` users | `src/auto/5E7E4.c` and others | List walks have no NULL check; a missing key reads address 0 | Removing an element twice. Specific situation | Traced |
| 12 | `Sound_PlayContact` | `src/auto/7A8C.c:141` | The voice lookup can end with `i == count` and still writes `pool->xC[i]->x18` | The 60-entry sound pool full. Rare | Traced |
| 13 | `Car_OnTriggerCrossed` | `src/auto/2C3A0.c:95` | A kind 4 trigger on track 8 reads `car->x3C->x10` with no check that the car is human; AI cars have `x3C == NULL` | Depends on trigger flags in track data. Data | Traced |
| 14 | `Hud_UpdateCountdown` | `src/auto/DA08.c:47` | After the deadline the u64 remaining time wraps; the clamp never fires and `trunc.w.d` runs on about 1.8e10, which may raise an FPU exception | Frames after the deadline while the element still updates | Traced |
| 15 | `Seq_ReadEnvelope` | `src/auto/65A08.c:45` | `1.0/n` with no zero check on three of four times; FPU invalid operation with exceptions enabled | Zero times in sound data. Data | Traced |
| 16 | `Fb_CreateContext` | `src/auto/57008.c:63` | Stores the address of a local struct in a global and keeps using a framebuffer it has freed | The assert screen, or an RCP stall of more than 100 retraces. Rare | Traced |
| 17 | `Pool_Alloc(gParticlePool)` users | `Camera_BuildProjection`, `ModelInst_BuildMatrix` and others | Results never checked; NULL goes into `guMtxF2L` | More than about 500 matrices in flight. Unlikely | Traced |
| 18 | `Save_CopyToGlobal` | `src/auto/50CCC.c:31` | Cup progress from the Controller Pak is not range-checked before use as an index | Corrupt save data only | Traced |

## Gameplay logic

| # | Function | File | What goes wrong | Trigger | Status |
|---|---|---|---|---|---|
| 19 | `Ai_PickTarget` | `src/auto/1B9C.c:109` | `d.y = tp.x - cp.y; d.z = tp.x - cp.z;` (should be `tp.y`, `tp.z`), so the "target is behind" flag is wrong and AIs brake or reverse at the wrong times | Every target pick | Checked |
| 20 | `Ai_ComputeIntercept` | `src/auto/16C8.c:63` | `cosf()` is given an angle in degrees, so closing speed and time to contact are garbage and the AI lead aim is effectively random | Always | Checked |
| 21 | `Track_LoadBranches` | `src/auto/2CB4.c:37` | `r->x38 -= e->xC` subtracts branch 0's length on every pass instead of branch i's, so the node count is wrong | A track with 2+ branches of different length | Checked |
| 22 | `Results_TallyStandings` | `src/auto/2BA70.c:69` | The tie-break adds car A's `x1C` to car B's score (`cars[i]` for `cars[j]`), so ties are judged wrongly and cars get spurious points | Every lap track (index 4 and up) | Checked |
| 23 | `Ai_SetEvadePoint` | `src/auto/3410.c:72` | A nearer car overwrites the pursuer as the target, but the distance used is still the pursuer's | Being chased with another car nearby. Common | Traced |
| 24 | `Ai_PickTarget2` | `src/auto/36B0.c:60` | Compares against an output that starts at -1, so target switching depends on the order of the car array | Always in derby state 0 | Traced |
| 25 | `Ai_UpdateState` | `src/auto/65A0.c:74` | State 2 exits on the same test that enters it, so the AI flips between states 0 and 2 while close to its target | Close to its target | Traced |
| 26 | `CarAI_OnCollision` | `src/auto/5394.c:60` | Anger is not reset in one of three target-switch paths | Low impact | Traced |
| 27 | `Car_PickRandomOther` | `src/auto/2080.c:21` | `rand() % 0` when no other car is active (a divide-by-zero break) | Mostly guarded. Unlikely | Traced |
| 28 | `Car_PickRandomActive` | `src/auto/2164.c:14` | The fallback search accepts health `>= 0`, so a dead player 0 stops it and it returns NULL while another human is alive | A dead player 0 | Traced |
| 29 | `Car_PhysicsStep`, `Physics_AddContact` | chunk 3 | One 50-entry contact buffer for all cars per step; in a big pile-up the last cars lose their impulses for that step | Large pile-ups. Uncommon | Traced |
| 30 | `Results_Init`, `Menu_ShowRecordRows` | `src/auto/4F218.c:96`, `src/auto/4ED44.c:44` | A car with 0 points leaves -1 in the bomb tag ranking, so `D_80131D88[-1]` is read and a garbage name is shown and copied | A car ending bomb tag with 0 points | Traced |
| 31 | `Results_ShowLapTimes` | `src/auto/4E784.c:108` | `(u64)(best - total) <= limit` is only reached when total is at or above best, so the subtraction wraps and the "close to the record" line plays only on an exact tie | Always | Checked |
| 32 | `Hud_UpdateRacerTimers` | `src/auto/E6E0.c:63` | The cap compares a remaining time with an absolute time; `"%2d"` of a 3-digit value goes into a 3-byte buffer | A deadline more than 99 s ahead | Traced |
| 33 | `func_8000D190` | `src/auto/DD90.c:30` | Team B's score padding tests team A's score | Scores either side of 1000 | Traced |
| 34 | `Hud_FlashDamageZone` | `src/auto/FDC4.c:44` | Exact `!=` on a computed double time, so a light hit can overwrite a heavy one within 1 s | Visual | Traced |
| 35 | `Timer_InitRate` | `src/auto/6399C.c:18` | MPAL (mode 2) is timed as 50 Hz, so game clocks run about 20% fast | MPAL consoles only | Traced |

## Audio

| # | Function | File | What goes wrong | Trigger | Status |
|---|---|---|---|---|---|
| 36 | `Voice_FindFree` | `src/auto/682C8.c:49` | The third search loop has no `v++`, so it only ever tests voice 0. Music channels can collide on one voice and drop out | All voices busy and none are sound effects | Checked |
| 37 | `Audio_UpdateVoices` | `src/auto/66ED0.c:82` | The volume track is gated on the pitch-track clock (`x14`) instead of its own (`x10`), a copy of the next line; volume curves run ahead of the notes | Volume track without pitch track at slow tempo. Data | Checked |
| 38 | `Seq_UpdateVolPan` | `src/auto/67550.c` | Pan is masked with `& 0x7F` instead of clamped, so a sound far right can jump to the left | A 3D sound right of the camera with right pan. Data | Traced |
| 39 | `Sound_SetTempoById` with `Seq_SetTempo` | `src/auto/66C64.c`, `src/auto/65B30.c` | Sound effect voices never store their base tempo, so per-frame tempo scaling resets an effect's own tempo to the 120 default | Positional effects with a tempo command. Data | Traced |
| 40 | `Seq_PushCall`, `Voice_PopEnvelope` | `src/auto/65E50.c:18`, `src/auto/65F04.c:43` | A signed envelope value (-64..63) is saved as an unsigned byte, so negative values come back as 192..255 | Returning from a sequence call during a negative curve. Data | Traced |
| 41 | `func_800656BC` | `src/auto/662BC.c:11` | Sets `x20 = x6C * x6C` instead of `x6C * x68` | Every use of that sequence command | Traced |
| 42 | `Sound_RemoveEmitter` | `src/auto/A634.c:24` | Removes the last emitter when the id is not found | An id removed twice in one pass. Rare | Traced |
| 43 | `Announcer_Update` | `src/auto/9418.c:58` | Writes index 15 of two 15-entry arrays when the queue is full | Full announcer queue. Low impact | Traced |
| 44 | `__amDMA` | `src/auto/68964.c` | When the DMA free list is empty it returns the buffer header address instead of sample data (same as the SDK demo) | Many sample DMAs in one frame. Brief noise | Traced |

## Graphics

| # | Function | File | What goes wrong | Trigger | Status |
|---|---|---|---|---|---|
| 45 | `Weather_Draw` | `src/auto/1A78C.c` | Weather state is global but the function runs once per viewport: rain falls N times faster, particles jump between cameras, and the double buffer stops alternating | 2+ players on track 10 (the weather track) | Traced |
| 46 | `Bg_Draw` | `src/auto/62248.c:218` | The last-column test uses the row index `i` instead of the column index `j`: a garbage column at the right edge and seams | Scaled UI backgrounds wider than 32 texels. Likely | Checked |
| 47 | `Car_EmitWheelDust` | `src/auto/B940.c:89` | Wheel 3 uses wheel 2's surface type | Always. Visual only | Traced |
| 48 | `Anim_UpdateTimers` | `src/auto/147B0.c:51` | The step count is an `s8`; 128+ periods in one frame wrap negative and freeze the animation | Very long frames. Rare | Traced |
| 49 | `Sprite_SetSize` | `src/auto/621AC.c:24` | Compares a stored `tw*4` with `tw`, always true, so `guS2DInitBg` reruns every frame | Always. CPU time only | Traced |
| 50 | `Credits_Update` | `src/auto/50294.c:79` | The 1 s end pause is overwritten by a 50 ms one | Always. Cosmetic | Traced |

## Minor

| # | Function | What goes wrong | Status |
|---|---|---|---|
| 51 | `Pak_LoadSave` | Leaks its 0x328-byte buffer when the Pak read fails | Traced |
| 52 | `Timer_Reset` | Discards the `osGetTime` result, so the first frame measures from power-on | Traced |
| 53 | `Vec3f_AngleBetween` | Returns 0 instead of 90 degrees for perpendicular vectors and does not clamp the `acos` argument | Traced |

## Unreachable in retail

These have no callers, or only run in debug modes, so they never trigger in the shipped game.

| Function | What goes wrong |
|---|---|
| `ItemList_MoveUp`, `ItemList_MoveDown` | Load from the head or tail node's `prev`/`next` before the NULL check |
| `Race_EstimateFinishTimes` | Reuses the outer loop counter and never advances `j` |
| `Grid_AddObjToCellsInRect` | Steps by grid width instead of height (all retail grids are square) |
| `Ui_InsertElementBefore` | Drops the head node when the match is the second node |
| `Clock_Step` (800637F4) | Multiplies by 1e-9 instead of 1e9, so the clock never advances |
| `Gfx_CalcBounds` (800585D0) | Reads the F3DEX2 vertex count with the F3DEX1 layout |
| `Gfx_SubmitTask` perf overlay | Divide by zero when two tasks finish in one retrace; the overlay bit is never set in retail |

## For a port

The port branch can fix these, since it no longer has to match. A fix that changes game behaviour (AI targeting, tie-breaks, lap counting) changes how the game plays, so it is worth keeping the original behaviour behind an option.
