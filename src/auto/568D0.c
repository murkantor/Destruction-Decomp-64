#include "common.h"
#include "m2c.h"

typedef struct { u32 w[20]; } ViMode;
typedef struct {
    ViMode mode;
    s32 x50;
    f32 x54;
    f32 x58;
    s32 x5C;
    s32 x60;
    s32 x64;
    s32 x68;
    s32 x6C;
    u8 x70;
    u8 x71;
    u8 pad72[2];
    s32 x74;
    s32 x78;
    u8 x7C;
    u8 x7D;
} Video;
extern ViMode osViModeTable[];
extern s32 D_800B0B08[];
extern s32 D_800B0AF0[];
extern s32 D_800B0AFC[];

void Video_SetMode(Video *v, s32 a1, s32 tv, s32 a3, s32 lo, s32 hi, s32 aa, s32 dith, s32 gamma, s32 div, f32 sx, f32 sy) {
    s32 flags;
    s32 idx;

    idx = D_800B0B08[tv] + hi * 4 + a3 * 2 + lo;
    idx += a1 * 14;
    v->mode = osViModeTable[idx];
    if (tv == 1) { v->mode.w[2] = 0x280; v->mode.w[8] = 0x400; v->mode.w[10] = 0x500; }
    flags = 0;
    flags |= (aa == 0) ? 2 : 1;
    flags |= dith ? 4 : 8;
    flags |= gamma ? 0x10 : 0x20;
    if (div) flags |= 0x40;
    v->x50 = flags;
    v->x54 = sx;
    v->x58 = sy;
    v->x70 = hi;
    v->x5C = D_800B0AF0[tv] * sx;
    v->x60 = D_800B0AFC[tv] * (sy / sx);
    v->x71 = hi ? 4 : 2;
    v->x64 = v->x5C * v->x71;
    v->x74 = v->x64 * v->x60;
    v->x78 = v->x5C * v->x60 * 2;
    v->x68 = D_800B0AF0[tv] / 320;
    v->x6C = D_800B0AFC[tv] / 240;
    v->x7C = 0;
    v->x7D = 0;
}
