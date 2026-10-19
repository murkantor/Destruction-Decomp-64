#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u8 pad[0xC]; s32 xC; s32 x10; } Img;
typedef struct {
    u8 pad0[4];
    s32 x4;
    u8 pad8[8];
    Img *x10;
    void *x14;
    f32 x18;
    f32 x1C;
    f32 x20;
    f32 x24;
    f32 x28;
    f32 x2C;
    f32 x30;
    s32 x34;
    s32 x38;
    f32 x3C;
    f32 x40;
} ElemA;
typedef struct {
    u8 pad0[4];
    s32 x4;
    u8 pad8[0x10];
    u8 x18[0x60];
    f32 x78;
    f32 x7C;
    s32 x80;
    s32 x84;
    s32 x88;
    s32 x8C;
} ElemB;
typedef struct {
    u8 pad0[4];
    s32 x4;
    u8 pad8[8];
    s32 x10;
    s32 x14;
    s16 x18;
    s16 x1A;
    s8 x1C;
} ElemC;
typedef struct Node { ElemA *x0; struct Node *x4; } Node;
typedef struct { u8 pad[0x8B54]; Node *x8B54; } Ctx;

Gfx *Gfx_SetupRenderModes(Gfx *);
void func_80062478(void *, f32, f32);
void func_80062598(void *, f32, f32);
void func_800626A8(void *, f32, f32);
void Sprite_SetRotationF(void *, f32);
void func_80062644(void *, f32, f32);
void func_800625F8(void *, s32, s32);
Gfx *func_8006102C(void *, Gfx *);
void func_80061480(void *, f32, f32, s32);
void Sprite_SetSize(void *, s32, s32, s32, s32);
Gfx *func_80061648(void *, Gfx *, s32);
Gfx *func_8005E92C(Gfx *, s32, s32, s32, s32, s32);
Gfx *func_80060320(Gfx *);

#define GFX2(g,a,b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

Gfx *func_8005E2AC(Ctx *ctx, Gfx *g) {
    Node *n;
    ElemA *e;

    if (ctx->x8B54 == NULL) {
        return g;
    }
    g = Gfx_SetupRenderModes(g);
    GFX2(g, 0xED000000, 0x5003C0);
    for (n = ctx->x8B54; n != NULL; n = n->x4) {
        e = n->x0;
        switch (e->x4) {
        case 1: {
            ElemA *a = e;
            func_80062478(a->x14, a->x18, a->x1C);
            if (a->x28 != 1.0f || a->x2C != 1.0f) {
                func_80062598(a->x14, a->x28, a->x2C);
            }
            if (a->x20 != 0.0f || a->x24 != 0.0f) {
                func_800626A8(a->x14, a->x20, a->x24);
            }
            if (a->x30 != 0.0f) {
                Sprite_SetRotationF(a->x14, a->x30);
            }
            if ((f32)a->x10->xC != a->x3C || (f32)a->x10->x10 != a->x40) {
                func_80062644(a->x14, a->x3C, a->x40);
            }
            if (a->x3C != 0.0f || a->x40 != 0.0f) {
                func_800625F8(a->x14, a->x34, a->x38);
            }
            g = func_8006102C(a->x14, g);
            break;
        }
        case 2: {
            ElemB *b = (ElemB *)e;
            if (b->x78 != 1.0 || b->x7C != 1.0) {
                func_80061480(b->x18, b->x78, b->x7C, 0);
            }
            Sprite_SetSize(b->x18, b->x80, b->x84, b->x88 * b->x78, b->x8C * b->x7C);
            g = func_80061648(b->x18, g, 0);
            break;
        }
        case 0: {
            ElemC *c = (ElemC *)e;
            GFX2(g, 0xE200001C, 0x503048);
            GFX2(g, 0xE3000A01, 0);
            GFX2(g, 0xE3001201, 0x2000);
            g = func_8005E92C(g, c->x10, c->x14, c->x18, c->x1A, c->x1C);
            break;
        }
        }
    }
    return func_80060320(g);
}
