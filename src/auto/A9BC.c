#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u32 x0; u32 x4; u32 x8; } TexAddrs;
extern u8 D_8008F8C0[];
extern u8 D_800958A1[];
extern TexAddrs *D_8012EE24;
Gfx *Model_Draw(void *, Gfx *);

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

Gfx *Car_DrawWheels(u8 *obj, Gfx *gfx) {
    GFX2(gfx, 0xDE000000, D_8008F8C0);
    GFX2(gfx, 0xD9FFF9FF, 0);
    if (D_800958A1[0]) {
        GFX2(gfx, 0xD9FFFFFF, 0x201);
    } else {
        GFX2(gfx, 0xD9FFFFFF, 0x401);
    }
    GFX2(gfx, 0xFA000000, 0xFFFF00FF);
    GFX2(gfx, 0xD7000002, 0x80008000);
    GFX2(gfx, 0xE3001001, 0x8000);
    GFX2(gfx, 0xFD100000, D_8012EE24->x8);
    GFX2(gfx, 0xE8000000, 0);
    GFX2(gfx, 0xF5000100, 0x07000000);
    GFX2(gfx, 0xE6000000, 0);
    GFX2(gfx, 0xF0000000, 0x073FC000);
    GFX2(gfx, 0xE7000000, 0);
    GFX2(gfx, 0xFD100000, D_8012EE24->x0);
    GFX2(gfx, 0xF5100000, 0x07017C5F);
    GFX2(gfx, 0xE6000000, 0);
    GFX2(gfx, 0xF3000000, 0x071FF200);
    GFX2(gfx, 0xE7000000, 0);
    GFX2(gfx, 0xF5080800, 0x00017C5F);
    GFX2(gfx, 0xF2000000, 0x0007C07C);
    gfx = Model_Draw(obj + 0xC0, gfx);
    gfx = Model_Draw(obj + 0xE8, gfx);
    gfx = Model_Draw(obj + 0x110, gfx);
    return Model_Draw(obj + 0x138, gfx);
}
