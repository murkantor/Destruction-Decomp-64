#include "common.h"
#include "m2c.h"

extern s32 D_80124364;
extern s32 D_8013137C;
extern s32 D_80126068[15];
extern s32 D_80125F58[15];

void func_800087C8(void) {
    s32 i;

    D_80124364 = 0;
    D_8013137C = -1;
    for (i = 0; i < 15; i++) {
        D_80126068[i] = -1;
        D_80125F58[i] = 0;
    }
}
