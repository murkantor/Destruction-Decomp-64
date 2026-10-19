#include "common.h"
#include "m2c.h"

void *Res_Find();                              /* extern */

s32 Res_GetSize(void) {
    return M2C_FIELD(Res_Find(), s32 *, 0x18);
}

void *Res_Find();                              /* extern */

s32 func_80063D8C(void) {
    return M2C_FIELD(Res_Find(), s32 *, 4);
}

void *Res_Find();                              /* extern */

s32 func_80063DAC(void) {
    return M2C_FIELD(Res_Find(), s32 *, 8);
}

void *Res_FindType();                              /* extern */

s32 func_80063DCC(void) {
    return M2C_FIELD(Res_FindType(), s32 *, 4);
}

void *Res_FindType();                              /* extern */

s32 func_80063DEC(void) {
    return M2C_FIELD(Res_FindType(), s32 *, 8);
}
