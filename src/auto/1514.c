#include "common.h"
#include "m2c.h"

typedef struct { f32 x0; f32 x4; f32 x8; f32 xC; f32 x10; f32 x14; } Seg;
u8 func_80001670(f32, f32);

static inline f32 Side(f32 ux, f32 uz, f32 px, f32 pz, f32 k) { return ux * px + uz * pz + k; }

s32 Seg_Intersect2D(Seg *a, Seg *b) {
    f32 ax;
    f32 cz;
    f32 bx;
    f32 bz;
    f32 dz;
    f32 cx;
    f32 dx;
    f32 az;
    f32 d1, d2;

    dx = b->xC;
    dz = b->x14;
    cx = b->x0;
    bx = a->xC;
    cz = b->x8;
    bz = a->x14;
    ax = a->x0;
    az = a->x8;

    d1 = Side(bz - az, ax - bx, cx, cz, bx * az - ax * bz);
    d2 = Side(bz - az, ax - bx, dx, dz, bx * az - ax * bz);
    if (d1 != 0.0f && d2 != 0.0f) {
        if (func_80001670(d1, d2)) return 0;
    }
    d1 = Side(dz - cz, cx - dx, ax, az, dx * cz - cx * dz);
    d2 = Side(dz - cz, cx - dx, bx, bz, dx * cz - cx * dz);
    if (d1 != 0.0f && d2 != 0.0f) {
        if (func_80001670(d1, d2)) return 0;
    }
    return 1;
}
