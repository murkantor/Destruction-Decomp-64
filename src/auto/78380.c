#include "common.h"
#include "m2c.h"

s32 func_80070D90();                                /* extern */
M2C_UNK func_80070DB0(s32);                         /* extern */
extern s32 D_800B56A0;

void func_80077780(s32 arg0) {
    s32 temp_v0;

    temp_v0 = func_80070D90();
    D_800B56A0 &= ~arg0 | 0x401;
    func_80070DB0(temp_v0);
}
