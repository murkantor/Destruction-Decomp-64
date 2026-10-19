#include "common.h"
#include "m2c.h"

s32 func_80070D90();                                /* extern */
M2C_UNK func_80070DB0(s32);                         /* extern */
extern void *D_800B6C24;

void func_80074A30(s32 arg0, s32 arg1, s16 arg2) {
    s32 temp_a0;

    temp_a0 = func_80070D90();
    M2C_FIELD(D_800B6C24, s32 *, 0x10) = arg0;
    M2C_FIELD(D_800B6C24, s32 *, 0x14) = arg1;
    M2C_FIELD(D_800B6C24, s16 *, 2) = arg2;
    func_80070DB0(temp_a0);
}
