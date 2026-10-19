#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[8];
    u32 x8;
    u8 padC[0x30];
    u32 x3C;
    u8 pad40[0x10];
    u32 x50;
    f32 x54;
    f32 x58;
    f32 x5C;
    u32 x60;
    u8 pad64[0x53];
    u8 xB7;
    u8 xB8;
    u8 xB9;
    u8 xBA;
    u8 xBB;
    u8 xBC;
    u8 xBD;
    u8 xBE;
    u8 xBF;
    u8 xC0;
    u8 padC1[4];
    u8 xC5;
} Env;

void Env_Update(Env *e) {
    s32 t;

    if (e->x8 >= e->x50 && e->xBB < 4) {
        e->xBB = 4;
        e->xBD = 1;
        e->xC5 = e->xBC;
    }
    if (--e->xBD != 0) {
        return;
    }
    e->xBD = e->xB7;
    switch (e->xBB) {
        case 1:
            t = (((e->x8 - e->x3C) >> 8) * e->x60) >> 10;
            if (t < e->xBE) {
                e->xBC = e->xB8 + (s32)(e->x54 * t);
            } else {
                e->xBB++;
                e->xBC = e->xB9;
            }
            break;
        case 2:
            t = ((((e->x8 - e->x3C) >> 8) - e->xBE) * e->x60) >> 10;
            if (t < e->xBF) {
                e->xBC = e->xB9 + (s32)(e->x58 * t);
            } else {
                e->xBB++;
                e->xBC = e->xBA;
            }
            break;
        case 3:
            break;
        case 4:
            t = (((e->x8 - e->x50) >> 8) * e->x60) >> 10;
            if (t < e->xC0) {
                e->xBC = e->xC5 - (s32)(e->x5C * t * e->xC5);
            } else {
                e->xBB++;
                e->xBC = 0;
            }
            break;
    }
}
