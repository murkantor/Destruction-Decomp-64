#include "common.h"
#include "m2c.h"

M2C_UNK Obj_Free(M2C_UNK);                     /* extern */
s32 Array_Find(M2C_UNK, void *, s16);            /* extern */
M2C_UNK Array_SwapRemove(void *, s32, s16);            /* extern */

void func_800241F4(void *arg0, M2C_UNK arg1) {
    void *temp_s1;
    void *temp_s1_2;

    temp_s1 = arg0 + 0x40C;
    Array_SwapRemove(temp_s1, Array_Find(arg1, temp_s1, M2C_FIELD(arg0, s16 *, 0xD12)), M2C_FIELD(arg0, s16 *, 0xD12));
    temp_s1_2 = arg0 + 0xC10;
    M2C_FIELD(arg0, s16 *, 0xD12) = (s16) ((u16) M2C_FIELD(arg0, s16 *, 0xD12) - 1);
    Array_SwapRemove(temp_s1_2, Array_Find(arg1, temp_s1_2, M2C_FIELD(arg0, s16 *, 0xD1C)), M2C_FIELD(arg0, s16 *, 0xD1C));
    M2C_FIELD(arg0, s16 *, 0xD1C) = (s16) ((u16) M2C_FIELD(arg0, s16 *, 0xD1C) - 1);
    Obj_Free(arg1);
}

s32 Array_Find(M2C_UNK, void *, s16);            /* extern */
M2C_UNK Array_SwapRemove(void *, s32, s16);            /* extern */

void func_80024290(void *arg0, M2C_UNK arg1) {
    void *temp_s1;

    temp_s1 = arg0 + 0x40C;
    Array_SwapRemove(temp_s1, Array_Find(arg1, temp_s1, M2C_FIELD(arg0, s16 *, 0xD12)), M2C_FIELD(arg0, s16 *, 0xD12));
    M2C_FIELD(arg0, s16 *, 0xD12) = (s16) ((u16) M2C_FIELD(arg0, s16 *, 0xD12) - 1);
}
