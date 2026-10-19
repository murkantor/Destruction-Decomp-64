#include "common.h"
#include "m2c.h"

s32 Array_Find(M2C_UNK, void *, s16);            /* extern */
M2C_UNK Array_SwapRemove(void *, s32, s16);            /* extern */

void func_80024028(void *arg0, M2C_UNK arg1) {
    s32 temp_v0;
    void *temp_s1;

    temp_s1 = arg0 + 0x40C;
    temp_v0 = Array_Find(arg1, temp_s1, M2C_FIELD(arg0, s16 *, 0xD12));
    if (temp_v0 >= 0) {
        Array_SwapRemove(temp_s1, temp_v0, M2C_FIELD(arg0, s16 *, 0xD12));
        M2C_FIELD(arg0, s16 *, 0xD12) = (s16) ((u16) M2C_FIELD(arg0, s16 *, 0xD12) - 1);
    }
}

M2C_UNK func_800107CC(M2C_UNK);                     /* extern */
s32 Array_Find(M2C_UNK, void *, s16);            /* extern */
M2C_UNK Array_SwapRemove(void *, s32, s16);            /* extern */

void func_80024084(void *arg0, M2C_UNK arg1) {
    s32 temp_v0;
    s32 temp_v0_2;
    void *temp_s1;
    void *temp_s1_2;

    temp_s1 = arg0 + 0x40C;
    temp_v0 = Array_Find(arg1, temp_s1, M2C_FIELD(arg0, s16 *, 0xD12));
    if (temp_v0 >= 0) {
        Array_SwapRemove(temp_s1, temp_v0, M2C_FIELD(arg0, s16 *, 0xD12));
        M2C_FIELD(arg0, s16 *, 0xD12) = (s16) ((u16) M2C_FIELD(arg0, s16 *, 0xD12) - 1);
    }
    temp_s1_2 = arg0 + 0xB90;
    temp_v0_2 = Array_Find(arg1, temp_s1_2, M2C_FIELD(arg0, s16 *, 0xD1A));
    if (temp_v0_2 >= 0) {
        Array_SwapRemove(temp_s1_2, temp_v0_2, M2C_FIELD(arg0, s16 *, 0xD1A));
        M2C_FIELD(arg0, s16 *, 0xD1A) = (s16) ((u16) M2C_FIELD(arg0, s16 *, 0xD1A) - 1);
    }
    func_800107CC(arg1);
}
