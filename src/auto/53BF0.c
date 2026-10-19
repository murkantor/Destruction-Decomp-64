#include "common.h"
#include "m2c.h"

s32 func_80050B70(M2C_UNK, s32, M2C_UNK, s32);      /* extern */
s32 func_80052F18();                                /* extern */

s32 func_80052FF0(void *arg0, void *arg1, M2C_UNK arg2) {
    s32 var_v0;

    var_v0 = func_80052F18();
    if (var_v0 == 0) {
        var_v0 = -(func_80050B70(arg2, M2C_FIELD(arg1, s32 *, 0x14), 1, M2C_FIELD(arg0, s32 *, 4)) != 1);
    }
    return var_v0;
}
