#include "common.h"
#include "m2c.h"

extern f32 D_800B4D30;

void func_8005D4AC(void *arg0, s32 *arg1) {
    if (M2C_FIELD(arg0, s32 *, 0xC) == 2) {
        *arg1 = (s32) ((f32) *arg1 * D_800B4D30);
    }
}

void func_8005D4E0(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x40) = 0;
}
