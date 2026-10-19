#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (b); }

Gfx *Gfx_InitRenderState(Gfx *gfx) {
    GFX2(gfx, 0xDB060000, 0);
    GFX2(gfx, 0xD9C0F9FB, 0);
    GFX2(gfx, 0xD7000000, 0);
    GFX2(gfx, 0xD9FFFFFF, 0x200004);
    GFX2(gfx, 0xE3000A01, 0);
    GFX2(gfx, 0xE3000800, 0x800000);
    GFX2(gfx, 0xED000000, 0x5003C0);
    GFX2(gfx, 0xE3000F00, 0);
    GFX2(gfx, 0xE3001001, 0);
    GFX2(gfx, 0xE3000D01, 0);
    GFX2(gfx, 0xE3000C00, 0x80000);
    GFX2(gfx, 0xE3001201, 0x2000);
    GFX2(gfx, 0xE3001402, 0xC00);
    GFX2(gfx, 0xFCFFFFFF, 0xFFFE793C);
    GFX2(gfx, 0xE3001700, 0);
    GFX2(gfx, 0xE2001E01, 0);
    GFX2(gfx, 0xE3001801, 0xC0);
    GFX2(gfx, 0xFCFFFFFF, 0xFFFE793C);
    GFX2(gfx, 0xFA000000, 0xFFFF00FF);
    GFX2(gfx, 0xFB000000, 0xFF8080FF);
    GFX2(gfx, 0xF8000000, 0x8080FFFF);
    GFX2(gfx, 0xF9000000, 0x80FF8000);
    GFX2(gfx, 0xE7000000, 0);
    return gfx;
}
