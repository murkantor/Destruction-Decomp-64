#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

Vec3f Mtx3_MulVec3fTransposed(f32 *m, Vec3f *v) {
    Vec3f r;

    r.x = m[0] * v->x + m[3] * v->y + m[6] * v->z;
    r.y = m[1] * v->x + m[4] * v->y + m[7] * v->z;
    r.z = m[2] * v->x + m[5] * v->y + m[8] * v->z;
    return r;
}
