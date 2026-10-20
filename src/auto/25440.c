#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[0xC]; u32 xC; s32 x10; u8 pad14[4]; s16 x18; s16 x1A; u8 pad1C[0x94]; } Ctrl;
typedef struct { u8 pad0[0x10]; s8 x10; u8 pad11[0x37]; u8 x48[8]; } Foo;
extern u8 gGameTime[];
extern u64 D_80095830;
u64 Time_Get(void *);
Ctrl *Pad_GetLatched(void);

void func_80024840(Foo *arg0) {
    Ctrl *c;
    u8 *out;
    s8 i;
    u8 ok;
    s8 idx;
    s16 v;
    s32 b;
    u8 r;

    c = Pad_GetLatched();
    out = arg0->x48;
    for (i = 0; i < 4; i++) {
        ok = (c[i].xC & 0x2000) == 0;
        if (!ok) {
            break;
        }
    }
    idx = arg0->x10;
    b = c[idx].x10 & 0x1000; out[0] = b && ok;
    b = c[idx].x10 & 0x8000; out[2] = b && ok;
    b = c[idx].x10 & 0x4000; out[1] = b && ok;
    if (c[idx].x18 >= 8) {
        v = c[idx].x18 - 7;
    } else if (c[idx].x18 < -7) {
        v = c[idx].x18 + 7;
    } else {
        v = 0;
    }
    out[6] = (v >= 0x38 || (c[idx].xC & 0x100)) && ok;
    out[5] = (v < -0x37 || (c[idx].xC & 0x200)) && ok;
    if (c[idx].x1A >= 8) {
        v = c[idx].x1A - 7;
    } else if (c[idx].x1A < -7) {
        v = c[idx].x1A + 7;
    } else {
        v = 0;
    }
    out[3] = (v >= 0x38 || (c[idx].xC & 0x800)) && ok;
    out[4] = (v < -0x37 || (c[idx].xC & 0x400)) && ok;
    b = c[idx].x10 & 0x20;
    out[7] = b && ok;
    if (out[1] || out[2] || out[0] || out[6] || out[5] || out[3] || out[4] || out[7]) {
        D_80095830 = Time_Get(gGameTime);
    }
}
