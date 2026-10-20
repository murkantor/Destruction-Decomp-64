#include "common.h"
#include "m2c.h"

void *Res_Find();                              /* extern */

s32 Res_GetSize(s32 arg0) {
    return M2C_FIELD(Res_Find(arg0), s32 *, 0x18);
}

void *Res_Find();                              /* extern */

s32 func_80063D8C(s32 arg0) {
    return M2C_FIELD(Res_Find(arg0), s32 *, 4);
}

void *Res_Find();                              /* extern */

s32 func_80063DAC(s32 arg0) {
    return M2C_FIELD(Res_Find(arg0), s32 *, 8);
}

void *Res_FindType();                              /* extern */

s32 func_80063DCC(s32 arg0) {
    return M2C_FIELD(Res_FindType(arg0), s32 *, 4);
}

void *Res_FindType();                              /* extern */

s32 func_80063DEC(s32 arg0) {
    return M2C_FIELD(Res_FindType(arg0), s32 *, 8);
}
