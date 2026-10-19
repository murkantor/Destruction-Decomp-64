#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

Gfx *Gfx_SetupRenderMode(Gfx *gfx) {
    GFX2(gfx, 0xE7000000, 0);
    GFX2(gfx, 0xE3000C00, 0x80000);
    GFX2(gfx, 0xE3000F00, 0);
    GFX2(gfx, 0xE3001201, 0x2000);
    GFX2(gfx, 0xE3001402, 0xC00);
    GFX2(gfx, 0xFCFFFFFF, 0xFFFCF279);
    GFX2(gfx, 0xE200001C, 0x552078);
    GFX2(gfx, 0xE3000A01, 0);
    GFX2(gfx, 0xD7000002, 0x80008000);
    GFX2(gfx, 0xD9FFFFFF, 0x401);
    return gfx;
}
