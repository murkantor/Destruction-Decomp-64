#include "common.h"
#include "m2c.h"

s32 func_80055A0C(s32, M2C_UNK, M2C_UNK);           /* extern */
M2C_UNK func_80063BFC(s32, s32);                    /* extern */
s32 func_80063D6C();                                /* extern */

s32 func_80062780(s32 arg0) {
    s32 temp_v0;

    temp_v0 = func_80055A0C(func_80063D6C(), 7, 0);
    func_80063BFC(arg0, temp_v0);
    return temp_v0;
}

M2C_UNK func_80055AB4();                            /* extern */

void func_800627C8(void) {
    func_80055AB4();
}
