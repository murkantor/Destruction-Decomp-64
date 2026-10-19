#include "common.h"
#include "m2c.h"

M2C_UNK func_800135AC(M2C_UNK);                     /* extern */
s32 func_80064428(M2C_UNK, void *, s16);            /* extern */
M2C_UNK func_80064464(void *, s32, s16);            /* extern */

void func_800241F4(void *arg0, M2C_UNK arg1) {
    void *temp_s1;
    void *temp_s1_2;

    temp_s1 = arg0 + 0x40C;
    func_80064464(temp_s1, func_80064428(arg1, temp_s1, M2C_FIELD(arg0, s16 *, 0xD12)), M2C_FIELD(arg0, s16 *, 0xD12));
    temp_s1_2 = arg0 + 0xC10;
    M2C_FIELD(arg0, s16 *, 0xD12) = (s16) ((u16) M2C_FIELD(arg0, s16 *, 0xD12) - 1);
    func_80064464(temp_s1_2, func_80064428(arg1, temp_s1_2, M2C_FIELD(arg0, s16 *, 0xD1C)), M2C_FIELD(arg0, s16 *, 0xD1C));
    M2C_FIELD(arg0, s16 *, 0xD1C) = (s16) ((u16) M2C_FIELD(arg0, s16 *, 0xD1C) - 1);
    func_800135AC(arg1);
}

s32 func_80064428(M2C_UNK, void *, s16);            /* extern */
M2C_UNK func_80064464(void *, s32, s16);            /* extern */

void func_80024290(void *arg0, M2C_UNK arg1) {
    void *temp_s1;

    temp_s1 = arg0 + 0x40C;
    func_80064464(temp_s1, func_80064428(arg1, temp_s1, M2C_FIELD(arg0, s16 *, 0xD12)), M2C_FIELD(arg0, s16 *, 0xD12));
    M2C_FIELD(arg0, s16 *, 0xD12) = (s16) ((u16) M2C_FIELD(arg0, s16 *, 0xD12) - 1);
}
