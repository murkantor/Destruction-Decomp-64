#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct {
    u16 imageX;
    u16 imageW;
    s16 frameX;
    u16 frameW;
    u16 imageY;
    u16 imageH;
    s16 frameY;
    u16 frameH;
    u32 imagePtr;
    u16 imageLoad;
    u8 imageFmt;
    u8 imageSiz;
    u16 imagePal;
    u16 imageFlip;
    u16 scaleW;
    u16 scaleH;
    s32 imageYorig;
    u8 padding[4];
    u8 tlut[0x18];
    s32 attr;
    u8 pad44[2];
    u16 x46;
    u8 pad48[2];
    u16 x4A;
    u8 pad4C[3];
    u8 alpha;
    u32 x50;
    u32 x54;
} Bg;

void osWritebackDCache(void *, s32);

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }
#define _SHIFTL(v, s, w) ((unsigned int)(((unsigned int)(v) & ((0x01 << (w)) - 1)) << (s)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define gSetImage(pkt, cmd, fmt, siz, width, i) \
{ \
    Gfx *_g = (Gfx *)(pkt); \
    _g->w0 = _SHIFTL(cmd, 24, 8) | _SHIFTL(fmt, 21, 3) | _SHIFTL(siz, 19, 2) | _SHIFTL((width)-1, 0, 12); \
    _g->w1 = (unsigned int)(i); \
}
#define gDPSetTile(pkt, fmt, siz, line, tmem, tile, palette, cmt, maskt, shiftt, cms, masks, shifts) \
{ \
    Gfx *_g = (Gfx *)(pkt); \
    _g->w0 = _SHIFTL(0xF5, 24, 8) | _SHIFTL(fmt, 21, 3) | _SHIFTL(siz, 19, 2) | _SHIFTL(line, 9, 9) | _SHIFTL(tmem, 0, 9); \
    _g->w1 = _SHIFTL(tile, 24, 3) | _SHIFTL(palette, 20, 4) | _SHIFTL(cmt, 18, 2) | _SHIFTL(maskt, 14, 4) | \
             _SHIFTL(shiftt, 10, 4) | _SHIFTL(cms, 8, 2) | _SHIFTL(masks, 4, 4) | _SHIFTL(shifts, 0, 4); \
}
#define gDPNoParam(pkt, cmd) \
{ \
    Gfx *_g = (Gfx *)(pkt); \
    _g->w0 = _SHIFTL(cmd, 24, 8); \
    _g->w1 = 0; \
}
#define gDPLoadTileGeneric(pkt, c, tile, uls, ult, lrs, lrt) \
{ \
    Gfx *_g = (Gfx *)(pkt); \
    _g->w0 = _SHIFTL(c, 24, 8) | _SHIFTL(uls, 12, 12) | _SHIFTL(ult, 0, 12); \
    _g->w1 = _SHIFTL(tile, 24, 3) | _SHIFTL(lrs, 12, 12) | _SHIFTL(lrt, 0, 12); \
}
#define gDPLoadTextureTile(pkt, timg, fmt, siz, tb, width, height, uls, ult, lrs, lrt, pal, cms, cmt, masks, maskt, shifts, shiftt) \
{ \
    gSetImage(pkt, 0xFD, fmt, siz, width, timg); \
    gDPSetTile(pkt, fmt, siz, (((((lrs) - (uls) + 1) * (tb)) + 7) >> 3), 0, 7, 0, cmt, maskt, shiftt, cms, masks, shifts); \
    gDPNoParam(pkt, 0xE6); \
    gDPLoadTileGeneric(pkt, 0xF4, 7, (uls) << 2, (ult) << 2, (lrs) << 2, (lrt) << 2); \
    gDPNoParam(pkt, 0xE7); \
    gDPSetTile(pkt, fmt, siz, (((((lrs) - (uls) + 1) * (tb)) + 7) >> 3), 0, 0, pal, cmt, maskt, shiftt, cms, masks, shifts); \
    gDPLoadTileGeneric(pkt, 0xF2, 0, (uls) << 2, (ult) << 2, (lrs) << 2, (lrt) << 2); \
}
#define gImmp1(pkt, c, p0) \
{ \
    Gfx *_g = (Gfx *)(pkt); \
    _g->w0 = _SHIFTL(c, 24, 8); \
    _g->w1 = (unsigned int)(p0); \
}
#define gSPScisTextureRectangle(pkt, xl, yl, xh, yh, tile, s, t, dsdx, dtdy) \
{ \
    Gfx *_g = (Gfx *)(pkt); \
    _g->w0 = (_SHIFTL(0xE4, 24, 8) | _SHIFTL(MAX((s16)(xh), 0), 12, 12) | _SHIFTL(MAX((s16)(yh), 0), 0, 12)); \
    _g->w1 = (_SHIFTL((tile), 24, 3) | _SHIFTL(MAX((s16)(xl), 0), 12, 12) | _SHIFTL(MAX((s16)(yl), 0), 0, 12)); \
    gImmp1(pkt, 0xE1, (_SHIFTL(((s) - (((s16)(xl) < 0) ? (((s16)(dsdx) < 0) ? (MAX((((s16)(xl) * (s16)(dsdx)) >> 7), 0)) : \
        (MIN((((s16)(xl) * (s16)(dsdx)) >> 7), 0))) : 0)), 16, 16) | \
        _SHIFTL(((t) - (((yl) < 0) ? (((s16)(dtdy) < 0) ? (MAX((((s16)(yl) * (s16)(dtdy)) >> 7), 0)) : \
        (MIN((((s16)(yl) * (s16)(dtdy)) >> 7), 0))) : 0)), 0, 16))); \
    gImmp1(pkt, 0xF1, (_SHIFTL((dsdx), 16, 16) | _SHIFTL((dtdy), 0, 16))); \
}

