#include "common.h"
#include "m2c.h"

extern s32 D_80095850[12];

void func_8002BF08(void) {
    s32 none = -1;
    s32 i;

    for (i = 11; i >= 0; i--) {
        D_80095850[i] = none;
    }
}
