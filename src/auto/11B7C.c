#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct {
    u8 pad0[0x14];
    u8 *img;
    s32 fmt;
    s32 siz;
    s32 width;
    s32 height;
} Tex;

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define TXL2WORDS(txls, b) MAX(1, ((txls) * (b) / 8))
#define CALC_DXT(width, b) (((1 << 11) + TXL2WORDS(width, b) - 1) / TXL2WORDS(width, b))

Gfx *Particle_LoadTexFrame(Tex *t, s32 frame, Gfx *g) {
    switch (t->siz) {
        case 2:
            GFX2(g, 0xFD100000 | ((t->fmt & 7) << 21), t->img + t->width * t->height * frame * 2);
            GFX2(g, 0xF5100000 | ((t->fmt & 7) << 21), 0x07083E0F);
            GFX2(g, 0xE6000000, 0);
            GFX2(g, 0xF3000000, 0x07000000 | ((MIN(t->width * t->height - 1, 0x7FF) & 0xFFF) << 12) | (CALC_DXT(t->width, 2) & 0xFFF));
            GFX2(g, 0xE7000000, 0);
            GFX2(g, 0xF5100000 | ((t->fmt & 7) << 21) | (((((t->width * 2) + 7) >> 3) & 0x1FF) << 9), 0x00083E0F);
            GFX2(g, 0xF2000000, ((((t->width - 1) << 2) & 0xFFF) << 12) | (((t->height - 1) << 2) & 0xFFF));
            break;
        case 3:
            GFX2(g, 0xFD180000 | ((t->fmt & 7) << 21), t->img + t->width * t->height * frame * 4);
            GFX2(g, 0xF5180000 | ((t->fmt & 7) << 21), 0x07083E0F);
            GFX2(g, 0xE6000000, 0);
            GFX2(g, 0xF3000000, 0x07000000 | ((MIN(t->width * t->height - 1, 0x7FF) & 0xFFF) << 12) | (CALC_DXT(t->width, 4) & 0xFFF));
            GFX2(g, 0xE7000000, 0);
            GFX2(g, 0xF5180000 | ((t->fmt & 7) << 21) | (((((t->width * 2) + 7) >> 3) & 0x1FF) << 9), 0x00083E0F);
            GFX2(g, 0xF2000000, ((((t->width - 1) << 2) & 0xFFF) << 12) | (((t->height - 1) << 2) & 0xFFF));
            break;
    }
    return g;
}
