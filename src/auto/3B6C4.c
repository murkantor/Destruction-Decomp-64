#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f a; Vec3f b; Vec3f c; } Basis3AAC4;
void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
void guNormalize(f32 *, f32 *, f32 *);

void Basis_Orthonormalize(Basis3AAC4 *m) {
    Vec3f a;
    Vec3f b;
    Vec3f c;

    a = m->a;
    b = m->b;
    Vec3f_Cross(&c, &a, &b);
    Vec3f_Cross(&b, &c, &a);
    guNormalize(&a.x, &a.y, &a.z);
    guNormalize(&b.x, &b.y, &b.z);
    guNormalize(&c.x, &c.y, &c.z);
    m->a = a;
    m->b = b;
    m->c = c;
}
