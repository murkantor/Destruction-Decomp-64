#include "common.h"
#include "m2c.h"

M2C_UNK *func_80055930(void *arg0) {
    M2C_UNK *temp_v0;

    temp_v0 = M2C_FIELD(arg0, M2C_UNK **, 0);
    if (temp_v0 == NULL) {
        return NULL;
    }
    M2C_FIELD(arg0, M2C_UNK **, 0) = (M2C_UNK *) *temp_v0;
    M2C_FIELD(arg0, s32 *, 0x10) = (s32) (M2C_FIELD(arg0, s32 *, 0x10) - M2C_FIELD(arg0, s32 *, 4));
    return temp_v0;
}

void func_80055960(void *arg0, M2C_UNK *arg1) {
    if (arg1 != NULL) {
        *arg1 = M2C_FIELD(arg0, M2C_UNK **, 0);
        M2C_FIELD(arg0, M2C_UNK **, 0) = arg1;
        M2C_FIELD(arg0, s32 *, 0x10) = (s32) (M2C_FIELD(arg0, s32 *, 0x10) + M2C_FIELD(arg0, s32 *, 4));
    }
}
