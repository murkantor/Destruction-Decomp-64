#include "common.h"
#include "m2c.h"

M2C_UNK func_80063890();                            /* extern */

void func_800635B0(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s8 *, 0x1C) = 0;
    M2C_FIELD(arg0, s32 *, 0x18) = arg1;
    func_80063890();
}
