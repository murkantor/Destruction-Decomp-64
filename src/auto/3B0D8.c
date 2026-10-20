#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
void Mtx3_RotateAxisAngle(void *, Vec3f *, f32);

void Mtx3_RotateByVector(void *a, Vec3f *v) {
    f32 len = v->x * v->x + v->y * v->y + v->z * v->z;
    f32 inv;
    Vec3f n;

    if (len < 1e-12) {
        return;
    }
    len = sqrtf(len);
    inv = 1.0f / len;
    n.x = v->x * inv;
    n.y = v->y * inv;
    n.z = v->z * inv;
    Mtx3_RotateAxisAngle(a, &n, len);
}
