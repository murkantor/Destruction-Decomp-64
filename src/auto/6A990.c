#include "common.h"
#include "m2c.h"

M2C_UNK guMtxIdentF();                              /* extern */

void func_80069D90(void *arg0, f32 arg1, f32 arg2, f32 arg3) {
    guMtxIdentF(arg0);
    M2C_FIELD(arg0, f32 *, 0x30) = arg1;
    M2C_FIELD(arg0, f32 *, 0x34) = arg2;
    M2C_FIELD(arg0, f32 *, 0x38) = arg3;
}
