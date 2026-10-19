#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u8 pad[0x18]; u32 x18; } Obj54DD0;
extern char D_800BBB6C[];
extern char D_800BBB70[];
void __assert(const char *, const char *, s32);

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (b); }

Gfx *Gfx_LoadTlut(Obj54DD0 *obj, Gfx *gfx) {
    if (obj == NULL) {
        __assert(D_800BBB6C, D_800BBB70, 0x5E);
    }
    if (obj->x18 != 0) {
        GFX2(gfx, 0xE7000000, 0);
        GFX2(gfx, 0xE3001001, 0x8000);
        GFX2(gfx, 0xFD100000, obj->x18);
        GFX2(gfx, 0xE8000000, 0);
        GFX2(gfx, 0xF5000100, 0x07000000);
        GFX2(gfx, 0xE6000000, 0);
        GFX2(gfx, 0xF0000000, 0x0703C000);
        GFX2(gfx, 0xE7000000, 0);
    }
    return gfx;
}
