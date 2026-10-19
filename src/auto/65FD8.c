#include "common.h"
#include "m2c.h"

s32 func_800653D8(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s8 *, 0xC6) = 0;
    return arg1;
}

s32 func_800653E4(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s8 *, 0xCA) = 1;
    return arg1;
}

s32 func_800653F4(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s8 *, 0xCA) = 0;
    return arg1;
}
