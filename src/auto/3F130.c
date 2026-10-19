#include "common.h"
#include "m2c.h"

s32 func_8000B1D0(s32);                             /* extern */

s32 func_8003E530(void *arg0, s32 arg1) {
    s32 var_a1;

    var_a1 = arg1;
    if (M2C_FIELD(arg0, s16 *, 0) == 0x18) {
        var_a1 = func_8000B1D0(M2C_FIELD(arg0, s32 *, 0xC));
    }
    return var_a1;
}
