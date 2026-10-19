#include "common.h"
#include "m2c.h"

typedef struct { u32 top : 8; u32 u : 12; u32 v : 12; } TexWord;
typedef struct {
    TexWord *x0;
    u8 pad4[8];
    s32 xC;
    s32 x10;
    u8 pad14[4];
    u8 x18;
    u8 pad19[3];
} Scroll;
typedef struct {
    u32 *x0;
    u32 x4[5];
    s8 x18;
    u8 pad19[0xB];
    u8 x24;
    u8 pad25[3];
} Anim;
extern Scroll D_801255F0[];
extern Anim D_80124B20[];
extern s32 D_80131380;
extern s32 D_80131F28;

void Tex_UpdateScrollAndAnim(void) {
    s8 i;
    s32 u, v;
    TexWord *p;

    for (i = 0; i < D_80131380; i++) {
        if (D_801255F0[i].x18) {
            p = D_801255F0[i].x0;
            u = p->u + D_801255F0[i].xC;
            if (u >= 0x1000) u -= 0xFFF;
            else if (u < 0) u += 0xFFF;
            v = p->v + D_801255F0[i].x10;
            if (v >= 0x1000) v -= 0xFFF;
            else if (v < 0) v += 0xFFF;
            *(u32 *)p = (*(u32 *)p & 0xFF000FFF) | ((u & 0xFFF) << 12);
            *(u32 *)p = (*(u32 *)p & ~0xFFF) | (v & 0xFFF);
            D_801255F0[i].x18 = 0;
        }
    }
    for (i = 0; i < D_80131F28; i++) {
        if (D_80124B20[i].x24) {
            *D_80124B20[i].x0 = D_80124B20[i].x4[D_80124B20[i].x18];
            D_80124B20[i].x24 = 0;
        }
    }
}
