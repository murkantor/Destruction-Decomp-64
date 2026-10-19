#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { s16 vscale[4]; s16 vtrans[4]; } Vp;
typedef struct { Vp *vp; s32 x4; s32 x8; s32 xC; s32 x10; s32 x14; s32 x18; } View;
typedef struct { u8 pad0[0x5C]; s32 x5C; s32 x60; } Screen;
typedef struct { u8 pad0[0x8C]; Screen *x8C; } Ctx;
extern u8 gParticlePool[];
void *Pool_Alloc(void *);
void func_80057334(Vp *);

Gfx *Gfx_SetViewportScissor(Gfx *gfx, View *v, Ctx *ctx) {
    s32 b[4];
    s32 unused[8];
    s32 i;

    v->vp = Pool_Alloc(gParticlePool);
    v->vp->vscale[0] = (v->x14 * 4) >> 1;
    v->vp->vscale[1] = (v->x18 * 4) >> 1;
    v->vp->vscale[2] = 0x3FF;
    v->vp->vscale[3] = 0;
    v->vp->vtrans[0] = v->x4 * 4 + ((v->xC * 4) >> 1);
    v->vp->vtrans[1] = v->x8 * 4 + ((v->x10 * 4) >> 1);
    v->vp->vtrans[2] = 0;
    v->vp->vtrans[3] = 0;
    {
        Gfx *g = gfx++;
        g->w0 = 0xDC080008;
        g->w1 = (u32)v->vp;
    }
    b[0] = v->x4;
    b[1] = v->x4 + v->xC;
    b[2] = v->x8;
    b[3] = v->x8 + v->x10;
    if (ctx != NULL) {
        for (i = 0; i < 2; i++) {
            if (b[i] < 0) {
                b[i] = 0;
            }
            if (b[i] >= ctx->x8C->x5C) {
                b[i] = ctx->x8C->x5C - 1;
            }
        }
        for (i = 0; i < 2; i++) {
            if (b[i + 2] < 0) {
                b[i + 2] = 0;
            }
            if (b[i + 2] >= ctx->x8C->x60) {
                b[i + 2] = ctx->x8C->x60 - 1;
            }
        }
    }
    {
        Gfx *g = gfx++;
        g->w0 = (0xED << 24) | (((s32)((f32)b[0] * 4.0f) & 0xFFF) << 12) | ((s32)((f32)b[2] * 4.0f) & 0xFFF);
        g->w1 = (((s32)((f32)b[1] * 4.0f) & 0xFFF) << 12) | ((s32)((f32)b[3] * 4.0f) & 0xFFF);
    }
    func_80057334(v->vp);
    return gfx;
}
