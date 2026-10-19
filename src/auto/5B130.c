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

static inline void Vec3f_Dot(f32 *out, Vec3f *a, Vec3f *b) {
    *out = a->x * b->x + a->y * b->y + a->z * b->z;
}

void Vec3f_CosAngle(f32 *out, Vec3f *a, Vec3f *b) {
    f32 dot;
    f32 lenA;
    f32 lenB;

    Vec3f_Dot(&dot, a, b);
    Vec3f_Length(&lenA, a);
    Vec3f_Length(&lenB, b);
    *out = dot / (lenA * lenB);
}
