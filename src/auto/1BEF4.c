#include "common.h"
#include "m2c.h"

#ifdef TARGET_PC
#define D_800D1C20 gFieldCarIds /* RACE_CARS long (port/game/field_part0.c) */
#endif
extern M2C_UNK D_800D1C20;

s32 Race_GetSlotCarId(s32 arg0) {
    return *((__typeof__(D_800D1C20) *)((s8 *)&D_800D1C20 + (arg0 * 4)));
}
