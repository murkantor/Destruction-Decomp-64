#include "common.h"
#include "m2c.h"

void func_800558FC(void *arg0, s32 *arg1, s32 *arg2) {
    M2C_UNK *var_v0;
    s32 var_v1;

    var_v0 = M2C_FIELD(arg0, M2C_UNK **, 0);
    var_v1 = 0;
    if (var_v0 != NULL) {
        do {
            var_v0 = *var_v0;
            var_v1 += 1;
        } while (var_v0 != NULL);
    }
    *arg1 = var_v1 * M2C_FIELD(arg0, s32 *, 4);
    *arg2 = var_v1;
}
