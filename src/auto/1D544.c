#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { s32 x0; s32 x4; } ResPair;
typedef struct { u8 x0; u8 pad1[3]; s32 x4; } ResSlot;
typedef struct {
    u8 pad0[0xC];
    u8 xC;
    u8 padD[3];
    Gfx *x10;
    u8 pad14[2];
    u8 x16;
    u8 pad17;
    ResPair *x18;
    ResSlot *x1C;
    Gfx *x20;
    s32 x24;
} ResHdr;
typedef struct {
    s32 a[5];
    s32 b[5];
    s8 n;
    u8 pad29;
    s16 x2A;
    u8 pad2C[4];
} ResTbl;
extern u8 D_80091B28;
extern ResTbl D_80091B30[];
void func_80013A54(s32 a0, ResTbl *a1, s8 a2, s8 a3, s16 a4);

void func_8001C944(ResHdr *hdr, s32 base, u8 flag) {
    u8 used[256];
    s32 i;
    s32 k;
    s32 t;
    s32 j;
    u8 found;
    s32 n;
    Gfx *gfx;
    Gfx *prev;
    u8 firstFA;
    ResSlot *slots;
    s32 cont;
    u32 w;
    ResPair *tbl;

    firstFA = 1;
    slots = NULL;
    cont = 1;
    prev = NULL;
    n = hdr->x16;
    for (i = 0; i < n; i++) {
        used[i] = 1;
    }
    if (hdr->x1C != NULL) {
        slots = hdr->x1C;
        for (i = 11; i >= 0; i--) {
            slots[i].x4 = 0;
        }
    }
    for (gfx = hdr->x20; cont && gfx != NULL; gfx++) {
        switch (gfx->w0 >> 24) {
        case 0xFD:
            w = gfx->w1;
            tbl = hdr->x18;
            for (k = 0; k < n; k++) {
                if (used[k] && w == tbl[k].x4) {
                    gfx->w1 = tbl[k].x0;
                    break;
                }
            }
            if (D_80091B28 == 0) {
                found = 1;
                for (t = 0; found && t < 7; t++) {
                    for (j = 0; found && j < D_80091B30[t].n; j++) {
                        if (gfx->w1 == D_80091B30[t].a[j]) {
                            func_80013A54((s32)&gfx->w1, &D_80091B30[t], j, D_80091B30[t].n, D_80091B30[t].x2A);
                            found = 0;
                        }
                    }
                }
            }
            if (hdr->x1C != NULL) {
                for (i = 0; i < 12; i++) {
                    if (k == slots[i].x0) {
                        used[k] = 0;
                        if (i >= 2) {
                            slots[i].x4 = (s32)gfx;
                        } else {
                            slots[i].x4 = (s32)prev;
                        }
                        break;
                    }
                }
            }
            prev = gfx;
            break;
        case 0x01:
            gfx->w1 = gfx->w1 - base + hdr->x24;
            break;
        case 0xFA:
            if (hdr->xC && firstFA) {
                hdr->x10 = gfx;
                firstFA = 0;
            }
            break;
        case 0xD9:
            if (flag) {
s32 b;
u32 w2 = gfx->w1;
b = !((w2 >> 10) & 1);
if ((w2 & 0x200) ? b : (b ^ 1) & 1) {
                    gfx->w1 ^= 0x600;
                }
            }
            break;
        case 0xDF:
            cont = 0;
            break;
        }
    }
}
