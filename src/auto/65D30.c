#include "common.h"
#include "m2c.h"

s32 func_80065130(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s8 *, 0xAF) = 1;
    return arg1;
}

void *func_80065140(void *arg0, u8 *arg1) {
    M2C_FIELD(arg0, u8 *, 0xB1) = (u8) *arg1;
    return arg1 + 1;
}

s32 func_80065150(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s8 *, 0xB2) = 1;
    return arg1;
}
