#include "common.h"
#include "m2c.h"

extern s8 D_800B4D70;

void Res_SetForceLoad(void) {
    D_800B4D70 = 1;
}


void Res_ClearForceLoad(void) {
    D_800B4D70 = 0;
}
