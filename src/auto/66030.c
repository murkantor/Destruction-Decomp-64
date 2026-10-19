#include "common.h"
#include "m2c.h"

void *func_80065430(void *arg0, u8 *arg1) {
    void *temp_v1;

    temp_v1 = M2C_FIELD(arg0, void **, 0x70);
    M2C_FIELD(arg0, void **, 0x7C) = (void *) (temp_v1 + *(M2C_UNK *)((*arg1 * 4) + M2C_FIELD(temp_v1, s32 *, 0x14)));
    return arg1 + 1;
}
