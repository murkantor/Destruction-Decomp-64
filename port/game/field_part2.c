/*
 * The doubled field ([game] double_field, port/src/field.c): per-car tables
 * that hold 12 cars on the N64 and are replaced on PC by RACE_CARS-long ones.
 *
 * The engine sounds: a pair of sound ids per car, by the car's index in the race
 * list (D_801311D0, 12 entries of 0x10 bytes; with more cars the N64 table runs
 * into D_80131290 and on). Sound_StartCarLoops (7620.c), EngineSfx_UpdateAll
 * (78A8.c) and EngineSfx_Crossfade (8884.c) use this table on PC, and
 * EngineSfx_UpdateAll calls Port_FieldSfxStopPair instead of Sfx_StopPair
 * (8B68.c), the only other user of the N64 table.
 */
#include "common.h"

typedef struct {
    s32 x0;  /* the sound playing */
    s32 x4;  /* the sound fading out (Sfx_StopPair: the pair) */
    s32 x8;
    u8 xC;
    u8 padD[3];
} FieldEngineSfx;

FieldEngineSfx gFieldEngineSfx[RACE_CARS];

void List_RemoveById(void *, s16);

/* Sfx_StopPair (8B68.c) on gFieldEngineSfx */
void Port_FieldSfxStopPair(void *ctx, s32 i) {
    FieldEngineSfx *s = &gFieldEngineSfx[i];

    if (s->x0 != 0) {
        List_RemoveById(ctx, s->x0);
        s->x0 = 0;
    }
    if (s->x4 != 0) {
        List_RemoveById(ctx, s->x4);
        s->x4 = 0;
    }
}
