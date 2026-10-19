#include "common.h"
#include "m2c.h"

M2C_UNK func_800748A0(void *);                      /* extern */

void func_8006D900(void *arg0) {
    void *temp_a0;

    temp_a0 = M2C_FIELD(arg0, void **, 0x274);
    if (M2C_FIELD(temp_a0, s32 *, 0x10) == 1) {
        M2C_FIELD(temp_a0, s32 *, 4) = (s32) (M2C_FIELD(temp_a0, s32 *, 4) | 0x10);
        func_800748A0(temp_a0);
    }
}
