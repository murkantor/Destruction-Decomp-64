#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { s16 x0, x2, x4, x6, x8, xA, xC, xE; } UnkVp;
typedef struct { u8 pad[4]; s32 x4; s32 x8; s32 xC; s32 x10; s32 x14; s32 x18; } UnkCam;
extern u8 D_8012455C;
extern u8 *D_8011EEF0;
extern u8 gParticlePool[];
void *Pool_Alloc(void *pool);
void func_80057334(UnkVp *);

Gfx *Gfx_SetViewport(Gfx *g, s32 a1, s32 a2) {
    UnkCam *c = (UnkCam *)(D_8011EEF0 + (D_8012455C * 28 + 0x198));
    UnkVp *v;
    Gfx *p;
    char unused[0x20];
    v = Pool_Alloc(gParticlePool);
    v->x0 = (c->x14 * 4) >> 1;
    v->x2 = (c->x18 * 4) >> 1;
    v->x4 = a1 * 4;
    v->x6 = 10;
    v->x8 = c->x4 * 4 + ((c->xC * 4) >> 1);
    v->xA = c->x8 * 4 + ((c->x10 * 4) >> 1);
    v->xC = a2 * 4;
    v->xE = 0;
    p = g++;
    p->w0 = 0xDC080008;
    p->w1 = (u32)v;
    func_80057334(v);
    return g;
}
