#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

typedef struct {
    u32 buttons;
    u8 pad[8];
    s16 stickX;
    s16 stickY;
    u8 pad2[0xB0 - 0x10];
} Controller;

typedef struct {
    u8 pad[0x10];
    s8 x10;
} CamOwner;

typedef struct {
    u8 pad[0x38];
    Vec3f pos;
    u8 pad2[0x54 - 0x44];
    Vec3f target;
    u8 pad3[0x74 - 0x60];
    Vec3f m[3];
    u8 pad4[0xB0 - 0x98];
    CamOwner *xB0;
} Camera;

extern Controller D_80125C8C[];
extern Vec3f D_800B8C10;

void Vec3f_Scale(Vec3f *out, Vec3f *v, f32 s);
void Vec3f_Add(Vec3f *out, Vec3f *a, Vec3f *b);
void Mtx2x3_RotateY(Vec3f *m, f32 angle);
void func_8003A568(Vec3f *m, Vec3f *axis, f32 angle);

void Camera_UpdateFreeFly(Camera *cam, f32 dt) {
    s32 sx = D_80125C8C[cam->xB0->x10].stickX;
    u32 btn = D_80125C8C[cam->xB0->x10].buttons;
    s32 sy = D_80125C8C[cam->xB0->x10].stickY;
    Vec3f up = D_800B8C10;
    Vec3f tmp;
    Vec3f *m = cam->m;
    f32 a;

    if (btn & 0x8000) {
        Vec3f_Scale(&tmp, &m[0], dt * 5.0f);
        cam->pos.x += tmp.x;
        cam->pos.y += tmp.y;
        cam->pos.z += tmp.z;
        cam->target.x += tmp.x;
        cam->target.y += tmp.y;
        cam->target.z += tmp.z;
    } else if (btn & 0x4000) {
        Vec3f_Scale(&tmp, &m[0], dt * -5.0f);
        cam->pos.x += tmp.x;
        cam->pos.y += tmp.y;
        cam->pos.z += tmp.z;
        cam->target.x += tmp.x;
        cam->target.y += tmp.y;
        cam->target.z += tmp.z;
    }
    if (btn & 0x8) {
        Vec3f_Scale(&tmp, &up, dt * 5.0f);
        cam->pos.x += tmp.x;
        cam->pos.y += tmp.y;
        cam->pos.z += tmp.z;
        cam->target.x += tmp.x;
        cam->target.y += tmp.y;
        cam->target.z += tmp.z;
    } else if (btn & 0x4) {
        Vec3f_Scale(&tmp, &up, dt * -5.0f);
        cam->pos.x += tmp.x;
        cam->pos.y += tmp.y;
        cam->pos.z += tmp.z;
        cam->target.x += tmp.x;
        cam->target.y += tmp.y;
        cam->target.z += tmp.z;
    }
    if (btn & 0x1) {
        Vec3f_Scale(&tmp, &m[2], dt * 5.0f);
        cam->pos.x += tmp.x;
        cam->pos.y += tmp.y;
        cam->pos.z += tmp.z;
        cam->target.x += tmp.x;
        cam->target.y += tmp.y;
        cam->target.z += tmp.z;
    } else if (btn & 0x2) {
        Vec3f_Scale(&tmp, &m[2], dt * -5.0f);
        cam->pos.x += tmp.x;
        cam->pos.y += tmp.y;
        cam->pos.z += tmp.z;
        cam->target.x += tmp.x;
        cam->target.y += tmp.y;
        cam->target.z += tmp.z;
    }
    if (m[0].y < 0.9 && (((btn >> 10) & 1) | (sy < -9))) {
        Mtx2x3_RotateY(m, dt * 0.7853982f);
        Vec3f_Add(&cam->target, &cam->pos, m);
    }
    if (m[0].y > -0.9 && (((btn >> 11) & 1) | (sy > 9))) {
        Mtx2x3_RotateY(m, dt * -0.7853982f);
        Vec3f_Add(&cam->target, &cam->pos, m);
    }
    if (((btn >> 9) & 1) | (sx < -9)) {
        func_8003A568(m, &up, dt * 0.7853982f);
        Vec3f_Add(&cam->target, &cam->pos, m);
    } else if (((btn >> 8) & 1) | (sx > 9)) {
        func_8003A568(m, &up, dt * -0.7853982f);
        Vec3f_Add(&cam->target, &cam->pos, m);
    }
}
