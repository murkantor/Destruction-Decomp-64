#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3;
Vec3 Mtx3_MulDiff(f32 m[3][3], Vec3 *a, Vec3 *b) {
    Vec3 d, r;
    d.x = b->x - a->x;
    d.y = b->y - a->y;
    d.z = b->z - a->z;
    r.x = m[0][0] * d.x + m[0][1] * d.y + m[0][2] * d.z;
    r.y = m[1][0] * d.x + m[1][1] * d.y + m[1][2] * d.z;
    r.z = m[2][0] * d.x + m[2][1] * d.y + m[2][2] * d.z;
    d = r;
    return d;
}
