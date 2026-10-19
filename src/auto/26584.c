#include "common.h"
#include "m2c.h"

M2C_UNK Mem_Free();                            /* extern */
extern s32 D_80096074;

void func_80025984(void) {
    Mem_Free();
    D_80096074 = 0;
}
