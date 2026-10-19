#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3f;
float sqrtf(float);

static inline void Vec3f_Length(f32 *out, Vec3f *v) {
    f32 lenSq = v->x * v->x + v->y * v->y + v->z * v->z;

    if (lenSq == 0.0f) {
        *out = 0.0f;
    } else {
        *out = sqrtf(lenSq);
    }
}

void Vec3f_SetLength(Vec3f *out, Vec3f *v, f32 length) {
    f32 len;
    f32 scale;

    Vec3f_Length(&len, v);
    if (len != 0.0f) {
        scale = length / len;
        out->x = v->x * scale;
        out->y = v->y * scale;
        out->z = v->z * scale;
    }
}
