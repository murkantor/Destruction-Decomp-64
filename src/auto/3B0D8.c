#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
void func_8003A568(void *, Vec3f *, f32);

void func_8003A4D8(void *a, Vec3f *v) {
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
    func_8003A568(a, &n, len);
}
