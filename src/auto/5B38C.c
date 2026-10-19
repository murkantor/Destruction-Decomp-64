#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3f;
float sinf(float);
float cosf(float);

void Vec3f_RotateX(Vec3f *out, Vec3f *v, f32 angle) {
    out->x = v->x;
    out->y = cosf(angle) * v->y - sinf(angle) * v->z;
    out->z = cosf(angle) * v->z + sinf(angle) * v->y;
}
