#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { f32 x, y, z; } Vec3f;

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

typedef struct {
    u8 pad0[0xC];
    f32 xC;
    u8 pad10[0x2C - 0x10];
    Vec3f eye;
    u8 pad38[0x48 - 0x38];
    Vec3f at;
} Camera;

typedef struct {
    u8 pad0[0xC];
    s16 xC;
} Game;

typedef struct {
    u8 x0;
    u8 x1;
} Opts;

typedef struct {
    u8 pad0[0x20];
    u32 x20;
} Model;

extern Game *gGame;
extern Opts *D_80125B68;
extern s32 gDrawSkyDome;
extern s32 gDrawWorld;
extern Model *D_801244D0;
extern u8 D_1304D8[];
extern u8 D_80125460[];
extern s32 D_80130248;
extern u8 D_80083B78[];
void func_80012548(Camera *);
Gfx *Camera_BuildProjection(Gfx *, Camera *);
Gfx *func_800196BC(void *, Gfx *);
void Tex_UpdateScrollAndAnim(void);
Gfx *func_80011FF4(Gfx *, Camera *);

Gfx *Render_DrawSkyAndWorld(s32 arg0, Gfx *gfx, Camera *cam) {
    Vec3f eye;
    Vec3f at;
    s32 far;
    Gfx *g;

    if (gGame->xC == 2) {
        if (D_80125B68->x1) {
            func_80012548(cam);
        }
        far = cam->xC;
        eye = cam->eye;
        at = cam->at;
        cam->at.x -= cam->eye.x;
        cam->at.z -= cam->eye.z;
        cam->eye.x = 0.0f;
        cam->eye.z = 0.0f;
        GFX2(gfx, 0xE2001D00, 4);
        GFX2(gfx, 0xEE000000, 0x7FFE0000);
        cam->xC = 8192.0f;
        gfx = Camera_BuildProjection(gfx, cam);
        GFX2(gfx, 0xDA380000, D_1304D8);
        if (gDrawSkyDome) {
            GFX2(gfx, 0xDE000000, D_801244D0->x20);
        }
        cam->xC = far;
        cam->eye = eye;
        cam->at = at;
        g = Camera_BuildProjection(gfx, cam);
        gfx = g + 2;
        g[0].w0 = 0xDA380000;
        g[0].w1 = (u32)D_1304D8;
        g[1].w0 = 0xE2001D00;
        g[1].w1 = 0;
        if (gDrawWorld) {
            gfx = func_800196BC(D_80125460, gfx);
        }
        Tex_UpdateScrollAndAnim();
        GFX2(gfx, 0xD8380002, 0x40);
        if (D_80083B78[D_80130248]) {
            gfx = func_80011FF4(gfx, cam);
        }
    }
    return gfx;
}
