#include "common.h"
#include "m2c.h"

extern const double D_800B9A80;
float sqrtf(float);
double fabs(double);

s32 Math_SolveQuadratic(f32 a, f32 b, f32 c, f32 *x1, f32 *x2) {
    f32 disc = b * b - 4.0f * a * c;
    f32 root;
    f32 inv;

    if (disc >= 0.0f && fabs(a) > D_800B9A80) {
        root = sqrtf(disc);
        inv = 1.0f / (2.0f * a);
        *x1 = (-b + root) * inv;
        *x2 = (-b - root) * inv;
        return 1;
    }
    return 0;
}
