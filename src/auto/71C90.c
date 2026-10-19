#include "common.h"
#include "m2c.h"

extern void *D_800B56C0;

s32 func_80071090(void *arg0) {
    void *var_a0;

    var_a0 = arg0;
    if (var_a0 == NULL) {
        var_a0 = D_800B56C0;
    }
    return M2C_FIELD(var_a0, s32 *, 4);
}
