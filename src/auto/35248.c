#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[4]; Vec3f n; s32 count; Vec3f v[1]; } Poly;
void Vec3f_Cross(Vec3f *out, Vec3f *a, Vec3f *b);

s32 func_80034648(Poly *poly, Vec3f *p) {
    Vec3f e1;
    Vec3f e2;
    Vec3f c;
    Vec3f *prev;
    Vec3f *cur;
    s32 i;

    prev = &poly->v[poly->count - 1];
    for (i = 0; i < poly->count; i++) {
        cur = &poly->v[i];
        e1.x = cur->x - prev->x;
        e1.y = cur->y - prev->y;
        e1.z = cur->z - prev->z;
        e2.x = p->x - prev->x;
        e2.y = p->y - prev->y;
        e2.z = p->z - prev->z;
        Vec3f_Cross(&c, &e1, &e2);
        if (c.x * poly->n.x + c.y * poly->n.y + c.z * poly->n.z < 0.0) {
            return 0;
        }
        prev = cur;
    }
    return 1;
}
