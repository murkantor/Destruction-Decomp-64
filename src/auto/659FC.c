#include "common.h"
#include "m2c.h"

s32 func_80064DFC(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s8 *, 0xB0) = 0;
    return arg1;
}
