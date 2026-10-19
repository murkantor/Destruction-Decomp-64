#include "common.h"
#include "m2c.h"

typedef struct { f32 ext[3]; f32 pos[3]; f32 m[3][3]; } Box32D88;
typedef struct { f32 min[3]; f32 max[3]; } Aabb32D88;
double fabs(double);

void Obb_ToAabb(Box32D88 *b, Aabb32D88 *out) {
    f32 r0;
    f32 r1;
    f32 r2;

    r0 = fabs(b->ext[0] * b->m[0][0]) + fabs(b->ext[1] * b->m[1][0]) + fabs(b->ext[2] * b->m[2][0]);
    r1 = fabs(b->ext[0] * b->m[0][1]) + fabs(b->ext[1] * b->m[1][1]) + fabs(b->ext[2] * b->m[2][1]);
    r2 = fabs(b->ext[0] * b->m[0][2]) + fabs(b->ext[1] * b->m[1][2]) + fabs(b->ext[2] * b->m[2][2]);
    out->max[0] = b->pos[0] + r0;
    out->min[0] = b->pos[0] - r0;
    out->max[1] = b->pos[1] + r1;
    out->min[1] = b->pos[1] - r1;
    out->max[2] = b->pos[2] + r2;
    out->min[2] = b->pos[2] - r2;
}
