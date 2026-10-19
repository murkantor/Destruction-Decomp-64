#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { s32 a; s32 b; } HeapBlock;

typedef struct {
    u8 pad0[0x5C];
    s32 x5C;
    s32 x60;
    u8 pad64[0x70 - 0x64];
    u8 x70;
    u8 pad71[0x7C - 0x71];
    u8 x7C;
} FrameCfg;

typedef struct {
    u8 pad0[0x88];
    u32 x88;
    FrameCfg *x8C;
    void *x90;
    void *x94;
    void *x98;
    Gfx *x9C;
    void *xA0;
    void *xA4;
    u32 xA8;
} Renderer;

extern s32 D_800B0B5C;
extern HeapBlock D_800BCF98[];
extern u32 D_800B0B20[];
extern u8 D_8007BCC0[];
extern u8 D_800B7560[];
void *HeapBlock_Ptr(HeapBlock *);
Gfx *Gfx_InitRenderState(void *, FrameCfg *);
Gfx *func_8005793C(Gfx *);
u32 osVirtualToPhysical(void *);

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

void func_800565B8(Renderer *r) {
    Gfx *g;

    D_800B0B5C ^= 1;
    g = Gfx_InitRenderState(HeapBlock_Ptr(&D_800BCF98[D_800B0B5C]), r->x8C);
    GFX2(g, 0xED000000, ((D_800B0B20[r->x8C->x7C] & 3) << 24) | (((s32)(r->x8C->x5C * 4.0f) & 0xFFF) << 12) | ((s32)(r->x8C->x60 * 4.0f) & 0xFFF));
    if (r->x94 != NULL) {
        if (r->x88 & 2) {
            GFX2(g, 0xE7000000, 0);
            GFX2(g, 0xE3000A01, 0x300000);
            GFX2(g, 0xFF100000 | ((r->x8C->x5C - 1) & 0xFFF), osVirtualToPhysical(r->x94));
            GFX2(g, 0xF7000000, 0xFFFCFFFC);
            GFX2(g, 0xF6000000 | (((r->x8C->x5C - 1) & 0x3FF) << 14) | (((r->x8C->x60 - 1) & 0x3FF) << 2), 0);
        }
        GFX2(g, 0xFE000000, osVirtualToPhysical(r->x94));
    }
    GFX2(g, (r->x8C->x70 ? 0xFF180000 : 0xFF100000) | ((r->x8C->x5C - 1) & 0xFFF), osVirtualToPhysical(r->x90));
    if (r->x88 & 1) {
        GFX2(g, 0xE7000000, 0);
        GFX2(g, 0xE3000A01, 0x300000);
        GFX2(g, 0xF7000000, r->xA8);
        GFX2(g, 0xF6000000 | (((r->x8C->x5C - 1) & 0x3FF) << 14) | (((r->x8C->x60 - 1) & 0x3FF) << 2), 0);
        GFX2(g, 0xE7000000, 0);
        GFX2(g, 0xE3000A01, 0);
    }
    if (r->x98 != NULL) {
        GFX2(g, 0xDE010000, r->x98);
        r->x98 = HeapBlock_Ptr(&D_800BCF98[D_800B0B5C]);
    } else {
        g = func_8005793C(g);
        r->x98 = HeapBlock_Ptr(&D_800BCF98[D_800B0B5C]);
        r->x9C = g;
        r->xA0 = D_8007BCC0;
        r->xA4 = D_800B7560;
    }
}
