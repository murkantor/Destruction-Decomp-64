#include "common.h"
#include "m2c.h"

M2C_UNK memset(void *, M2C_UNK, M2C_UNK);    /* extern */
extern M2C_UNK D_801253FC;
extern M2C_UNK D_80125408;

void Cheat_ResetInput(s8 arg0) {
    s32 temp_s0;

    temp_s0 = arg0 * 0x18;
    memset(temp_s0 + (s8 *)&D_801253FC, 0, 0xB);
    *((__typeof__(D_80125408) *)((s8 *)&D_80125408 + temp_s0)) = 0;
}
