#include "common.h"
#include "m2c.h"

s32 __osDisableInt();                                /* extern */
M2C_UNK __osRestoreInt(s32);                         /* extern */
extern void *D_800B6C24;

void osViSwapBuffer(s32 arg0) {
    s32 temp_v0;

    temp_v0 = __osDisableInt();
    M2C_FIELD(D_800B6C24, s32 *, 4) = arg0;
    M2C_FIELD(D_800B6C24, u16 *, 0) = (u16) (M2C_FIELD(D_800B6C24, u16 *, 0) | 0x10);
    __osRestoreInt(temp_v0);
}
