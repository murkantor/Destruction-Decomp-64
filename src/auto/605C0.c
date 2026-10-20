#include "common.h"
#include "m2c.h"

M2C_UNK Model_Init(M2C_UNK, M2C_UNK, s32, s32);  /* extern */
M2C_UNK ModelInst_Init(s32, M2C_UNK);                /* extern */
M2C_UNK ModelInst_SetScale(s32, f32);                    /* extern */

void ModelInst_Create(s32 *arg0, M2C_UNK arg1, s32 arg2, M2C_UNK arg3, s32 arg4, s32 arg5, f32 arg6) {
    Model_Init(arg1, arg3, arg4, arg5);
    ModelInst_Init(arg2, arg1);
    ModelInst_SetScale(arg2, arg6);
    *arg0 = arg2;
}
