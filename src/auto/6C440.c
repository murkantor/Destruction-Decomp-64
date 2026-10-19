#include "common.h"
#include "m2c.h"

s32 osSetIntMask(s32);                             /* extern */

void func_8006B840(void *arg0, void *arg1) {
    s32 temp_v0;

    temp_v0 = osSetIntMask(1);
    M2C_FIELD(arg1, s32 *, 0x10) = (s32) M2C_FIELD(arg0, s32 *, 0x20);
    M2C_FIELD(arg1, void **, 0) = (void *) M2C_FIELD(arg0, void **, 0);
    M2C_FIELD(arg0, void **, 0) = arg1;
    osSetIntMask(temp_v0);
}
