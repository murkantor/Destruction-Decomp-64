#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

f32 Vec3f_DistXZ(Vec3f a, Vec3f b) {
    f32 dx = a.x - b.x;
    f32 dz = a.z - b.z;

    if (dx == 0.0 && dz == 0.0) {
        return 1.0f;
    }
    return sqrtf(dx * dx + dz * dz);
}
