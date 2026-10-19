#include "common.h"
#include "m2c.h"

extern s32 D_800B04A0;
extern M2C_UNK D_800BCA60;

s32 func_80054028(void) {
    return *((__typeof__(D_800BCA60) *)((s8 *)&D_800BCA60 + (D_800B04A0 * 4)));
}
