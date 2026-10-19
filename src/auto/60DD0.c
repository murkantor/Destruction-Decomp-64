#include "common.h"
#include "m2c.h"

extern s8 D_801301F8;
extern s8 D_801301F9;

void func_800601D0(s32 arg0) {
    if (arg0 & 0xFF) {
        D_801301F8 = 0;
        D_801301F9 = 0;
        return;
    }
    D_801301F8 = 1;
    D_801301F9 = 1;
}
