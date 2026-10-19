#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3f;
float sqrtf(float);

extern const double D_800BBE58;

void Vec3f_Normalize(Vec3f *out, Vec3f *v) {
    f32 lenSq = v->x * v->x + v->y * v->y + v->z * v->z;
    f32 inv;

    if (lenSq <= D_800BBE58) {
        out->x = 0.0f;
        out->y = 0.0f;
        out->z = 0.0f;
    } else {
        inv = 1.0f / sqrtf(lenSq);
        out->x = v->x * inv;
        out->y = v->y * inv;
        out->z = v->z * inv;
    }
}
