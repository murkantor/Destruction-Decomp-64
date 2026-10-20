#include "common.h"
#include "m2c.h"

extern M2C_UNK D_800D1C20;

s32 Race_GetSlotCarId(s32 arg0) {
    return *((__typeof__(D_800D1C20) *)((s8 *)&D_800D1C20 + (arg0 * 4)));
}
