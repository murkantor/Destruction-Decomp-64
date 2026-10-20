#include "common.h"
#include "m2c.h"

f32 func_80039DA4(f32, f32, f32, f32);              /* extern */
f32 Clock_GetDeltaSeconds(M2C_UNK *);                       /* extern */
extern f32 D_8009733C;
extern M2C_UNK gGameTime;

void func_8003E490(f32 arg0, f32 *arg1, f32 *arg2) {
    f32 temp_f0;
    f32 temp_f14;

    temp_f0 = Clock_GetDeltaSeconds(&gGameTime);
    temp_f14 = *arg1 + (*arg2 * temp_f0);
    *arg1 = temp_f14;
    *arg2 += temp_f0 * func_80039DA4(D_8009733C, temp_f14 - arg0, 2.0f * sqrtf(fabsf(D_8009733C)), *arg2);
}
