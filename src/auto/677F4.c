#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x38];
    s32 x38;
    s32 x3C;
    u8 pad40[0x10];
    s32 x50;
    u8 pad54[0x3E];
    u16 x92;
    u8 pad94[0x16];
    u16 xAA;
    u16 xAC;
    u8 padAE[9];
    u8 xB7;
    u8 xB8;
    u8 padB9[2];
    u8 xBB;
    u8 xBC;
    u8 xBD;
} Voice;

void func_80066BF4(Voice *v) {
    if (v->x92 != 0x7FFF) {
        if (v->xAA != 0) {
            v->x50 = v->x3C + (v->xAA << 8);
        } else {
            v->x50 = v->x38 - (v->xAC << 8);
        }
    } else {
        v->x50 = 0x7FFFFFFF;
    }
    v->xBB = 1;
    v->xBC = v->xB8;
    v->xBD = v->xB7;
}
