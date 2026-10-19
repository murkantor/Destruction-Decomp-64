#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[3]; u8 x3; } Unk2C;
typedef struct {
    u8 pad0[0x2C];
    Unk2C *x2C;
    u8 pad30[0x198 - 0x30];
    s32 x198;
    s32 x19C;
    u8 pad1A0[0x1B0 - 0x1A0];
    s32 x1B0;
    f32 x1B4[6];
    f32 x1CC;
    u8 pad1D0[0xA54 - 0x1D0];
    s32 xA54;
    s32 xA58;
} Car;

void Car_SelectGear(Car *car, f32 speed) {
    s32 max = car->x198;
    s32 min = car->x19C;
    s32 gear;
    s32 rpm;
    s32 lo, hi, up, down;

    if (car->x2C->x3) {
        car->xA54 = 750;
        car->xA58 = 0;
        return;
    }
    gear = 1;
    rpm = speed * car->x1B4[gear] * car->x1CC;
    while ((rpm < (s32)(max * 0.25f) || rpm > (s32)(max * 0.85f)) && gear != 0) {
        hi = max * 0.85f;
        if (rpm > hi && gear == car->x1B0 - 1) {
            break;
        }
        lo = max * 0.25f;
        down = rpm < lo;
        if (down && gear == 1) {
            break;
        }
        if (rpm > hi) {
            gear += (gear != car->x1B0 - 1);
        }
        gear -= (down && gear != 1);
        rpm = speed * car->x1B4[gear] * car->x1CC;
    }
    if (rpm > max) {
        rpm = max;
    } else if (rpm < min) {
        rpm = min;
    }
    car->xA54 = rpm;
    car->xA58 = gear;
}
