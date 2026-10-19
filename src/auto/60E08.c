#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
extern u8 D_801301F8[];
extern u8 D_B7980[];
extern u8 D_7D050[];

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

Gfx *Gfx_SetupRenderModes(Gfx *gfx) {
    GFX2(gfx, 0xE7000000, 0);
    if (D_801301F8[0]) {
        Gfx *_g = gfx++;
        _g->w0 = 0xE1000000;
        _g->w1 = (u32)D_B7980;
        _g = gfx++;
        _g->w0 = 0xDD0007FF;
        _g->w1 = (u32)D_7D050;
    }
    GFX2(gfx, 0xE3000C00, 0);
    GFX2(gfx, 0xE3000F00, 0);
    GFX2(gfx, 0xE3001001, 0);
    GFX2(gfx, 0xE3001402, 0xC00);
    GFX2(gfx, 0xE2001E01, 1);
    GFX2(gfx, 0xF9000000, 1);
    GFX2(gfx, 0xFCFFFFFF, 0xFFFCF279);
    return gfx;
}
