#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { f32 x, y, z; } Vec3f;
typedef f32 MtxF[4][4];
typedef struct {
    void *x0;
    void *x4;
    f32 zNear;
    f32 zFar;
    f32 fovy;
    f32 aspect;
    f32 scale;
    s32 x1C;
    f32 x20[3];
    Vec3f eye;
    u8 pad38[0x48 - 0x38];
    Vec3f at;
    u8 pad54[0x5C - 0x54];
    Vec3f up;
} Cam;
extern u8 gParticlePool[];
extern void *Pool_Alloc(void *);
extern void guPerspectiveF(MtxF, u16 *, f32, f32, f32, f32, f32);
extern void guLookAtF(MtxF, f32, f32, f32, f32, f32, f32, f32, f32, f32);
extern void guMtxIdentF(MtxF);
extern void guMtxF2L(MtxF, void *);
extern void guMtxCatF(void *, MtxF, MtxF);
extern void osWritebackDCache(void *, s32);
extern void func_80057334(void *);
extern double fabs(double);

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

Gfx *Camera_BuildProjection(Gfx *gfx, Cam *cam) {
    MtxF proj;
    MtxF ident;
    u16 norm;

    guPerspectiveF(proj, &norm, cam->fovy, cam->aspect, cam->zNear, cam->zFar, cam->scale);
    switch (cam->x1C) {
    case 1:
        guMtxCatF(cam->x20, proj, proj);
        break;
    case 2: {
        MtxF look;

        if (fabs(cam->eye.x - cam->at.x) < 0.01) {
            cam->eye.x = cam->at.x + 0.01;
        }
        if (fabs(cam->eye.y - cam->at.y) < 0.01) {
            cam->eye.y = cam->at.y + 0.01;
        }
        if (fabs(cam->eye.z - cam->at.z) < 0.01) {
            cam->eye.z = cam->at.z + 0.01;
        }
        guLookAtF(look, cam->eye.x, cam->eye.y, cam->eye.z, cam->at.x, cam->at.y, cam->at.z, cam->up.x, cam->up.y, cam->up.z);
        guMtxCatF(look, proj, proj);
        break;
    }
    }
    guMtxIdentF(ident);
    cam->x0 = Pool_Alloc(gParticlePool);
    cam->x4 = Pool_Alloc(gParticlePool);
    guMtxF2L(proj, cam->x0);
    osWritebackDCache(cam->x0, 0x40);
    guMtxF2L(ident, cam->x4);
    osWritebackDCache(cam->x4, 0x40);
    GFX2(gfx, 0xDA380007, (u32)cam->x0 - 0x80000000);
    GFX2(gfx, 0xDA380003, (u32)cam->x4 - 0x80000000);
    GFX2(gfx, 0xDB0E0000, norm);
    GFX2(gfx, 0xDB040004, 2);
    GFX2(gfx, 0xDB04000C, 2);
    GFX2(gfx, 0xDB040014, 0xFFFE);
    GFX2(gfx, 0xDB04001C, 0xFFFE);
    func_80057334(cam->x0);
    func_80057334(cam->x4);
    return gfx;
}
