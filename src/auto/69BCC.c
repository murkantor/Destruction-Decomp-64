#include "common.h"
#include "m2c.h"

void func_80068FCC(void *arg0, f32 arg1, f32 arg2, f32 arg3, f32 *arg4, f32 *arg5, f32 *arg6) {
    *arg4 = (M2C_FIELD(arg0, f32 *, 0) * arg1) + (M2C_FIELD(arg0, f32 *, 0x10) * arg2) + (M2C_FIELD(arg0, f32 *, 0x20) * arg3) + M2C_FIELD(arg0, f32 *, 0x30);
    *arg5 = (M2C_FIELD(arg0, f32 *, 4) * arg1) + (M2C_FIELD(arg0, f32 *, 0x14) * arg2) + (M2C_FIELD(arg0, f32 *, 0x24) * arg3) + M2C_FIELD(arg0, f32 *, 0x34);
    *arg6 = (M2C_FIELD(arg0, f32 *, 8) * arg1) + (M2C_FIELD(arg0, f32 *, 0x18) * arg2) + (M2C_FIELD(arg0, f32 *, 0x28) * arg3) + M2C_FIELD(arg0, f32 *, 0x38);
}
