#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad0[0xC]; f32 xC; u8 pad10[4]; f32 x14; f32 x18; f32 x1C; u8 pad20[0x90]; Vec3f xB0; Vec3f xBC; Vec3f xC8; Vec3f xD4; Vec3f xE0; } Body;
void Vec3f_Dot(f32 *out, Vec3f *a, Vec3f *b);
void Vec3f_Add(Vec3f *out, Vec3f *a, Vec3f *b);
f64 fabs(f64);

s32 Body_EnergyRatioExceeds(Body *o, Vec3f *dv) {
    Vec3f v;
    Vec3f w;
    f32 d[4];
    f32 ix;
    f32 iy;
    f32 iz;
    f32 e0;
    f32 e1;
    f32 r;

    ix = o->x14;
    iy = o->x18;
    iz = o->x1C;
    Vec3f_Dot(&d[0], &o->xB0, &o->xB0);
    Vec3f_Dot(&d[1], &o->xBC, &o->xC8);
    Vec3f_Dot(&d[2], &o->xBC, &o->xD4);
    Vec3f_Dot(&d[3], &o->xBC, &o->xE0);
    e0 = (o->xC * d[0] + ix * d[1] * d[1] + iy * d[2] * d[2] + iz * d[3] * d[3]) * 0.5;
    Vec3f_Add(&v, &o->xB0, &dv[0]);
    Vec3f_Add(&w, &o->xBC, &dv[1]);
    Vec3f_Dot(&d[0], &v, &v);
    Vec3f_Dot(&d[1], &w, &o->xC8);
    Vec3f_Dot(&d[2], &w, &o->xD4);
    Vec3f_Dot(&d[3], &w, &o->xE0);
    e1 = (o->xC * d[0] + ix * d[1] * d[1] + iy * d[2] * d[2] + iz * d[3] * d[3]) * 0.5;
    if (fabs(e0) > 1e-9) {
        d[0] = e1 / e0;
    } else {
        d[0] = 1.1f;
    }
    if (d[0] > 1.0f) {
        return 1;
    }
    return 0;
}
