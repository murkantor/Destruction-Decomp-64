#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { f32 x, y, z; } Vec3f;
#define GFX2(g,a,b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }
typedef struct { u8 pad[4]; s16 x4; s16 x6; s16 x8; u8 padA[2]; u8 xC; u8 padD[3]; Gfx *x10; } Fog;
typedef struct { u8 pad[0x2C]; f32 x2C; f32 x30; f32 x34; } Obj12548;
typedef struct { u8 a[4]; u8 b[4]; u8 pad[0x10]; } FadeEnt;

extern Fog *D_801244D0;
extern s16 D_80083DA4;
extern s8 D_80083DA6;
extern s16 D_80083DA8;
extern s16 D_800BC990[];
extern FadeEnt D_800BC8A0[];
extern void *D_8011EEF8;

s32 rand(void);
void Sound_PlayAt(void *, f32, f32, f32, Vec3f, s32);

void func_80012548(Obj12548 *o) {
    u8 tbl[10] = { 0, 0, 0, 1, 1, 2, 3, 4, 5, 6 };
    Vec3f zero = { 0.0f, 0.0f, 0.0f };
    Fog *f = D_801244D0;
    Gfx *g;
    u8 i;
    f32 fr;

    if (f->xC == 0) {
        return;
    }
    g = f->x10;
    if (--D_80083DA4 >= 0) {
        return;
    }
    if (D_80083DA6 < 10) {
        if (D_80083DA6 == 0) {
            D_800BC990[0] = (255 - f->x4) / 6;
            D_800BC990[1] = (255 - f->x6) / 6;
            D_800BC990[2] = (255 - f->x8) / 6;
            for (i = 0; i < 10; i++) {
                D_800BC8A0[i].a[0] = ~(D_800BC990[0] * tbl[i]);
                D_800BC8A0[i].b[0] = ~(D_800BC990[0] * tbl[i]);
                D_800BC8A0[i].a[1] = ~(D_800BC990[1] * tbl[i]);
                D_800BC8A0[i].b[1] = ~(D_800BC990[1] * tbl[i]);
                D_800BC8A0[i].a[2] = ~(D_800BC990[2] * tbl[i]);
                D_800BC8A0[i].b[2] = ~(D_800BC990[2] * tbl[i]);
            }
        }
        GFX2(g, 0xFA000000, (D_800BC8A0[D_80083DA6].a[0] << 24) | (D_800BC8A0[D_80083DA6].a[1] << 16) | (D_800BC8A0[D_80083DA6].a[2] << 8));
        D_80083DA6++;
        return;
    }
    if (D_80083DA8 == 0) {
        GFX2(g, 0xFA000000, ((u8)f->x4 << 24) | ((u8)f->x6 << 16) | ((u8)f->x8 << 8));
        fr = rand();
        D_80083DA8 = fr * (30.0f / 32767.0f);
    }
    if (--D_80083DA8 <= 0) {
        Sound_PlayAt(D_8011EEF8, o->x2C, o->x30, o->x34, zero, 9);
        D_80083DA8 = 0;
        D_80083DA6 = 0;
        fr = rand();
        D_80083DA4 = fr * (300.0f / 32767.0f) + 60.0f;
    }
}
