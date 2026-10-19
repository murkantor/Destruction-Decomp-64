#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } V3;
void Vec3_AddToArray(V3 *a, V3 *b, s32 n) {
    s32 i;
    V3 *p = a; for (i = 0; i < n; i++) { p->x += b->x; p->y += b->y; p->z += b->z; p++; }
}
