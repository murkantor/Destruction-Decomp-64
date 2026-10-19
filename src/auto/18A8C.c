#include "common.h"
#include "m2c.h"

s32 func_80017E8C(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s32 *, 0x2AC) = arg1;
    return arg1 + 0x18;
}
