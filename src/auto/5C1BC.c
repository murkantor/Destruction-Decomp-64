#include "common.h"
#include "m2c.h"

void func_8005B5BC(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6) {
    M2C_FIELD(arg0, s32 *, 0) = 0;
    M2C_FIELD(arg0, s32 *, 4) = arg1;
    M2C_FIELD(arg0, s32 *, 8) = arg2;
    M2C_FIELD(arg0, s32 *, 0xC) = arg3;
    M2C_FIELD(arg0, s32 *, 0x10) = arg4;
    M2C_FIELD(arg0, s32 *, 0x14) = arg5;
    M2C_FIELD(arg0, s32 *, 0x18) = arg6;
}
