#include "common.h"
#include "m2c.h"

extern f64 D_800BC170;
extern f64 D_800BC178;

s32 func_8006B6F4(void *arg0, s32 arg1) {
    return (s32) (f32) (((f64) ((f32) arg1 * (f32) M2C_FIELD(arg0, s32 *, 0x44)) / D_800BC170) + D_800BC178);
}

extern f64 D_800BC180;
extern f64 D_800BC188;

s32 _timeToSamples(void *arg0, s32 arg1) {
    return (s32) (f32) (((f64) ((f32) arg1 * (f32) M2C_FIELD(arg0, s32 *, 0x44)) / D_800BC180) + D_800BC188) & ~0xF;
}
