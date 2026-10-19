#include "common.h"
#include "m2c.h"

s32 func_800192C0(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 4);
}

s32 func_800192C8(s32 *arg0) {
    return *arg0;
}

void func_800192D0(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s32 *, 8) = arg1;
}
