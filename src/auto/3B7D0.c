#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3;

Vec3 Mtx3_MulVec3f(f32 m[3][3], Vec3 *v) {
    Vec3 r;
    r.x = m[0][0] * v->x + m[0][1] * v->y + m[0][2] * v->z;
    r.y = m[1][0] * v->x + m[1][1] * v->y + m[1][2] * v->z;
    r.z = m[2][0] * v->x + m[2][1] * v->y + m[2][2] * v->z;
    return r;
}
