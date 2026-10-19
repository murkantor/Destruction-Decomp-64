#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3f;
float sqrtf(float);

void Vec3f_Length(f32 *out, Vec3f *v) {
    f32 lenSq = v->x * v->x + v->y * v->y + v->z * v->z;

    if (lenSq == 0.0f) {
        *out = 0.0f;
    } else {
        *out = sqrtf(lenSq);
    }
}
