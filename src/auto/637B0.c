#include "common.h"
#include "m2c.h"

extern u64 D_800C06F8;
extern u64 D_800C0700;
extern f32 D_800C0708;
extern s32 D_800C070C;

u64 osGetTime(void);
s32 func_80062F34(void);

void Fps_Update(void) {
    u64 now;
    u64 x;
    u64 delta;
    s32 v;

    now = (osGetTime() * 64) / 3;
    delta = now - D_800C06F8;
    x = delta * func_80062F34();
    v = x * 1e-9 + 0.5;
    D_800C070C = v;
    if (func_80062F34() > 0) {
        D_800C0708 = (f32)(u32)v / func_80062F34();
    } else {
        D_800C0708 = 0.0f;
    }
    D_800C06F8 = now;
    D_800C0700 = D_800C0708 * 1000000000.0;
}
