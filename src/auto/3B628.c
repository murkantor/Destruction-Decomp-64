#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { f32 xx, yy, zz, xy, xz, yz; } Tensor;

Vec3f Tensor_MulVec3f(Tensor *t, Vec3f *v) {
    Vec3f r;
    f32 x = v->x;
    f32 y = v->y;
    f32 z = v->z;
    f32 rx = x * t->xx + y * t->xy + z * t->xz;
    f32 ry = x * t->xy + y * t->yy + z * t->yz;
    f32 rz = x * t->xz + y * t->yz + z * t->zz;

    r.x = rx;
    r.y = ry;
    r.z = rz;
    return r;
}
