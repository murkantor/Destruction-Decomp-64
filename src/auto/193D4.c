#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { f32 x, y, z; } Vec3f;
typedef struct { s16 x0; s16 x2; s16 x4; s16 x6; u8 pad8[4]; u8 xC; u8 xD; u8 xE; u8 xF; } Vtx;
typedef struct { u8 x0; u8 pad1[3]; Vec3f p[3]; s32 x28; } Tri2C;
typedef struct { Gfx *x0; Vtx *x4; u8 pad8[0x10]; Gfx *x18; Vtx *x1C; u8 pad20[0xE8]; } DLBuf;
typedef struct {
    u8 pad0[8];
    s32 x8;
    DLBuf bufs[2];
    u8 pad21C[0x224 - 0x21C];
    Tri2C *x224;
    u8 pad228[4];
    f32 x22C;
    f32 x230;
    f32 x234;
} Ctx187D4;

extern s32 D_800BC9CC;
extern Gfx *D_800BC9D0;
extern s8 D_800D31CC[];

Gfx *func_80019050(Gfx *);

#define GFX2(g,a,b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

void Skid_Draw(Ctx187D4 *c) {
    DLBuf *b;
    Gfx *g;
    s32 i;
    u8 j;

    c->x8 = (c->x8 + 1) % 2;
    b = &c->bufs[c->x8];
    b->x18 = b->x0;
    b->x1C = b->x4;
    D_800BC9D0 = 0;
    D_800BC9CC = 0;
    b->x18 = func_80019050(b->x18);
    GFX2(b->x18, 0xD9FFF9FF, 0);
    GFX2(b->x18, 0xD9FFFFFF, 0x200005);
    GFX2(b->x18, 0xFCFFFFFF, 0xFFFE793C);
    if (D_800D31CC[0] != 3) {
        GFX2(b->x18, 0xE200001C, 0x5049D8);
    } else {
        GFX2(b->x18, 0xE200001C, 0x504A50);
    }
    g = b->x18;
    D_800BC9CC = 0;
    D_800BC9D0 = 0;
    for (i = 0; i < 40; i++) {
        if (c->x224[i].x0 == 0) {
            continue;
        }
        if (D_800BC9CC + 3 > 32) {
            D_800BC9D0->w0 = 0x01000000 | ((D_800BC9CC & 0xFF) << 12) | ((D_800BC9CC & 0x7F) << 1);
            D_800BC9D0->w1 = (u32)b->x1C;
            b->x1C += D_800BC9CC;
            D_800BC9CC = 0;
        }
        if (D_800BC9CC == 0) {
            D_800BC9D0 = g;
            GFX2(g, 0x01020040, b->x1C);
        }
        for (j = 0; j < 3; j++) {
            Vtx *v = &b->x1C[D_800BC9CC];
            v->x0 = (c->x224[i].p[j].x - c->x22C) * 32.0f;
            v->x2 = (c->x224[i].p[j].y - c->x230) * 32.0f;
            v->x4 = (c->x224[i].p[j].z - c->x234) * 32.0f;
            v->x6 = 0;
            v->xC = 0;
            v->xD = 0;
            v->xE = 0;
            v->xF = (c->x224[i].x28 << 6) / 20;
            D_800BC9CC++;
        }
        D_800BC9CC -= 3;
        GFX2(g, 0x05000000 | (((((D_800BC9CC) * 2) & 0xFF) << 16) | ((((D_800BC9CC + 1) * 2) & 0xFF) << 8) | (((D_800BC9CC + 2) * 2) & 0xFF)), 0);
        D_800BC9CC += 3;
    }
    b->x18 = g;
    if (D_800BC9D0 != 0) {
        D_800BC9D0->w0 = 0x01000000 | ((D_800BC9CC & 0xFF) << 12) | ((D_800BC9CC & 0x7F) << 1);
        D_800BC9D0->w1 = (u32)b->x1C;
    }
    GFX2(b->x18, 0xDF000000, 0);
}
