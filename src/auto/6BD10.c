#include "common.h"
#include "m2c.h"

void func_8006B110(void *arg0, s32 arg1, s32 arg2) {
    s32 temp_v1;

    temp_v1 = 0x10 - (arg1 & 0xF);
    if (temp_v1 != 0x10) {
        M2C_FIELD(arg0, s32 *, 0) = (s32) (arg1 + temp_v1);
    } else {
        M2C_FIELD(arg0, s32 *, 0) = arg1;
    }
    M2C_FIELD(arg0, s32 *, 8) = arg2;
    M2C_FIELD(arg0, s32 *, 0xC) = 0;
    M2C_FIELD(arg0, s32 *, 4) = (s32) M2C_FIELD(arg0, s32 *, 0);
}
