#include "common.h"
#include "m2c.h"

s32 __osDisableInt();                                /* extern */
M2C_UNK __osRestoreInt(s32);                         /* extern */
extern void *D_800B6C24;

void func_8006C430(f32 arg0) {
    s32 temp_v0;

    temp_v0 = __osDisableInt();
    M2C_FIELD(D_800B6C24, f32 *, 0x24) = arg0;
    M2C_FIELD(D_800B6C24, u16 *, 0) = (u16) (M2C_FIELD(D_800B6C24, u16 *, 0) | 4);
    __osRestoreInt(temp_v0);
}
