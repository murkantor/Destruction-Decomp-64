#include "common.h"
#include "m2c.h"

void *Mem_AllocAligned(M2C_UNK, M2C_UNK, M2C_UNK);     /* extern */

void func_8006043C(void) {
    void *temp_v0;

    temp_v0 = Mem_AllocAligned(0x24, 0x3F, 0);
    M2C_FIELD(temp_v0, s32 *, 0x14) = 0;
    M2C_FIELD(temp_v0, s32 *, 0x18) = 0;
    M2C_FIELD(temp_v0, s32 *, 0x1C) = 0;
    M2C_FIELD(temp_v0, s32 *, 0x20) = 0;
}
