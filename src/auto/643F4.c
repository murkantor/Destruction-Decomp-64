#include "common.h"
#include "m2c.h"

typedef struct { s8 h; s8 m; s8 s; s32 frac; } TimeHMS;
typedef struct {
    u64 x0;
    u64 x8;
    f32 x10;
    s32 x14;
    u8 pad18[8];
    TimeHMS x20;
} Clock;
f32 func_80062F40(void);
TimeHMS Time_Split(u64 t);

void Clock_Step(Clock *c, s32 n) {
    f32 f;

    c->x14 = n;
    f = n * func_80062F40();
    c->x10 = f;
    c->x8 = f * 1e-9;
    c->x0 += c->x8;
    c->x20 = Time_Split(c->x0);
}
