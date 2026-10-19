#include "common.h"
#include "m2c.h"

void func_8001FA48(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x8C) = (s32) (M2C_FIELD(arg0, s32 *, 0x8C) - 1);
}
