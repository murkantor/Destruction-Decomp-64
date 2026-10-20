#include "common.h"
#include "m2c.h"

M2C_UNK Mem_Free();                            /* extern */
extern s32 D_80096074;

void Tag_Free(void) {
    Mem_Free();
    D_80096074 = 0;
}
