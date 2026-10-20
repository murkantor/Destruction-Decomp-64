#include "common.h"
#include "m2c.h"

s32 Car_Draw(s32);                             /* extern */

s32 CarObj_Draw(void *arg0, s32 arg1) {
    s32 var_a1;

    var_a1 = arg1;
    if (M2C_FIELD(arg0, s16 *, 0) == 0x18) {
        var_a1 = Car_Draw(M2C_FIELD(arg0, s32 *, 0xC));
    }
    return var_a1;
}
