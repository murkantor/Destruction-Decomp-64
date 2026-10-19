#include "common.h"
#include "m2c.h"

s32 func_80010288();                                /* extern */
extern f64 D_800BA370;

f32 func_8003EC70(M2C_UNK arg0, M2C_UNK arg1, f32 arg2) {
    return (f32) ((f64) arg2 - ((f64) (f32) func_80010288() * D_800BA370));
}

s32 func_80010288();                                /* extern */

f32 func_8003ECC0(M2C_UNK arg0, M2C_UNK arg1, f32 arg2) {
    return arg2 - (f32) func_80010288();
}
