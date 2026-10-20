#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u8 pad[0x14]; Gfx *(*x14)(f32, Gfx *, s32, s32); } ModelVt;
typedef struct { ModelVt *x0; u32 x4; u8 x8; u8 x9; u8 xA; } Model;
void ModelInst_BuildMatrix(Model *);

Gfx *func_8005B02C(Model *m, Gfx *gfx, f32 f, s32 arg3) {
    Gfx *next;

    if (m->x0->x14 == NULL) {
        return gfx;
    }
    if (m->x8 & 1) {
        ModelInst_BuildMatrix(m);
        m->x8 &= ~1;
    }
    { Gfx *_g = gfx++; _g->w0 = 0xDA380000; _g->w1 = m->x4; }
    gfx[0].w0 = 0xFB000000;
    gfx[0].w1 = (m->xA << 24) | (m->xA << 16) | (m->xA << 8) | m->x9;
    next = gfx + 1;
    if (m->x8 & 2) {
        gfx[1].w0 = 0xE200001C;
        gfx[1].w1 = 0x50007B;
        next = gfx + 3;
        gfx[2].w0 = 0xE2001E01;
        gfx[2].w1 = 3;
    }
    gfx = m->x0->x14(f, next, arg3, -1);
    { Gfx *_g = gfx++; _g->w0 = 0xD8380002; _g->w1 = 0x40; }
    return gfx;
}
