#include "common.h"
#include "m2c.h"

f32 func_80059B60(f32, f32);                        /* extern */

void func_8005A610(f32 *arg0, void *arg1) {
    *arg0 = func_80059B60(M2C_FIELD(arg1, f32 *, 4), M2C_FIELD(arg1, f32 *, 0));
}
