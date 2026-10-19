#include "common.h"
#include "m2c.h"

s32 osSetIntMask(s32);                             /* extern */

void osScAddClient(void *arg0, void *arg1, s32 arg2) {
    s32 temp_v0;

    temp_v0 = osSetIntMask(1);
    M2C_FIELD(arg1, s32 *, 4) = arg2;
    M2C_FIELD(arg1, void **, 0) = (void *) M2C_FIELD(arg0, void **, 0x260);
    M2C_FIELD(arg0, void **, 0x260) = arg1;
    osSetIntMask(temp_v0);
}
