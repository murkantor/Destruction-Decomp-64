#include "common.h"
#include "m2c.h"

typedef struct { s8 h; s8 m; s8 s; s32 frac; } TimeHMS;
typedef struct Clock {
    u64 x0;
    u64 x8;
    f32 x10;
    s32 x14;
    struct Clock *x18;
    u8 x1C;
    TimeHMS x20;
} Clock;
u64 func_80062F20(void);
s32 Fps_GetDeltaFrames(void);
u64 Clock_GetDelta(Clock *);
s32 Clock_GetFrames(Clock *);
TimeHMS Time_Split(u64 t);

void Clock_Update(Clock *c) {
    u64 dt;

    if (c->x18 == NULL) {
        dt = func_80062F20();
        c->x14 = Fps_GetDeltaFrames();
    } else {
        dt = Clock_GetDelta(c->x18);
        c->x14 = Clock_GetFrames(c->x18);
    }
    if (c->x1C) {
        dt = 0;
        c->x14 = 0;
    }
    c->x8 = dt;
    c->x0 += dt;
    c->x10 = (f32)dt * 1e-9;
    c->x20 = Time_Split(c->x0);
}
