#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3f;
float sinf(float);
float cosf(float);

void Vec3f_RotateY(Vec3f *out, Vec3f *v, f32 angle) {
    out->x = cosf(angle) * v->x + sinf(angle) * v->z;
    out->y = v->y;
    out->z = cosf(angle) * v->z - sinf(angle) * v->x;
}