Gfx *Bg_Draw(Bg *bg, Gfx *g, s32 flags) {
    s32 lastW;
    s32 lastH;
    s32 th;
    s32 stepFX;
    s32 stepIX;
    s32 stepFY;
    s32 stepIY;
    s32 iy;
    s32 dsdx;
    s32 dtdy;
    s32 imageW;
    s32 frameW;
    s32 frameH;
    s32 tmpW;
    s32 tmpH;
    s32 nx;
    s32 ny;
    s32 fy;
    s32 ix;
    s32 fx;
    s32 i;
    s32 j;
    s32 uls;
    s32 ult;
    s32 lrs;
    s32 lrt;
    s32 x0;
    s32 y0;
    s32 x1;
    s32 y1;
    s32 tw;
    Bg *b;

    osWritebackDCache(bg, 0x28);
    if (!(flags & 1)) {
        if (bg->attr & 2) {
            osWritebackDCache(bg->tlut, 0x18);
            GFX2(g, 0xE3001001, 0x8000);
            GFX2(g, 0x05000017, bg->tlut);
        }
        if (bg->attr & 8) {
            GFX2(g, 0xFC119623, 0xFF2FFFFF);
#ifdef TARGET_PC
            /* alpha is the low byte of the s32 the setters store at 0x4C */
            GFX2(g, 0xFA000000, (u8)*(s32 *)((u8 *)bg + 0x4C) | 0xFFFFFF00);
#else
            GFX2(g, 0xFA000000, bg->alpha | 0xFFFFFF00);
#endif
        } else {
            GFX2(g, 0xFCFFFFFF, 0xFFFCF279);
        }
        if (!(bg->attr & 5) && bg->imageSiz != 3) {
            goto bgcopy;
        }
        if (bg->attr & 4) {
            GFX2(g, 0xE2001D00, 4);
            if (bg->imageSiz == 3) {
                GFX2(g, 0xE200001C, 0x504A50);
            } else {
                GFX2(g, 0xE200001C, 0x552230);
            }
#ifdef TARGET_PC
            /* the prim depth is the low halfword of each s32 func_80061524 stores at 0x44 and 0x48 */
            GFX2(g, 0xEE000000, ((u16)*(s32 *)((u8 *)bg + 0x44) << 16) | (u16)*(s32 *)((u8 *)bg + 0x48));
#else
            GFX2(g, 0xEE000000, (bg->x46 << 16) | bg->x4A);
#endif
        } else {
            GFX2(g, 0xE2001D00, 0);
            if (bg->imageSiz == 3) {
                GFX2(g, 0xE200001C, 0x504240);
            } else {
                GFX2(g, 0xE200001C, 0x0F0A4000);
            }
        }
        GFX2(g, 0xE3000A01, 0);
        if (!(bg->attr & 5) && bg->imageSiz != 3) {
            goto bg1cyc;
        }
        GFX2(g, 0xE3000C00, 0);
        GFX2(g, 0xE3001201, 0);
        GFX2(g, 0x0B000000, 0);
    }
    if (bg->attr & 1) {
        b = bg;
        frameW = b->frameW >> 2;
        tmpW = (frameW << 10) / b->scaleW;
        frameH = b->frameH >> 2;
        tmpH = (frameH << 10) / b->scaleH;
        dsdx = (tmpW << 10) / frameW;
        imageW = b->imageW >> 2;
        dtdy = (tmpH << 10) / frameH;
        if (tmpW > 32) {
            nx = (tmpW - 1) / 30 + 1;
            stepFX = (frameW << 8) / nx;
            stepIX = (tmpW << 8) / nx;
        } else {
            nx = 1;
            stepIX = tmpW << 8;
            stepFX = frameW << 8;
        }
        ny = 1;
        if (tmpH > 32) {
            ny = (tmpH - 1) / 30 + 1;
            stepFY = (frameH << 8) / ny;
            stepIY = (tmpH << 8) / ny;
        } else {
            stepIY = tmpH << 8;
            stepFY = frameH << 8;
        }
        iy = (b->imageY >> 5) << 8;
        fy = (b->frameY >> 2) << 8;
        for (i = 0; i < ny; i++) {
            y0 = fy >> 8;
            fy += stepFY;
            y1 = fy >> 8;
            ult = iy >> 8;
            iy += stepIY;
            ult -= (i != 0);
            lrt = iy >> 8;
            lrt -= (i == ny - 1);
            ix = (b->imageX >> 5) << 8;
            fx = (b->frameX >> 2) << 8;
            for (j = 0; j < nx; j++) {
                x0 = fx >> 8;
                fx += stepFX;
                uls = ix >> 8;
                x1 = fx >> 8;
                ix += stepIX;
                lrs = ix >> 8;
                uls -= (j != 0);
                if (i == nx - 1) {
                    lrs--;
                }
                if (b->imageSiz == 3) {
                    gDPLoadTextureTile(g++, b->imagePtr, 0, 3, 2, imageW, 0, uls, ult, lrs, lrt, 0, 0, 0, 0, 0, 0, 0);
                } else if (b->imageSiz == 2) {
                    gDPLoadTextureTile(g++, b->imagePtr, 0, 2, 2, imageW, 0, uls, ult, lrs, lrt, 0, 0, 0, 0, 0, 0, 0);
                }
                gSPScisTextureRectangle(g++, x0 << 2, y0 << 2, x1 << 2, y1 << 2, 0, uls << 5, ult << 5, dsdx, dtdy);
            }
        }
        goto end;
    }
    b = bg;
    th = 32;
    imageW = b->imageW >> 2;
    frameW = b->frameW >> 2;
    frameH = b->frameH >> 2;
    nx = ((frameW - 1) >> 5) + 1;
    ny = ((frameH - 1) >> 5) + 1;
    lastW = ((frameW - 1) & 0x1F) + 1;
    lastH = ((frameH - 1) & 0x1F) + 1;
    ult = b->imageY >> 5;
    y0 = b->frameY >> 2;
    for (i = 0; i < ny; i++) {
        uls = b->imageX >> 5;
        x0 = b->frameX >> 2;
        tw = 32;
        if (i == ny - 1) {
            th = lastH;
        }
        lrt = ult + th - 1;
        y1 = y0 + th;
        for (j = 0; j < nx; j++) {
            if (j == nx - 1) {
                tw = lastW;
            }
            lrs = uls + tw - 1;
            x1 = x0 + tw;
            if (b->imageSiz == 3) {
                gDPLoadTextureTile(g++, b->imagePtr, 0, 3, 2, imageW, 0, uls, ult, lrs, lrt, 0, 0, 0, 0, 0, 0, 0);
            } else if (b->imageSiz == 2) {
                gDPLoadTextureTile(g++, b->imagePtr, 0, 2, 2, imageW, 0, uls, ult, lrs, lrt, 0, 0, 0, 0, 0, 0, 0);
            }
            gSPScisTextureRectangle(g++, x0 << 2, y0 << 2, x1 << 2, y1 << 2, 0, uls << 5, ult << 5, 1 << 10, 1 << 10);
            uls += 32;
            x0 += 32;
        }
        ult += 32;
        y0 += 32;
    }
    goto end;
bg1cyc:
    GFX2(g, 0xE2001D00, 0);
    GFX2(g, 0xE3001201, bg->x50);
    GFX2(g, 0x0B000000, bg->x54);
    GFX2(g, 0x09000000, bg);
    goto end;
bgcopy:
    GFX2(g, 0xE200001C, 0);
    GFX2(g, 0xE3000A01, 0x200000);
    GFX2(g, 0x0A000000, bg);
end:
    GFX2(g, 0xE7000000, 0);
    return g;
}
