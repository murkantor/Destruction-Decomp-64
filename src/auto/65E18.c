#include "common.h"
#include "m2c.h"

s32 func_80065218(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s8 *, 0xCE) = 1;
    return arg1;
}

s32 func_80065228(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s8 *, 0xCE) = 0;
    return arg1;
}

s32 func_80065234(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s8 *, 0xCF) = 1;
    return arg1;
}

s32 func_80065244(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s8 *, 0xCF) = 0;
    return arg1;
}
