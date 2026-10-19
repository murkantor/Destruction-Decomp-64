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
} Obj609D8;

f32 cosf(f32);
f32 sinf(f32);

void Sprite_SetRotationF(Obj609D8 *obj, f32 angle) {
    s32 c;
    s32 s;
    Mtx2 *m;

    c = cosf(angle) * 65536.0f;
    s = sinf(angle) * 65536.0f;
    m = obj->x18;
    {
        s32 sx = m->x14;
        s32 sy = m->x16;
        s32 a = (c << 10) / sx;
        s32 b = (-s << 10) / sy;
        s32 d = (c << 10) / sy;
        s32 e = (s << 10) / sx;
        m->x0 = a;
        m->xC = d;
        m->x4 = b;
        m->x8 = e;
    }
}
