#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f r0; Vec3f r1; Vec3f r2; } Mtx3;

extern f64 D_800B9B18; /* PI */
extern f64 D_800B9B20; /* 1/2PI */
extern f64 D_800B9B28; /* 2PI */
extern f64 D_800B9B30; /* -PI */
extern f64 D_800B9B38;
extern f64 D_800B9B40;
extern f64 D_800B9B48;
extern f64 D_800B9B50;
extern f64 D_800B9B58;
extern f64 D_800B9B60;
extern f64 D_800B9B68;
extern f64 D_800B9B70;
extern f64 D_800B9B78;
extern f64 D_800B9B80;
extern f64 D_800B9B88;
extern f64 D_800B9B90;
extern f64 D_800B9B98;
extern f64 D_800B9BA0;

static inline void SinCos(f32 a, f32 *s, f32 *c) {
    f32 x2;

    if (a > D_800B9B18) {
        a = a - (s32)((a + D_800B9B18) * D_800B9B20) * D_800B9B28;
    } else if (a < D_800B9B30) {
        a = a + (s32)((D_800B9B18 - a) * D_800B9B38) * D_800B9B40;
    }
    x2 = a * a;
    *c = x2 * (x2 * (x2 * (x2 * (x2 * (x2 * D_800B9B48 + D_800B9B58) + D_800B9B68) + D_800B9B78) + D_800B9B88) + D_800B9B98) + D_800B9BA0;
    *s = a * (x2 * (x2 * (x2 * (x2 * (D_800B9B60 - x2 * D_800B9B50) + D_800B9B70) + D_800B9B80) + D_800B9B90) + D_800B9BA0);
}

void Mtx3_RotateY(Mtx3 *m, f32 angle) {
    Vec3f r2;
    Vec3f r0;
    f32 s;
    f32 c;

    r2 = m->r2;
    r0 = m->r0;
    SinCos(angle, &s, &c);
    m->r2.x = c * r2.x + s * r0.x;
    m->r2.y = c * r2.y + s * r0.y;
    m->r2.z = c * r2.z + s * r0.z;
    m->r0.x = -s * r2.x + c * r0.x;
    m->r0.y = -s * r2.y + c * r0.y;
    m->r0.z = -s * r2.z + c * r0.z;
}

extern f64 D_800B9BA8, D_800B9BB0, D_800B9BB8, D_800B9BC0, D_800B9BC8, D_800B9BD0;
extern f64 D_800B9BD8, D_800B9BE0, D_800B9BE8, D_800B9BF0, D_800B9BF8, D_800B9C00;
extern f64 D_800B9C08, D_800B9C10, D_800B9C18, D_800B9C20, D_800B9C28, D_800B9C30;

void Mtx2x3_RotateY(Vec3f *m, f32 angle) {
    Vec3f r0, r1;
    f32 sc[2];
    f64 x = angle;
    f64 s;
    f32 sn, c;

    r0 = m[0];
    r1 = m[1];
    if (x > D_800B9BA8) {
        angle = x - (s32)((x + D_800B9BA8) * D_800B9BB0) * D_800B9BB8;
    } else if (x < D_800B9BC0) {
        angle = x + (s32)((D_800B9BA8 - x) * D_800B9BC8) * D_800B9BD0;
    }
    s = angle * angle;
    c = s * (s * (s * (s * (s * (s * D_800B9BD8 + D_800B9BE8) + D_800B9BF8) + D_800B9C08) + D_800B9C18) + D_800B9C28) + D_800B9C30;
    sn = angle * (s * (s * (s * (s * (D_800B9BF0 - s * D_800B9BE0) + D_800B9C00) + D_800B9C10) + D_800B9C20) + D_800B9C30);
    m[0].x = c * r0.x + sn * r1.x;
    m[0].y = c * r0.y + sn * r1.y;
    m[0].z = c * r0.z + sn * r1.z;
    m[1].x = -sn * r0.x + c * r1.x;
    m[1].y = -sn * r0.y + c * r1.y;
    sc[0] = sn;
    sc[1] = c;
    m[1].z = -sn * r0.z + c * r1.z;
}
