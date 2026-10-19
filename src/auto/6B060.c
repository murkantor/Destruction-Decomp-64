#include "common.h"
#include "m2c.h"

extern M2C_UNK D_800B56B0;

void func_8006A460(void *arg0, s32 arg1, s32 arg2) {
    M2C_FIELD(arg0, M2C_UNK **, 0) = &D_800B56B0;
    M2C_FIELD(arg0, M2C_UNK **, 4) = &D_800B56B0;
    M2C_FIELD(arg0, s32 *, 8) = 0;
    M2C_FIELD(arg0, s32 *, 0xC) = 0;
    M2C_FIELD(arg0, s32 *, 0x10) = arg2;
    M2C_FIELD(arg0, s32 *, 0x14) = arg1;
}
