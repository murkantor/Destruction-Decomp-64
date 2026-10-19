#include "common.h"
#include "m2c.h"

s32 func_80070D90();                                /* extern */
M2C_UNK func_80070DB0(s32);                         /* extern */
extern void *D_800B6C24;

void func_8006C480(s32 arg0) {
    s32 temp_v0;

    temp_v0 = func_80070D90();
    M2C_FIELD(D_800B6C24, s32 *, 4) = arg0;
    M2C_FIELD(D_800B6C24, u16 *, 0) = (u16) (M2C_FIELD(D_800B6C24, u16 *, 0) | 0x10);
    func_80070DB0(temp_v0);
}
