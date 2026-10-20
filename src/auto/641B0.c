#include "common.h"
#include "m2c.h"

M2C_UNK Clock_Reset();                            /* extern */

void Clock_Init(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s8 *, 0x1C) = 0;
    M2C_FIELD(arg0, s32 *, 0x18) = arg1;
    Clock_Reset();
}
