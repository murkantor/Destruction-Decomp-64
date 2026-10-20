#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u32 *dl; u8 pad[0x104]; } Lod15630;
typedef struct {
    s32 x0;
    Lod15630 x4[2];
    f32 x214;
    f32 x218;
    f32 x21C;
    u8 pad220[0xA4];
    Gfx *x2C4;
} Obj15630;
extern u8 gParticlePool[];
void *Pool_Alloc(void *);
void guPosition(void *, f32, f32, f32, f32, f32, f32, f32);
void osWritebackDCache(void *, s32);
void Gfx_FreeAfterFrame(void *);

void Shadow_Draw(Obj15630 *obj) {
    void *m = Pool_Alloc(gParticlePool);

    guPosition(m, 0.0f, 0.0f, 0.0f, 0.0078125f, obj->x214, obj->x218, obj->x21C);
    osWritebackDCache(m, 0x40);
    {
        Gfx *g = &obj->x2C4[0];
        g->w0 = 0xDA380000;
        g->w1 = PHYS_ADDR(m);
    }
    {
        Gfx *g = &obj->x2C4[1];
        g->w0 = 0xDE000000;
        g->w1 = (u32)obj->x4[obj->x0].dl;
    }
    {
        Gfx *g = &obj->x2C4[2];
        g->w0 = 0xD8380002;
        g->w1 = 0x40;
    }
    Gfx_FreeAfterFrame(m);
}
