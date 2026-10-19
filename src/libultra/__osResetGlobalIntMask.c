#include "common.h"
#include "m2c.h"

s32 __osDisableInt();                                /* extern */
M2C_UNK __osRestoreInt(s32);                         /* extern */
extern s32 D_800B56A0;

void __osResetGlobalIntMask(s32 arg0) {
    s32 temp_v0;

    temp_v0 = __osDisableInt();
    D_800B56A0 &= ~arg0 | 0x401;
    __osRestoreInt(temp_v0);
}
