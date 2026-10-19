#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

typedef struct {
    u8 pad[0x1C];
    f32 x1C;
    u8 pad2[0x38 - 0x20];
    Vec3f eye;
    u8 pad3[0x54 - 0x44];
    Vec3f target;
    u8 pad4[0x68 - 0x60];
    Vec3f up;
} Camera;

typedef struct {
    u8 pad[0x290];
    Vec3f pos;
    u8 pad2[0x2B4 - 0x29C];
    f32 m[9];
} Car;

typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    s32 xC;
    Vec3f eyeOfs;
    Vec3f tgtOfs;
    f32 x28;
} CamCfg;

typedef struct {
    s32 active;
    s32 idx;
    s32 step;
    f32 timer;
} CpState;
extern CpState D_80130200;

void Math_SinCos(f32, f32 *, f32 *);
void Vec3f_Set(Vec3f *, f32, f32, f32);
void Camera_UpdateBasis(Camera *);

void func_80021C20(Camera *cam, Car *car, CamCfg *cfg) {
    Vec3f *pos = &car->pos;
    f32 *m = car->m;
    Vec3f *eye = &cam->eye;
    Vec3f *tgt = &cam->target;
    f32 s, c;

    if (cfg->x0 == 1) {
        cam->target.x = car->pos.x;
        cam->target.y = car->pos.y;
        cam->target.z = car->pos.z;
        cam->eye.y = car->pos.y + 3.0;
        Math_SinCos(D_80130200.timer * 0.3, &s, &c);
        cam->eye.x = car->pos.x + c * 7.0;
        cam->eye.z = car->pos.z + s * 7.0;
    } else {
        switch (cfg->x4) {
            case 0:
                cam->eye.x = cfg->eyeOfs.x;
                cam->eye.y = cfg->eyeOfs.y;
                cam->eye.z = cfg->eyeOfs.z;
                break;
            case 1:
                cam->eye.x = car->pos.x + cfg->eyeOfs.x * car->m[0] + cfg->eyeOfs.y * car->m[3] + cfg->eyeOfs.z * car->m[6];
                cam->eye.y = car->pos.y + cfg->eyeOfs.x * car->m[1] + cfg->eyeOfs.y * car->m[4] + cfg->eyeOfs.z * car->m[7];
                cam->eye.z = car->pos.z + cfg->eyeOfs.x * car->m[2] + cfg->eyeOfs.y * car->m[5] + cfg->eyeOfs.z * car->m[8];
                break;
            case 2:
                cam->eye.x = car->pos.x + cfg->eyeOfs.x;
                cam->eye.y = car->pos.y + cfg->eyeOfs.y;
                cam->eye.z = car->pos.z + cfg->eyeOfs.z;
                break;
        }
        switch (cfg->x8) {
            case 0:
                tgt->x = eye->x + cfg->tgtOfs.x;
                tgt->y = eye->y + cfg->tgtOfs.y;
                tgt->z = eye->z + cfg->tgtOfs.z;
                break;
            case 1:
                tgt->x = pos->x + cfg->tgtOfs.x * m[0] + cfg->tgtOfs.y * m[3] + cfg->tgtOfs.z * m[6];
                tgt->y = pos->y + cfg->tgtOfs.x * m[1] + cfg->tgtOfs.y * m[4] + cfg->tgtOfs.z * m[7];
                tgt->z = pos->z + cfg->tgtOfs.x * m[2] + cfg->tgtOfs.y * m[5] + cfg->tgtOfs.z * m[8];
                break;
            case 2:
                tgt->x = pos->x + cfg->tgtOfs.x;
                tgt->y = pos->y + cfg->tgtOfs.y;
                tgt->z = pos->z + cfg->tgtOfs.z;
                break;
        }
        if (eye->x == tgt->x && eye->y == tgt->y && eye->z == tgt->z) {
            tgt->y = tgt->y - 1.0f;
            tgt->z = tgt->z + 0.1f;
        }
        if (eye->x == tgt->x && eye->z == tgt->z) {
            tgt->z = tgt->z + 0.1f;
        }
    }
    cam->x1C = cfg->x28;
    Vec3f_Set(&cam->up, 0.0f, 1.0f, 0.0f);
    Camera_UpdateBasis(cam);
}
