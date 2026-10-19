#include "common.h"
#include "m2c.h"

M2C_UNK func_80062FB8(M2C_UNK);                     /* extern */
s32 func_80064428(M2C_UNK, void *, s16);            /* extern */
M2C_UNK func_80064464(void *, s32, s16);            /* extern */

void func_80024304(void *arg0, M2C_UNK arg1) {
    s32 temp_v0;
    void *temp_s1;

    temp_s1 = arg0 + 0x40C;
    temp_v0 = func_80064428(arg1, temp_s1, M2C_FIELD(arg0, s16 *, 0xD12));
    if (temp_v0 >= 0) {
        func_80064464(temp_s1, temp_v0, M2C_FIELD(arg0, s16 *, 0xD12));
        M2C_FIELD(arg0, s16 *, 0xD12) = (s16) ((u16) M2C_FIELD(arg0, s16 *, 0xD12) - 1);
    }
    func_80062FB8(arg1);
}
