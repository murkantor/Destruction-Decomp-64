#include "common.h"
#include "m2c.h"

extern s32 D_800D1080;

int rand(void) {
    D_800D1080 = D_800D1080 * 1103515245 + 12345;
    return ((D_800D1080 + 1) >> 16) & 0x7FFF;
}
