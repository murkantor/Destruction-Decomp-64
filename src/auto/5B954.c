#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u8 pad[8]; u8 x8; u8 x9; u8 xA; } Mat;
#define gfx(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->w0 = (a); _g->w1 = (u32)(b); }

Gfx *func_8005AD54(Mat *m, Gfx *g) {
    gfx(g++, 0xFB000000, (m->xA << 24) | (m->xA << 16) | (m->xA << 8) | m->x9);
    if (m->x8 & 2) {
        gfx(g++, 0xE200001C, 0x0050007B);
        gfx(g++, 0xE2001E01, 3);
    }
    return g;
}
