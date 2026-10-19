#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u8 pad[0x14]; Gfx *(*draw)(f32, Gfx *, s32, s32); } Vt5B168;
typedef struct { Vt5B168 *vt; u8 pad4[4]; u8 x8; u8 x9; u8 xA; } Obj5B168;

Gfx *func_8005B168(Obj5B168 *obj, Gfx *gfx, f32 f, s32 arg3, s32 arg4) {
    Gfx *g;

    if (obj->vt->draw == NULL) {
        return gfx;
    }
    gfx->w0 = 0xFB000000;
    gfx->w1 = (obj->xA << 24) | (obj->xA << 16) | (obj->xA << 8) | obj->x9;
    g = gfx + 1;
    if (obj->x8 & 2) {
        gfx[1].w0 = 0xE200001C;
        gfx[1].w1 = 0x50007B;
        g = gfx + 3;
        gfx[2].w0 = 0xE2001E01;
        gfx[2].w1 = 3;
    }
    return obj->vt->draw(f, g, arg3, arg4);
}
