#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3;
extern f64 D_800B7D10;
extern f64 D_800B7D18;
f64 acos(f64);
f32 sqrtf(f32);
f64 fabs(f64);

f32 Vec3f_AngleBetween(Vec3 a, Vec3 b) {
    f32 dot = a.x * b.x + a.y * b.y + a.z * b.z;

    if (fabs(dot) < D_800B7D10) {
        return 0.0f;
    }
    return acos(dot / sqrtf((a.x * a.x + a.y * a.y + a.z * a.z) * (b.x * b.x + b.y * b.y + b.z * b.z))) * D_800B7D18;
}
