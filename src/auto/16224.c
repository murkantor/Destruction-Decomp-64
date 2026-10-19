#include "common.h"
#include "m2c.h"

s32 func_80015624(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s32 *, 0x2C4) = arg1;
    return arg1 + 0x18;
}
