#include "common.h"
#include "m2c.h"

M2C_UNK func_8005A970(M2C_UNK, M2C_UNK, s32, s32);  /* extern */
M2C_UNK func_8005AA68(s32, M2C_UNK);                /* extern */
M2C_UNK func_8005AB4C(s32, f32);                    /* extern */

void func_8005F9C0(s32 *arg0, M2C_UNK arg1, s32 arg2, M2C_UNK arg3, s32 arg4, s32 arg5, f32 arg6) {
    func_8005A970(arg1, arg3, arg4, arg5);
    func_8005AA68(arg2, arg1);
    func_8005AB4C(arg2, arg6);
    *arg0 = arg2;
}
