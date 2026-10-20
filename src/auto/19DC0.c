#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { s16 vscale[4]; s16 vtrans[4]; } Vp;
typedef struct { u8 pad[4]; s32 x4; s32 x8; s32 xC; s32 x10; s32 x14; s32 x18; } View;
typedef struct { u8 pad[0x198]; View views[1]; } Scene;
extern Scene *D_8011EEF0;
extern u8 D_8012455C;
extern u8 gParticlePool[];
void *Pool_Alloc(void *);
void Gfx_FreeAfterFrame(Vp *);

Gfx *Gfx_SetViewport2(Gfx *g, s32 z) {
    View *v = &D_8011EEF0->views[D_8012455C];
    Vp *vp = Pool_Alloc(gParticlePool);
    u8 unused[25];

    vp->vscale[0] = (v->x14 * 4) >> 1;
    vp->vscale[1] = (v->x18 * 4) >> 1;
    vp->vscale[2] = 0x3FF;
    vp->vscale[3] = 1;
    vp->vtrans[0] = v->x4 * 4 + ((v->xC * 4) >> 1);
    vp->vtrans[1] = v->x8 * 4 + ((v->x10 * 4) >> 1);
    vp->vtrans[2] = z * 4;
    vp->vtrans[3] = 0;
    { Gfx *_g = g++; _g->w0 = 0xDC080008; _g->w1 = (u32)vp; }
    Gfx_FreeAfterFrame(vp);
    return g;
}
