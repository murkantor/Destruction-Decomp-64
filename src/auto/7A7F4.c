#include "common.h"
#include "m2c.h"

extern s32 D_800D1080;

void srand(s32 arg0) {
    D_800D1080 = arg0;
}
