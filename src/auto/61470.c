#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    s32 xC;
    u8 pad10[4];
    u16 x14;
    u16 x16;
} Mtx2;

typedef struct {
    u8 pad[0x18];
    Mtx2 *x18;
} Obj60870;

s16 coss(u16);
s16 sins(u16);

void Sprite_SetRotation(Obj60870 *obj, u16 angle) {
    s32 c;
    s32 s;
    Mtx2 *m;

    c = coss(angle) * 2;
    s = sins(angle) * 2;
    if (c == 0xFFFE) {
        c = 0x10000;
    }
    if (c == -0xFFFE) {
        c = -0x10000;
    }
    if (s == 0xFFFE) {
        s = 0x10000;
    }
    if (s == -0xFFFE) {
        s = -0x10000;
    }
    m = obj->x18;
    {
        s32 sx = m->x14;
        s32 sy = m->x16;
        s32 a = (c << 10) / sx;
        s32 b = (-s << 10) / sy;
        s32 d = (s << 10) / sx;
        s32 e = (c << 10) / sy;
        m->x0 = a;
        m->x4 = b;
        m->x8 = d;
        m->xC = e;
    }
}
