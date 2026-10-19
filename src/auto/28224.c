#include "common.h"
#include "m2c.h"

s32 Mem_AllocAlignedAlt(M2C_UNK, M2C_UNK, M2C_UNK);       /* extern */
M2C_UNK func_80063BFC(s32, s32);                    /* extern */

s32 func_80027624(s32 arg0) {
    s32 temp_v0;

    temp_v0 = Mem_AllocAlignedAlt(0x150, 7, 0);
    func_80063BFC(arg0, temp_v0);
    return temp_v0;
}
