#include "common.h"
#include "m2c.h"

extern s16 D_800CCAB4;
extern s16 D_800CCAB6;

void Sound_SetMasterVolume(s32 arg0, s16 arg1) {
    if (arg0 & 1) {
        D_800CCAB4 = arg1;
    }
    if (arg0 & 2) {
        D_800CCAB6 = arg1;
    }
}
