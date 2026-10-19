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

float sinf(float);
float cosf(float);

static inline void Vec3f_FromAngleXY(Vec3f *out, f32 angle) {
    out->x = cosf(angle);
    out->y = sinf(angle);
    out->z = 0.0f;
}

static inline void Vec3f_Scale(Vec3f *out, Vec3f *v, f32 s) {
    out->x = v->x * s;
    out->y = v->y * s;
    out->z = v->z * s;
}

void Vec3f_FromAngleXYScaled(Vec3f *out, Vec3f *v, f32 angle) {
    f32 len;

    Vec3f_Length(&len, v);
    Vec3f_FromAngleXY(out, angle);
    Vec3f_Scale(out, out, len);
}
