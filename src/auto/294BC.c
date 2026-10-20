#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { s16 x0; s16 x2; s16 x4; u8 pad6[0x10 - 0x6]; } Pt;
typedef struct { s32 minX, maxX, minZ, maxZ; } Bounds;

extern double D_800B9050;
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Length(f32 *, Vec3f *);

void Car_DeformVerts(Vec3f pos, Pt *p, s32 n, f32 scale, Bounds *b) {
    Vec3f v;
    Vec3f d;
    f32 len;
    f32 f;
    s32 i;

    for (i = 0; i < n; p++, i++) {
        if (p->x0 > b->maxX || p->x0 < b->minX || p->x4 > b->maxZ || p->x4 < b->minZ) {
            continue;
        }
        v.x = p->x0;
        v.y = p->x2;
        v.z = p->x4;
        Vec3f_Sub(&d, &pos, &v);
        Vec3f_Length(&len, &d);
        len = len * D_800B9050;
        f = scale - len;
        p->x0 = p->x0 * f;
        p->x4 = p->x4 * f;
    }
}
