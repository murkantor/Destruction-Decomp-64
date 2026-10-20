#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u8 pad[0xC]; u32 xC; u8 pad2[0x108 - 0x10]; } E108;
typedef struct { u32 dl; u8 pad[0x104]; } Lod;
typedef struct { u8 pad[0x8]; s32 x8; Lod lods[2]; u8 pad2[0x10]; f32 x22C, x230, x234; u8 pad3[0x74]; Gfx *x2AC; } Obj;
extern u8 gParticlePool[];
extern void *Pool_Alloc(void *);
extern void guPosition(void *, f32, f32, f32, f32, f32, f32, f32);
extern void osWritebackDCache(void *, s32);
extern void Gfx_FreeAfterFrame(void *);
void Obj_EmitPositionedDL(Obj *o) {
    void *m = Pool_Alloc(gParticlePool);
    guPosition(m, 0.0f, 0.0f, 0.0f, 0.03125f, o->x22C, o->x230, o->x234);
    osWritebackDCache(m, 0x40);
    { Gfx *g = &o->x2AC[0]; g->w0 = 0xDA380000; g->w1 = PHYS_ADDR(m); }
    { Gfx *g = &o->x2AC[1]; g->w0 = 0xDE000000; g->w1 = o->lods[o->x8].dl; }
    { Gfx *g = &o->x2AC[2]; g->w0 = 0xD8380002; g->w1 = 0x40; }
    Gfx_FreeAfterFrame(m);
}
