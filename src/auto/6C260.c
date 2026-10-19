#include "common.h"
#include "m2c.h"

M2C_UNK func_8006B0BC(s32, void *);                 /* extern */
M2C_UNK func_8006B0DC(s32);                         /* extern */

void func_8006B660(void *arg0) {
    s32 var_s0;
    void *var_s1;

    var_s1 = arg0;
    var_s0 = M2C_FIELD(var_s1, s32 *, 0x14);
    if (var_s0 != 0) {
        do {
            func_8006B0DC(var_s0);
            func_8006B0BC(var_s0, var_s1 + 4);
            var_s0 = M2C_FIELD(var_s1, s32 *, 0x14);
        } while (var_s0 != 0);
    }
}
