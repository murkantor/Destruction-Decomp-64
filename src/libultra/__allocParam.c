#include "common.h"
#include "m2c.h"

extern void *alGlobals;

M2C_UNK *__allocParam(void) {
    M2C_UNK *temp_v0;
    M2C_UNK *var_v1;

    temp_v0 = M2C_FIELD(alGlobals, M2C_UNK **, 0x2C);
    var_v1 = NULL;
    if (temp_v0 != NULL) {
        var_v1 = temp_v0;
        M2C_FIELD(alGlobals, M2C_UNK **, 0x2C) = (M2C_UNK *) *var_v1;
        *var_v1 = NULL;
    }
    return var_v1;
}
