#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x10]; s32 n; Vec3f pts[1]; } Poly;

s32 Poly_ContainsXZMargin(Poly *p, Vec3f *pt, f32 m) {
    char unused[1];
    Vec3f *prev = &p->pts[p->n - 1];
    Vec3f *cur;
    s32 i;
    f32 dx, dz, ex, ez;

    for (i = 0; i < p->n; i++) {
        cur = &p->pts[i];
        dx = pt->x - prev->x;
        ex = cur->x - prev->x;
        dz = pt->z - prev->z;
        ez = cur->z - prev->z;
        if (ex > 0.0) {
            dz -= m;
        } else {
            dz += m;
        }
        if (ez > 0.0) {
            dx += m;
        } else {
            dx -= m;
        }
        if (ez * dx - ex * dz < 0.0) {
            return 0;
        }
        prev = cur;
    }
    return 1;
}
