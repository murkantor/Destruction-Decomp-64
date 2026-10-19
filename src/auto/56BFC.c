#include "common.h"
#include "m2c.h"

extern s32 D_800B0B18;
void osCreateViManager(s32);

s32 func_80055FFC(s32 skip) {
    if (skip) {
        if (D_800B0B18 != 0) {
            return 0;
        }
        D_800B0B18 = 1;
    }
    if (D_800B0B18 != 0) {
        return 0;
    }
    osCreateViManager(0xFE);
    D_800B0B18 = 1;
    return 1;
}
