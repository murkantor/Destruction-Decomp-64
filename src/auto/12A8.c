#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
f32 sqrtf(f32);

static inline f32 dist2D(Vec3f p, Vec3f q) {
    f32 dx = p.x - q.x;
    f32 dz = p.z - q.z;

    if (dx == 0.0 && dz == 0.0) {
        return 1.0f;
    }
    return sqrtf(dx * dx + dz * dz);
}

f32 Math_PointLineDist2D(Vec3f a, Vec3f b, Vec3f c) {
    f32 da = dist2D(a, c);
    f32 db = dist2D(b, c);
    f32 dc = dist2D(a, b);
    f32 t, h;

    if (dc == 0.0f) {
        return 0.0f;
    }
    t = (db * db - da * da + dc * dc) / (dc + dc);
    h = db * db - t * t;
    if (h < 0.0) {
        return 0.0f;
    }
    return sqrtf(h);
}
