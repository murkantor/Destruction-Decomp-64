#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

typedef struct {
    f32 x0;
    f32 x4;
    f32 x8;
    f32 xC;
    u8 x10;
} Rel;

f32 Car_GetSpeed(void *);
Vec3f Car_GetPos(void *);
Vec3f Car_GetVelocity(void *);
f64 acos(f64);
f32 cosf(f32);
f32 sqrtf(f32);
f64 fabs(f64);

static inline f32 HDist(Vec3f a, Vec3f b) {
    f32 dx = a.x - b.x;
    f32 dz = a.z - b.z;

    if (dx == 0.0 && dz == 0.0) {
        return 1.0f;
    }
    return sqrtf(dx * dx + dz * dz);
}

static inline f32 Angle(Vec3f a, Vec3f b) {
    f32 d = a.x * b.x + a.y * b.y + a.z * b.z;

    if (fabs(d) < 1e-06) {
        return 0.0f;
    }
    return acos(d / sqrtf((a.x * a.x + a.y * a.y + a.z * a.z) * (b.x * b.x + b.y * b.y + b.z * b.z))) * 57.29577957855229;
}

void func_80000AC8(void *a, void *b, Rel *out) {
    f32 sa = Car_GetSpeed(a);
    f32 sb = Car_GetSpeed(b);
    Vec3f v;
    Vec3f pa;
    Vec3f pb;
    f32 ang;

    out->x10 = 0;
    pa = Car_GetPos(a);
    pb = Car_GetPos(b);
    out->x0 = HDist(pa, pb);
    v.x = pa.x - pb.x;
    v.y = pa.y - pb.y;
    v.z = pa.z - pb.z;
    ang = Angle(v, Car_GetVelocity(b));
    if (ang < 90.0f) {
        v = Car_GetVelocity(a);
        v.x = -v.x;
        v.y = -v.y;
        v.z = -v.z;
        ang = Angle(v, Car_GetVelocity(b));
        out->x8 = (sa + sa) * cosf(ang);
    } else {
        ang = Angle(Car_GetVelocity(a), Car_GetVelocity(b));
        out->x8 = sa * cosf(ang);
    }
    if (out->x8 == 0.0f || sb < 0.8 || out->x0 < 3.0) {
        out->xC = 0.0f;
    } else if (out->x8 > 0.0f) {
        out->xC = out->x0 / out->x8;
    } else {
        out->xC = out->x0 / -(f64)out->x8;
    }
}
