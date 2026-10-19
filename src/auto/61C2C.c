#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u8 pad[0x15]; u8 x15; } Sub14;
typedef struct { u8 pad[4]; s32 x4; } Sub20;
typedef struct {
    s32 x0;
    s32 x4;
    u8 pad8[2];
    u16 xA;
    u8 padC[2];
    u16 xE;
    u8 pad10[3];
    u8 x13;
    Sub14 *x14;
    void *x18;
    void *x1C;
    Sub20 *x20;
} Obj6102C;

void osWritebackDCache(void *, s32);

#define GFX2(g,a,b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

Gfx *func_8006102C(Obj6102C *obj, Gfx *g) {
    s32 mode;

    osWritebackDCache(obj->x14, 0x18);
    osWritebackDCache(obj->x1C, 0x18);
    osWritebackDCache(obj->x18, 0x18);
    if (obj->x4 & 8) {
        GFX2(g, 0xFC119623, 0xFF2FFFFF);
        GFX2(g, 0xFA000000, obj->x13 | 0xFFFFFF00);
    } else {
        GFX2(g, 0xFCFFFFFF, 0xFFFCF279);
    }
    if (obj->x20->x4 != 0) {
        osWritebackDCache(obj->x20, 0x18);
        GFX2(g, 0xE3001001, 0x8000);
        GFX2(g, 0x05000017, obj->x20);
    } else {
        GFX2(g, 0xE3001001, 0);
    }
    if (obj->x4 & 4) {
        GFX2(g, 0xE2001D00, 4);
        if (obj->x14->x15 == 3) {
            GFX2(g, 0xE200001C, 0x504A50);
        } else {
            GFX2(g, 0xE200001C, 0x552230);
        }
        GFX2(g, 0xEE000000, (obj->xA << 16) | obj->xE);
    } else {
        if (obj->x14->x15 == 3) {
            GFX2(g, 0xE200001C, 0x504240);
        } else {
            GFX2(g, 0xE200001C, 0x0F0A4000);
        }
        GFX2(g, 0xE2001D00, 0);
    }
    GFX2(g, 0x05000017, obj->x1C);
    GFX2(g, 0xE3000A01, 0);
    GFX2(g, 0xE3001201, 0x2000);
    GFX2(g, 0x0B000000, 8);
    mode = obj->x0 & 3;
    if (mode == 3) {
        GFX2(g, 0xDC170000, obj->x18);
        GFX2(g, 0x02000000, obj->x14);
    } else if (mode == 1) {
        GFX2(g, 0xDC170000, obj->x18);
        GFX2(g, 0xDA000000, obj->x14);
    } else {
        GFX2(g, 0x01000000, obj->x14);
    }
    GFX2(g, 0xE7000000, 0);
    return g;
}
