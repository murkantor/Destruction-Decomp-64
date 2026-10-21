#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { f32 x, y, z; } Vec3f;
typedef struct { s32 m[4][4]; } Mtx;
typedef struct { s32 x0; s32 x4; } HeapBlock;
typedef struct Particle {
    struct Particle *next;
    Vec3f pos;
    Vec3f vel;
} Particle;
typedef struct {
    f32 scale;
    f32 size;
    Vec3f vel;
    s8 count;
    u8 pad[3];
} WeatherType;
typedef struct {
    Particle *head;
    u8 pad4[0x28];
    Gfx *bufs[2];
    HeapBlock blk[2];
} Weather;
typedef struct {
    Mtx *mtx;
    u8 pad4[0x28];
    Vec3f x2C;
    u8 pad38[0x10];
    Vec3f x48;
    u8 pad54[0x14];
} Cam;
typedef struct { u8 pad[0x1074]; u8 x1074; u8 pad1075[0x10FC - 0x1075]; s16 x10FC; } Game;

extern Game *gGame;
extern u8 D_8008F820;
extern u8 D_80131F08;
extern WeatherType D_8008F824[2];
extern Weather D_800D1AA0;
extern s8 D_800D1AE4;
extern s8 D_800D1AE5;
extern f32 D_8012435C;
extern Vec3f D_80124588;
extern Vec3f D_801242AC;
extern Cam D_800D1BB0;
extern Mtx D_800D2E80[4][2];
extern int rand(void);
extern void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
extern void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
extern void Vec3f_Scale(Vec3f *, Vec3f *, f32);
extern void Vec3f_Normalize(Vec3f *, Vec3f *);
extern void Particles_Update(Weather *);
extern void guMtxL2F(f32 mf[4][4], Mtx *m);
extern Gfx *Particles_Draw(Weather *, f32 (*)[4], Gfx *);
extern void guPosition(Mtx *, f32, f32, f32, f32, f32, f32, f32);

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

Gfx *Weather_Draw(Cam *cam, Gfx *gfx) {
    Vec3f d;
    Vec3f dir;
    f32 m[4][4];
    Vec3f offs[4] = { { -7.0f, 1.0f, -7.0f }, { -7.0f, 1.0f, 7.0f }, { 7.0f, 1.0f, -7.0f }, { 7.0f, 1.0f, 7.0f } };
    Weather *w;
    f32 step;
    s32 type;
    s32 frame;
    Particle *p;
    Gfx *g;
    s32 i;
    f32 yoff;

    if (D_8008F820) {
        return gfx;
    }
    if (gGame->x10FC == 0x1C && D_80131F08) {
        return gfx;
    }
    step = D_8012435C * 70.0 * 2.0;
    type = D_800D1AE5;
    D_800D1AE4 = (D_800D1AE4 + 1) % 2;
    frame = D_800D1AE4;
    w = &D_800D1AA0;
    if (gGame->x1074 == 0) {
        Vec3f_Sub(&d, &cam->x48, &D_800D1BB0.x48);
        Vec3f_Scale(&d, &d, D_8012435C * 10.0);
        for (p = w->head; p != NULL; p = p->next) {
            Vec3f_Sub(&p->pos, &p->pos, &d);
            if (p->pos.y < D_801242AC.y) {
                p->pos.x = D_801242AC.x + rand() * (step * (1.0f / 32767));
                p->pos.y = p->pos.y + step;
                p->pos.z = D_801242AC.z + rand() * (step * (1.0f / 32767));
            } else {
                if (D_80124588.y < p->pos.y) {
                    p->pos.y = p->pos.y - step;
                }
                if (p->pos.x < D_801242AC.x) {
                    p->pos.x = p->pos.x + step;
                } else if (D_80124588.x < p->pos.x) {
                    p->pos.x = p->pos.x - step;
                }
                if (p->pos.z < D_801242AC.z) {
                    p->pos.z += step;
                }
                if (p->pos.z > D_80124588.z) {
                    p->pos.z -= step;
                }
            }
            if (type == 1 && rand() * (100.0f / 32767) > 85.0) {
                p->vel.x = (rand() * (1.0f / 32767) - 0.5) * 4.0;
                p->vel.z = (rand() * (1.0f / 32767) - 0.5) * 4.0;
            }
        }
    }
    D_800D1BB0 = *cam;
    if (gGame->x1074 == 0) {
        Particles_Update(w);
    }
    guMtxL2F(m, cam->mtx);
    g = Particles_Draw(w, m, w->bufs[frame]);
    g->w0 = 0xDF000000;
    g->w1 = 0;
    Vec3f_Sub(&dir, &cam->x48, &cam->x2C);
    Vec3f_Normalize(&dir, &dir);
    Vec3f_Scale(&dir, &dir, D_8008F824[type].scale);
    Vec3f_Add(&dir, &cam->x2C, &dir);
    step = 1.0 / (D_8012435C * 12.0);
    yoff = 0.0f;
    if (type == 0) {
        yoff = 1.0f;
    }
    for (i = 0; i < 4; i++) {
        guPosition(&D_800D2E80[i][frame], 0.0f, 0.0f, 0.0f, step, dir.x + offs[i].x * 0.8333333333333333,
                   dir.y + (offs[i].y + yoff) * 0.8333333333333333, dir.z + offs[i].z * 0.8333333333333333);
        GFX2(gfx, 0xDA380000, PHYS_ADDR(&D_800D2E80[i][frame]));
        GFX2(gfx, 0xDE000000, w->bufs[frame]);
        GFX2(gfx, 0xD8380002, 0x40);
    }
    return gfx;
}
