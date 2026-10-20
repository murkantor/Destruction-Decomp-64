#include "common.h"
#include "m2c.h"

extern s32 D_800B04A0;
extern M2C_UNK D_800BCA60;

void DebugMenu_SetCurrent(s32 arg0) {
    *((__typeof__(D_800BCA60) *)((s8 *)&D_800BCA60 + (D_800B04A0 * 4))) = arg0;
}


void DebugMenu_Push(s32 arg0) {
    s32 temp_v0;

    temp_v0 = D_800B04A0 + 1;
    D_800B04A0 = temp_v0;
    *((__typeof__(D_800BCA60) *)((s8 *)&D_800BCA60 + (temp_v0 * 4))) = arg0;
}
