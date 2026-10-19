#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0xC4];
    f32 xC4;
    u8 padC8[0x120 - 0xC8];
    f32 x120;
} Body;

typedef struct {
    f32 x0;
    u8 pad4[0x1BC - 0x4];
} Wheel;

typedef struct {
    u8 pad0[0x2C];
    u8 *x2C;
    u8 pad30[0x198 - 0x30];
    s32 x198;
    u8 pad19C[0x1D0 - 0x19C];
    s32 x1D0;
    f32 x1D4[(0x364 - 0x1D4) / 4];
    Body x364;
    u8 pad[0xBA4 - 0x364 - sizeof(Body)];
    s32 xBA4;
    f32 xBA8;
} Car;

void Car_UpdateEngineRpm(Car *c) {
    s32 n;
    Body *b = &c->x364;
    f32 *tbl = c->x1D4;
    s32 i;
    s32 cnt;
    f32 speed;
    f32 rpm;
    f32 lo;
    f32 base;
    f32 hi;
    f32 x;

    n = c->x1D0 - 1;
    if (*c->x2C != 0) {
        cnt = 0;
        for (i = 0; i < 4; i++) {
            if (((Wheel *)((u8 *)c + 0x500))[i].x0 >= 1.0) {
                cnt++;
            }
        }
        if (cnt >= 2) {
            rpm = c->xBA8 + 200.0f;
            c->xBA4 = n;
            c->xBA8 = rpm;
            if (c->x198 < rpm) {
                c->xBA8 = c->x198;
            }
            return;
        }
    }
    speed = -b->xC4 * b->x120 * 2.235;
    if (speed < 0.0) {
        i = 0;
        base = 1000.0f;
        lo = 0.0f;
        x = -speed;
        hi = -tbl[0];
    } else {
        for (i = 1; i < n; i++) {
            if (speed < tbl[i]) {
                break;
            }
        }
        if (i == 1) {
            base = 1000.0f;
            lo = 0.0f;
        } else {
            base = (f32)(i - 2) * 400.0 + 2500.0;
            lo = tbl[i - 1];
        }
        hi = tbl[i];
        x = speed;
    }
    rpm = base + (x - lo) / (hi - lo) * ((f32)(c->x198 * 0.7) - base);
    if (rpm > c->x198 * 0.8) {
        rpm = c->x198 * 0.8;
    }
    c->xBA4 = i;
    c->xBA8 = rpm;
}
