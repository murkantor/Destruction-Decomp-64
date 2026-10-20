/*
 * The doubled field ([game] double_field, port/src/field.c): the game's tables kept
 * per car slot (Race_FindCarSlot) or per index in the race's car list that hold 12
 * entries on the N64. Entries 0-11 stay in the game's own tables, so code that only
 * knows those (the results screens) reads what it always did; the entries past them,
 * for cars 12 and up and for Race_FindCarSlot's "not found" (the slot one past the
 * last car), are kept here. The game code reaches them through FIELD_* macros that
 * are the plain table access on the N64.
 */
#include "common.h"

#define FIELD_EXTRA (RACE_CARS + 1 - 12)

/* Per slot: the race results tallied for the standings (Results_TallyStandings,
   Race_AwardPot, Race_Setup) */
s32 gFieldD_80095D04[FIELD_EXTRA];    /* points (car x18) */
s32 gFieldD_80095D34[FIELD_EXTRA];    /* car x20 */
s32 gFieldD_80095D64[FIELD_EXTRA];    /* car x1C */
s32 gFieldD_80095D94[FIELD_EXTRA][3]; /* the pot awarded, per round */
s32 gFieldD_80095DF4[FIELD_EXTRA];    /* health left x 100 (arenas) */
u64 gFieldD_80095E28[FIELD_EXTRA][5]; /* lap times, best lap, total */

/* Per car: where each car was at the end of the last update (Race_Setup) */
f32 gFieldD_800D1B08[FIELD_EXTRA][3];
