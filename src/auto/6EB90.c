#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[2];
    u16 x2;
    u8 pad4[2];
    u16 x6;
    u8 pad8[0xC];
    u16 x14;
    u8 x16;
    u8 x17;
    u8 pad18[4];
    u16 x1C;
    u16 x1E;
    u16 x20;
    u16 x22;
    u16 x24;
    u16 x26;
} Obj;

void func_8006DF90(Obj *o) {
    u32 base;
    u16 sh;
    u32 t;

    base = 0x200;
    if (o->x16 == 2) {
        base = 0x100;
    }
    sh = 6 - o->x17;
    if (o->x14 == 0x33) {
        o->x1C = o->x2 >> sh;
        o->x1E = base / o->x1C * 4;
        o->x22 = 0x7FF / o->x1C + 1;
        o->x24 = o->x1C * 2;
        o->x26 = o->x1E * o->x24;
        o->x20 = o->x26 / 2 - 1;
    } else {
        o->x1C = (o->x6 >> sh) + 3;
        o->x1E = base / o->x1C * 4;
        o->x24 = (o->x2 >> sh) * 2;
        t = o->x1E * o->x24;
        o->x26 = t >> 16;
        o->x20 = t;
        o->x22 = o->x1E - 1;
    }
}
