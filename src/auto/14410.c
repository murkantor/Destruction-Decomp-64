#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    f32 x4;
    u8 x8;
    u8 x9;
    u8 padA[2];
    s32 xC;
    s32 x10;
    s32 x14;
    u8 x18;
    u8 pad19[3];
} UnkEntry1C;
extern s32 D_80131380;
extern UnkEntry1C D_801255F0[];

void Tex_AddScroll(s32 a, s16 b, u8 c, u8 d) {
    D_801255F0[D_80131380].x0 = a;
    D_801255F0[D_80131380].x4 = b * 0.001;
    D_801255F0[D_80131380].x8 = c;
    D_801255F0[D_80131380].x9 = d;
    D_801255F0[D_80131380].xC = 0;
    D_801255F0[D_80131380].x10 = 0;
    D_801255F0[D_80131380].x14 = 0;
    D_801255F0[D_80131380].x18 = 0;
    if (D_801255F0[D_80131380].x4 < 0.001) {
        D_801255F0[D_80131380].x4 = 0.001f;
    }
    D_80131380++;
}
