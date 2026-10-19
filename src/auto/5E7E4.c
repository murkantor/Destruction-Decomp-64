#include "common.h"
#include "m2c.h"

void Mem_Free(void *);                   /* extern */

void func_8005DBE4(void *arg0, s32 arg1) {
    s32 **var_a0;
    s32 **var_v1;

    var_a0 = M2C_FIELD(arg0, s32 ***, 0x8B54);
    var_v1 = NULL;
    if (**var_a0 != arg1) {
        do {
            var_v1 = var_a0;
            var_a0 = M2C_FIELD(var_a0, s32 ***, 4);
        } while (**var_a0 != arg1);
    }
    if (M2C_FIELD(arg0, s32 ***, 0x8B54) == var_a0) {
        M2C_FIELD(arg0, s32 ***, 0x8B54) = (s32 **) M2C_FIELD(var_a0, s32 ***, 4);
    } else {
        M2C_FIELD(var_v1, s32 ***, 4) = (s32 **) M2C_FIELD(var_a0, s32 ***, 4);
    }
    Mem_Free(var_a0);
}
