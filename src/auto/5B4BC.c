#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3f;
float sinf(float);
float cosf(float);

void Vec3f_RotateZ2(Vec3f *out, Vec3f *v, f32 angle) {
    out->x = cosf(angle) * v->x - sinf(angle) * v->y;
    out->y = cosf(angle) * v->y + sinf(angle) * v->x;
    out->z = v->z;
}
