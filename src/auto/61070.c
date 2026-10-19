#include "common.h"
#include "m2c.h"

void *func_80055A0C(M2C_UNK, M2C_UNK, M2C_UNK);     /* extern */

void *func_80060470(void) {
    void *temp_v0;

    temp_v0 = func_80055A0C(0x24, 0x3F, 0);
    M2C_FIELD(temp_v0, void **, 0x14) = NULL;
    M2C_FIELD(temp_v0, void **, 0x18) = NULL;
    M2C_FIELD(temp_v0, void **, 0x1C) = NULL;
    M2C_FIELD(temp_v0, void **, 0x20) = NULL;
    M2C_FIELD(temp_v0, void **, 0x14) = func_80055A0C(0x18, 0x3F, 0);
    M2C_FIELD(temp_v0, void **, 0x18) = func_80055A0C(0x18, 0x3F, 0);
    M2C_FIELD(temp_v0, void **, 0x1C) = func_80055A0C(0x18, 0x3F, 0);
    M2C_FIELD(temp_v0, void **, 0x20) = func_80055A0C(0x18, 0x3F, 0);
    return temp_v0;
}
