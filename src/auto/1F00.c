#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3f;

typedef struct {
    u8 pad[0x10];
    s32 x10;
} Obj1300;

void Car_GetPos(Vec3f *, void *);
f32 sqrtf(f32);

static inline f32 Dist1300(void *a, void *b) {
    Vec3f v[2];
    f32 dx;
    f32 dz;

    Car_GetPos(&v[0], a);
    Car_GetPos(&v[1], b);
    dx = v[0].x - v[1].x;
    dz = v[0].z - v[1].z;
    if (dx == 0.0 && dz == 0.0) {
        return 1.0f;
    }
    return sqrtf(dx * dx + dz * dz);
}

Obj1300 *Obj_PickNearer(void *a, Obj1300 *b, Obj1300 *c) {
    f32 d;

    if (b->x10 <= 0) {
        return c;
    }
    if (c == NULL) {
        return b;
    }
    d = Dist1300(a, b);
    if (d < Dist1300(a, c)) {
        return b;
    }
    return c;
}
