#include "common.h"
#include "m2c.h"

void func_80060C0C(void *arg0) {
    void *temp_v0;

    temp_v0 = M2C_FIELD(arg0, void **, 0x18);
    M2C_FIELD(temp_v0, s32 *, 0) = 0x10000;
    M2C_FIELD(temp_v0, s32 *, 0xC) = 0x10000;
    M2C_FIELD(temp_v0, s32 *, 4) = 0;
    M2C_FIELD(temp_v0, s32 *, 8) = 0;
    M2C_FIELD(temp_v0, s16 *, 0x12) = 0;
    M2C_FIELD(temp_v0, s16 *, 0x10) = 0;
    M2C_FIELD(temp_v0, s16 *, 0x16) = 0x400;
    M2C_FIELD(temp_v0, s16 *, 0x14) = 0x400;
}
