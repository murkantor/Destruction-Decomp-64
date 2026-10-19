#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad0[0x1A]; u8 x1A; u8 x1B; u8 x1C; } CamCfg;
typedef struct {
    u8 pad0[0x38];
    Vec3f x38;
    u8 pad44[0x10];
    Vec3f x54;
    u8 pad60[8];
    Vec3f x68;
    u8 pad74[0x24];
    Vec3f x98;
    u8 padA4[0xC];
    CamCfg *xB0;
} Cam;
typedef struct {
    Vec3f pos;
    Vec3f vel;
    Vec3f angvel;
    Vec3f m[3];
} Phys;
typedef struct {
    u8 pad0[0x290];
    Phys ph;
} Obj;

extern u8 D_800958A1;
f32 sqrtf(f32);
f32 fabsf(f32);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
void Camera_UpdateBasis(Cam *);

static inline void approach(Vec3f *cur, Vec3f *goal, f32 speed2, f32 m)
{
    Vec3f d;
    f32 dist2;
    f32 t;

    Vec3f_Sub(&d, cur, goal);
    dist2 = d.x * d.x + d.y * d.y + d.z * d.z;
    if (16.0 < dist2)
        dist2 = 16.0f;
    if (dist2 < 0.001) {
        *cur = *goal;
    } else {
        t = (dist2 < speed2) ? 1.0f : sqrtf(fabsf(speed2 / dist2));
        if (t < 0.1)
            t = 0.1f;
        if (m < t)
            t = m;
        cur->x = (1.0f - t) * cur->x + t * goal->x;
        cur->y = (1.0f - t) * cur->y + t * goal->y;
        cur->z = (1.0f - t) * cur->z + t * goal->z;
    }
}

void Camera_UpdateChase2(Cam *cam, Obj *o, f32 a2, f32 a3, f32 a4, f32 a5, f32 a6, f32 a7, f32 dt)
{
    Vec3f base;
    Vec3f v;
    Vec3f u;
    Vec3f tgt;
    Vec3f t;
    Vec3f w;
    Vec3f rel;
    Vec3f F;
    f32 dot, d0, d1, d2, e0, e1, e2, uu;
    Phys *ph = &o->ph;
    Vec3f *up;
    Vec3f *vel = &ph->vel;
    f32 kx = 2.0f * sqrtf(50.0f);
    f32 ky;
    f32 kz = 2.0f * sqrtf(40.0f);
    Vec3f *m = ph->m;
    Vec3f *eye;
    f32 sc;
    f32 nm;
    f32 f8, f12, f10;
    s32 i;


    base = ph->pos;
    Vec3f_Dot(&dot, vel, &m[0]);
    eye = &cam->x38;
    up = &cam->x68;
    ky = kz;
    if (cam->xB0->x1A) {
        a3 = -a3;
        a5 = -a5;
        if (dot < 0.0)
            a3 = a3 + dot * 0.042;
    } else {
        if (dot > 0.0)
            a3 = a3 + dot * 0.042;
    }
    nm = -a3;
    sc = dt * 0.25;
    for (i = 0; i < 4; i++) {
        v.x = nm * m[0].x + a4 * m[1].x + a2 * m[2].x;
        v.y = nm * m[0].y + a4 * m[1].y + a2 * m[2].y;
        v.z = nm * m[0].z + a4 * m[1].z + a2 * m[2].z;
        Vec3f_Cross(&w, &ph->angvel, &v);
        Vec3f_Add(&w, &w, vel);
        Vec3f_Sub(&rel, &cam->x98, &w);
        Vec3f_Add(&t, &base, &v);
        Vec3f_Sub(&v, &t, eye);
        Vec3f_Dot(&d0, &v, &m[0]);
        Vec3f_Dot(&d1, &v, &m[1]);
        Vec3f_Dot(&d2, &v, &m[2]);
        Vec3f_Dot(&e0, &rel, &m[0]);
        Vec3f_Dot(&e1, &cam->x98, &m[1]);
        Vec3f_Dot(&e2, &cam->x98, &m[2]);
        f8 = d0 * 50.0f - kx * e0;
        f12 = d1 * 40.0f - ky * e1;
        f10 = d2 * 40.0f - kz * e2;
        F.x = f8 * m[0].x + f12 * m[1].x + f10 * m[2].x;
        F.y = f8 * m[0].y + f12 * m[1].y + f10 * m[2].y;
        F.z = f8 * m[0].z + f12 * m[1].z + f10 * m[2].z;
        cam->x98.x += F.x * sc;
        cam->x98.y += F.y * sc;
        cam->x98.z += F.z * sc;
        eye->x += cam->x98.x * sc;
        eye->y += cam->x98.y * sc;
        eye->z += cam->x98.z * sc;
    }
    if (eye->y < base.y)
        eye->y = base.y;
    tgt.x = base.x + a5 * m[0].x + a6 * up->x;
    tgt.y = base.y + a5 * m[0].y + a6 * up->y;
    tgt.z = base.z + a5 * m[0].z + a6 * up->z;
    if ((!D_800958A1 && cam->xB0->x1C) || (D_800958A1 && cam->xB0->x1B)) {
        dot = fabsf(dot);
        if (44.0 < dot)
            a7 *= 0.4f;
        else
            a7 *= (f32)((44.0 - dot) * 0.0136363636363636357 + 0.4);
        tgt.x += a7 * m[2].x;
        tgt.y += a7 * m[2].y;
        tgt.z += a7 * m[2].z;
    } else if ((!D_800958A1 && cam->xB0->x1B) || (D_800958A1 && cam->xB0->x1C)) {
        dot = fabsf(dot);
        if (44.0 < dot)
            a7 *= 0.4f;
        else
            a7 *= (f32)((44.0 - dot) * 0.0136363636363636357 + 0.4);
        tgt.x -= a7 * m[2].x;
        tgt.y -= a7 * m[2].y;
        tgt.z -= a7 * m[2].z;
    }
    Vec3f_Sub(&v, &tgt, &base);
    Vec3f_Cross(&u, &ph->angvel, &v);
    Vec3f_Add(&u, &u, vel);
    Vec3f_Scale(&u, &u, dt);
    Vec3f_Dot(&uu, &u, &u);
    if (uu < 0.25)
        approach(&cam->x54, &tgt, uu, 0.25f);
    else
        approach(&cam->x54, &tgt, uu, 1.0f);
    Camera_UpdateBasis(cam);
}
