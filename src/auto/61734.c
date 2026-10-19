#include "common.h"
#include "m2c.h"

void func_80060B34(void *arg0, s16 arg1) {
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x18), s16 *, 0x14) = arg1;
}

void func_80060B40(void *arg0, s16 arg1) {
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x18), s16 *, 0x16) = arg1;
}

void func_80060B4C(void *arg0, s16 arg1, s16 arg2) {
    void *temp_v0;

    temp_v0 = M2C_FIELD(arg0, void **, 0x18);
    M2C_FIELD(temp_v0, s16 *, 0x14) = arg1;
    M2C_FIELD(temp_v0, s16 *, 0x16) = arg2;
}

void func_80060B5C(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    void *temp_a0;

    temp_a0 = M2C_FIELD(arg0, void **, 0x14);
    M2C_FIELD(temp_a0, s16 *, 0xC) = (s16) (arg4 << 5);
    M2C_FIELD(temp_a0, s16 *, 4) = (s16) (arg3 << 5);
    M2C_FIELD(temp_a0, s16 *, 0x12) = (s16) ((arg1 >> (4 - M2C_FIELD(temp_a0, u8 *, 0x15))) + (arg2 * M2C_FIELD(temp_a0, u16 *, 0x10)));
}
