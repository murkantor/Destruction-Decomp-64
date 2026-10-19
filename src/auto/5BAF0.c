#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u8 pad[0xC]; s32 xC; } Sub;
typedef struct { Sub *x0; u32 x4; u8 x8; u8 x9; u8 xA; } Obj;
void Particle_BuildMatrix(Obj *);
Gfx *Gfx_CopyDisplayList(Gfx *, s32, s32, s32, s32);

#define GFX2(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->w0 = (a); _g->w1 = (b); }

Gfx *Particle_Draw(Obj *o, Gfx *g, s32 a2, s32 a3) {
    Gfx *p;

    if (o->x0->xC == 0) {
        return g;
    }
    if (o->x8 & 1) {
        Particle_BuildMatrix(o);
        o->x8 &= ~1;
    }
    GFX2(g++, 0xDA380000, o->x4);
    g[0].w0 = 0xFB000000;
    g[0].w1 = (o->xA << 24) | (o->xA << 16) | (o->xA << 8) | o->x9;
    p = g + 1;
    if (o->x8 & 2) {
        g[1].w0 = 0xE200001C;
        g[1].w1 = 0x0050007B;
        p = g + 3;
        g[2].w0 = 0xE2001E01;
        g[2].w1 = 3;
    }
    g = Gfx_CopyDisplayList(p, o->x0->xC, 10, a2, a3);
    GFX2(g++, 0xD8380002, 0x40);
    return g;
}
