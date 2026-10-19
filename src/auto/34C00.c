#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad0[0x10]; s32 count; Vec3f pts[1]; } Poly34000;

s32 Poly_StraddlesPlane(Poly34000 *poly, Vec3f *p, Vec3f *n) {
    s32 i;
    s32 pos = 0;
    s32 neg = 0;
    f32 d;
    Vec3f *pt;

    for (i = 0; i < poly->count; i++) {
        pt = (Vec3f *)((u8 *)poly + (i * 12 + 0x14));
        d = n->x * (pt->x - p->x) + n->y * (pt->y - p->y) + n->z * (pt->z - p->z);
        if (d > 1e-5) {
            pos++;
        } else if (d < -1e-5) {
            neg++;
        }
        if (pos > 0 && neg > 0) {
            return 1;
        }
    }
    return 0;
}
