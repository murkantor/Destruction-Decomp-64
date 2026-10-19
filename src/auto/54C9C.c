#include "common.h"
#include "m2c.h"

extern s32 D_800B04A0;

void func_8005409C(void) {
    if (D_800B04A0 > 0) {
        D_800B04A0 -= 1;
    }
}
