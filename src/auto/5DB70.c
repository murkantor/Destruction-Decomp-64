#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Length(f32 *, Vec3f *);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
f64 acos(f64);

s32 func_8005CF70(Vec3f *a, Vec3f *b, Vec3f *c) {
    Vec3f v;
    Vec3f d;
    f32 lenV;
    f32 lenD;
    f32 dot;
    f32 ang;

    v = *b;
    Vec3f_Sub(&d, c, a);
    v.y = 0.0f;
    d.y = 0.0f;
    Vec3f_Length(&lenV, &v);
    Vec3f_Length(&lenD, &d);
    if (lenD <= 0.2) {
        ang = 90.0f;
    } else {
        Vec3f_Dot(&dot, &v, &d);
        ang = (f32)acos(dot / (lenD * lenV)) * 57.2957795785522919;
        if (v.z * d.x - v.x * d.z >= 0.0) {
            if (ang <= 90.0) {
                ang = 90.0 - ang;
            } else {
                ang = ang - 90.0;
            }
        } else {
            if (ang <= 90.0) {
                ang = ang + 90.0f;
            } else {
                ang = 270.0 - ang;
            }
        }
    }
    return ang * 1.42222222222222228;
}
