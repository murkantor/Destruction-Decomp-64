#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct {
    u32 x0;
    u8 *widths;
    u8 height;
    u8 fmt;
    u8 siz;
    u8 padB;
    u8 *tex;
    u16 perRow;
    u8 pad12[2];
    u8 numChars;
    u8 pad15[3];
    u8 *map;
    u8 pad1C;
    u8 alpha;
} Font;

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

Gfx *func_8005E92C(Gfx *gfx, Font *f, u8 *str, s16 x, s16 y, s16 maxPerLine) {
    s16 n;
    s16 cx;
    s16 cy;
    s32 i;
    s32 j;
    s32 sum;
    s32 row;
    s32 col;
    u8 *addr;
    u8 *p;

    n = 0;
    if (f->x0 & 1) {
        GFX2(gfx, 0xFC119623, 0xFF2FFFFF);
        GFX2(gfx, 0xFA000000, f->alpha | ~0xFF);
    } else {
        GFX2(gfx, 0xFCFFFFFF, 0xFFFCF279);
    }
    p = str;
    cx = x;
    cy = y;
    for (; *p != 0; p++) {
        for (i = 0; i < f->numChars; i++) {
            if (*p == f->map[i]) {
                break;
            }
        }
        if (i >= f->numChars) {
            i = 0x31;
        }
        sum = 0;
        for (j = 0; j < i; j++) {
            sum += f->widths[j];
        }
        if (f->perRow == 0) {
            f->perRow = 1;
        }
        row = sum / f->perRow;
        col = sum % f->perRow;
        if (f->siz == 2) {
            addr = f->tex + (f->perRow * row * f->height + col) * 2;
        } else {
            addr = f->tex + (f->perRow * row * f->height + col) * 4;
        }
        if (f->siz == 2) {
            GFX2(gfx, 0xFD100000 | ((f->fmt & 7) << 21) | ((f->perRow - 1) & 0xFFF), addr);
            GFX2(gfx, 0xF5101200 | ((f->fmt & 7) << 21), 0x07080200);
            GFX2(gfx, 0xE6000000, 0);
            GFX2(gfx, 0xF4000000, 0x07080080);
            GFX2(gfx, 0xE7000000, 0);
            GFX2(gfx, 0xF5101200 | ((f->fmt & 7) << 21), 0x00080200);
            GFX2(gfx, 0xF2000000, 0x00080080);
        } else if (f->siz == 3) {
            GFX2(gfx, 0xFD180000 | ((f->fmt & 7) << 21) | ((f->perRow - 1) & 0xFFF), addr);
            GFX2(gfx, 0xF5181200 | ((f->fmt & 7) << 21), 0x07080200);
            GFX2(gfx, 0xE6000000, 0);
            GFX2(gfx, 0xF4000000, 0x07080080);
            GFX2(gfx, 0xE7000000, 0);
            GFX2(gfx, 0xF5181200 | ((f->fmt & 7) << 21), 0x00080200);
            GFX2(gfx, 0xF2000000, 0x00080080);
        }
        GFX2(gfx, 0xE4000000 | ((((cx + f->widths[i]) * 4) & 0xFFF) << 12) | (((cy + f->height) * 4) & 0xFFF),
             (((cx * 4) & 0xFFF) << 12) | ((cy * 4) & 0xFFF));
        GFX2(gfx, 0xE1000000, 0);
        GFX2(gfx, 0xF1000000, 0x04000400);
        n++;
        if (n >= maxPerLine && maxPerLine > 0) {
            n = 0;
            cx = x;
            cy += f->height;
        } else {
            cx += f->widths[i];
        }
    }
    return gfx;
}
