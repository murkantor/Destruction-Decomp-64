#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Normalize(Vec3f *, Vec3f *);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);

f32 Snd_DopplerPitch(Vec3f *a, Vec3f *b, Vec3f *c, Vec3f *d) {
    Vec3f diff;
    Vec3f n;
    f32 t;
    f32 r;

    Vec3f_Sub(&diff, c, a);
    Vec3f_Normalize(&n, &diff);
    Vec3f_Dot(&t, b, &n);
    r = t + 331.0f;
    Vec3f_Dot(&t, d, &n);
    r = r / (t + 331.0f);
    if (!(r >= 1.0)) {
        return (r - 1.0) * 24.0;
    }
    return (r - 1.0) * 12.0;
}
