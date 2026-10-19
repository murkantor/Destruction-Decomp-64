#include "common.h"
#include "m2c.h"

void func_80056578(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s32 *, 0xA8) = arg1;
}

void func_80056580(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s32 *, 0xB8) = arg1;
}

void func_80056588(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    if ((M2C_FIELD(arg0, s32 *, 0x98) == 0) && (M2C_FIELD(arg0, s32 *, 0x9C) == 0)) {
        M2C_FIELD(arg0, s32 *, 0x98) = arg1;
        M2C_FIELD(arg0, s32 *, 0x9C) = arg2;
        M2C_FIELD(arg0, s32 *, 0xA0) = arg3;
        M2C_FIELD(arg0, s32 *, 0xA4) = arg4;
    }
}
