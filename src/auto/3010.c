#include "common.h"
#include "m2c.h"

s32 Mem_AllocAligned(s32, M2C_UNK, M2C_UNK);           /* extern */
M2C_UNK Res_Load(s32, s32);                    /* extern */
s32 Res_GetSize();                                /* extern */

s32 func_80002410(s32 arg0) {
    s32 temp_v0;

    temp_v0 = Mem_AllocAligned(Res_GetSize(), 7, 0);
    Res_Load(arg0, temp_v0);
    return temp_v0;
}
