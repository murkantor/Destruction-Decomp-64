#include "common.h"
#include "m2c.h"

extern s32 D_80131F50[12];
extern s32 D_801252F8[64];

void func_80014DC0(void) {
    s32 none;
    s32 i;

    none = -1;
    for (i = 11; i >= 0; i--) {
        D_80131F50[i] = none;
    }
    none = -1;
    for (i = 63; i >= 0; i--) {
        D_801252F8[i] = none;
    }
}
