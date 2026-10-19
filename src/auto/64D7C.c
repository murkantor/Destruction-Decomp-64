#include "common.h"
#include "m2c.h"

M2C_UNK func_80023DDC(void *, M2C_UNK);             /* extern */
s32 func_80064428(M2C_UNK, void *, s16);            /* extern */
M2C_UNK func_80064464(void *, s32, s16);            /* extern */

void func_8006417C(void *arg0, M2C_UNK arg1) {
    s32 temp_v0;
    void *temp_s1;

    temp_s1 = arg0 + 0xC;
    temp_v0 = func_80064428(arg1, temp_s1, M2C_FIELD(arg0, s16 *, 0xD10));
    if (temp_v0 >= 0) {
        func_80064464(temp_s1, temp_v0, M2C_FIELD(arg0, s16 *, 0xD10));
        M2C_FIELD(arg0, s16 *, 0xD10) = (s16) ((u16) M2C_FIELD(arg0, s16 *, 0xD10) - 1);
    }
    func_80023DDC(arg0, arg1);
}

s16 func_80064D70();                                /* extern */

s32 func_800641F0(void *arg0) {
    M2C_FIELD(arg0, s16 *, 2) = func_80064D70();
    M2C_FIELD(arg0, s16 *, 0xD10) = 0;
    M2C_FIELD(arg0, s16 *, 0xD12) = 0;
    M2C_FIELD(arg0, s16 *, 0xD14) = 0;
    M2C_FIELD(arg0, s32 *, 0xA0C) = 0;
    M2C_FIELD(arg0, s16 *, 0xD16) = 0;
    M2C_FIELD(arg0, s16 *, 0xD18) = 0;
    M2C_FIELD(arg0, s16 *, 0xD1A) = 0;
    M2C_FIELD(arg0, void **, 4) = arg0;
    return 0;
}
