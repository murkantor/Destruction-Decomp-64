#include "common.h"
#include "m2c.h"

void func_80060F8C(void *arg0, s32 arg1, s32 arg2) {
    void *temp_v1;

    temp_v1 = M2C_FIELD(arg0, void **, 0x14);
    M2C_FIELD(temp_v1, s16 *, 0) = (s16) ((s32) -((s32) (arg1 * M2C_FIELD(temp_v1, u16 *, 4)) >> 9) / (s32) M2C_FIELD(temp_v1, u16 *, 2));
    M2C_FIELD(temp_v1, s16 *, 8) = (s16) ((s32) -((s32) (arg2 * M2C_FIELD(temp_v1, u16 *, 0xC)) >> 9) / (s32) M2C_FIELD(temp_v1, u16 *, 0xA));
}
