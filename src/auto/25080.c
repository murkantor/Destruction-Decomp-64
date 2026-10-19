#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[0xC]; u32 xC; u8 pad10[8]; s16 x18; s16 x1A; u8 pad1C[0x94]; } Ctrl;
typedef struct { u8 x0; u8 x1; u8 x2; u8 x3; s16 x4; } Wheel;
extern f64 D_800B8C80;
extern f32 D_80095828;
extern u8 gGameTime[];
extern u64 D_80095830;
f32 Math_Atan2(f32, f32);
f64 fabs(f64);
u64 Time_Get(void *);

void Input_ReadWheel(Ctrl *c, s32 idx, Wheel *w) {
    u32 btnA;
    u32 btnB;
    s16 x;
    s16 y;
    f32 ang;

    btnA = c[idx].xC & 0x8000;
    btnB = c[idx].xC & 0x4000;
    if (c[idx].x18 >= 8) {
        x = c[idx].x18 - 7;
    } else if (c[idx].x18 < -7) {
        x = c[idx].x18 + 7;
    } else {
        x = 0;
    }
    w->x4 = (x < 61) ? ((x < -61) ? -61 : x) : 61;
    w->x3 = 0;
    if (btnB) {
        if (c[idx].x1A >= 8) {
            y = c[idx].x1A - 7;
        } else if (c[idx].x1A < -7) {
            y = c[idx].x1A + 7;
        } else {
            y = 0;
        }
        ang = fabs(Math_Atan2(y, x) * D_800B8C80);
        if (y < 0 && ang > D_80095828) {
            w->x3 = 1;
        }
    }
    {
        u8 r = 0;
        if (w->x3) {
            r = 1;
        } else if (btnA) {
            r = 1;
        }
        w->x0 = r;
    }
    w->x1 = btnB && !w->x3;
    w->x2 = (c[idx].xC >> 4) & 1;
    if (*(u16 *)&w->x0 || *(u16 *)&w->x2 || w->x4) {
        D_80095830 = Time_Get(gGameTime);
    }
}
