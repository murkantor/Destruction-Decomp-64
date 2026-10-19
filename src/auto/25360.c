#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[4]; s16 x4; u8 x6; } Wheel;
typedef struct { u8 pad[0x10]; s8 x10; u8 pad11[3]; s32 x14; } Car;
extern u8 D_800958A1;
s32 func_80062F14(void);
void func_80024480(s32, s32, Wheel *);

void func_80024760(Wheel *w, Car *c) {
    s32 idx;
    s32 v;

    if (w->x6) {
        return;
    }
    idx = c->x10;
    v = func_80062F14();
    if (c->x14) {
        return;
    }
    func_80024480(v, idx, w);
    if (D_800958A1) {
        w->x4 = -(f64)w->x4;
    }
    w->x6 = 1;
}
