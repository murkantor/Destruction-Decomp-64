#include "common.h"
#include "m2c.h"

extern s32 D_800D1C50;
extern s32 D_801260D0;

void Collision_ResetLastPair(void) {
    D_800D1C50 = -1;
    D_801260D0 = -1;
}
