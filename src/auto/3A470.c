#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x2C];
    u8 *x2C;
    u8 pad30[0x168];
    s32 x198;
    u8 pad19C[0xA0C];
    f32 xBA8;
    u8 padBAC[0x30];
    s32 xBDC;
} Car39870;

void Car_UpdateRpm(Car39870 *car, f32 dt) {
    if (*car->x2C) {
        car->xBA8 = car->xBA8 + dt * 0.83333333333333337 * (car->x198 - 1000.0);
        car->xBDC = 1;
    } else {
        car->xBA8 = car->xBA8 - dt * 0.83333333333333337 * (car->x198 - 1000.0);
    }
    if (car->xBA8 < 1000.0) {
        car->xBA8 = 1000.0f;
    } else if ((f32)car->x198 < car->xBA8) {
        car->xBA8 = car->x198;
    }
}
