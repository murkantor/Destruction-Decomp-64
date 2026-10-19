#include "common.h"
#include "m2c.h"

s32 func_80070E90(s32);                             /* extern */

void func_8006D138(void *arg0, void *arg1, s32 arg2) {
    s32 temp_v0;

    temp_v0 = func_80070E90(1);
    M2C_FIELD(arg1, s32 *, 4) = arg2;
    M2C_FIELD(arg1, void **, 0) = (void *) M2C_FIELD(arg0, void **, 0x260);
    M2C_FIELD(arg0, void **, 0x260) = arg1;
    func_80070E90(temp_v0);
}
