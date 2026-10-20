#include "common.h"
#include "m2c.h"

extern s32 D_800C06F0;

s32 Timer_GetRate(void) {
    return D_800C06F0;
}

extern f32 D_800C06F4;

f32 func_80062F40(void) {
    return D_800C06F4;
}

extern f32 D_800C0708;

f32 func_80062F4C(void) {
    return D_800C0708;
}

extern s32 D_800C070C;

s32 Fps_GetDeltaFrames(void) {
    return D_800C070C;
}
