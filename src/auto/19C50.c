#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
extern Gfx D_80083B20[];
extern Gfx D_8008F8C0[];
extern u8 D_800958A1;
#define gfx(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->w0 = (a); _g->w1 = (u32)(b); }

Gfx *func_80019050(Gfx *g) {
    if (D_800958A1) {
        gfx(g++, 0xDE000000, D_80083B20);
    } else {
        gfx(g++, 0xDE000000, D_8008F8C0);
    }
    g[0].w0 = 0xE3001001;
    g[0].w1 = 0;
    g[1].w0 = 0xD9C0F9FB;
    g[1].w1 = 0;
    return g + 2;
}
