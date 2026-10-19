#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x10]; s32 n; Vec3f pts[1]; } Poly;

s32 Poly_ContainsXZ(Poly *p, Vec3f *pt) {
    char unused[1];
    Vec3f *prev = &p->pts[p->n - 1];
    Vec3f *cur;
    s32 i;

    for (i = 0; i < p->n; i++) {
        cur = &p->pts[i];
        if ((cur->z - prev->z) * (pt->x - prev->x) - (cur->x - prev->x) * (pt->z - prev->z) < 0.0) {
            return 0;
        }
        prev = cur;
    }
    return 1;
}
