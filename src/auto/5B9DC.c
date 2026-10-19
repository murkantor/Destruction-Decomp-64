#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u8 pad[0xC]; u32 xC; } Model5ADDC;
typedef struct { Model5ADDC *x0; u32 x4; u8 x8; u8 x9; u8 xA; } Obj5ADDC;
void func_8005ABC4(Obj5ADDC *);

Gfx *Model_Draw(Obj5ADDC *o, Gfx *gfx) {
    Gfx *g;
    Gfx *p;

    if (o->x0->xC == 0) {
        return gfx;
    }
    if (o->x8 & 1) {
        func_8005ABC4(o);
        o->x8 &= ~1;
    }
    g = gfx++;
    g->w0 = 0xDA380000;
    g->w1 = o->x4;
    gfx[0].w0 = 0xFB000000;
    gfx[0].w1 = (o->xA << 24) | (o->xA << 16) | (o->xA << 8) | o->x9;
    p = gfx + 1;
    if (o->x8 & 2) {
        gfx[1].w0 = 0xE200001C;
        gfx[1].w1 = 0x0050007B;
        p = gfx + 3;
        gfx[2].w0 = 0xE2001E01;
        gfx[2].w1 = 3;
    }
    p[0].w0 = 0xDE000000;
    p[0].w1 = o->x0->xC;
    p[1].w0 = 0xD8380002;
    p[1].w1 = 0x40;
    return p + 2;
}
