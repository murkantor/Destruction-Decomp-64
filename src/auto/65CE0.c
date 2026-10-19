#include "common.h"
#include "m2c.h"

s32 func_800650E0(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s8 *, 0xCD) = 0;
    M2C_FIELD(arg0, s32 *, 0x64) = 0;
    return arg1;
}
