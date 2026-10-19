#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

typedef struct {
    u8 pad0[0x10];
    s32 n;
    Vec3f v[1];
} Poly;

static inline Vec3f Intersect(Vec3f *cur, Vec3f *prev, f32 dPrev, f32 d) {
    Vec3f r;
    f32 t = 1.0f / (dPrev - d);

    r.x = t * (dPrev * cur->x - d * prev->x);
    r.y = t * (dPrev * cur->y - d * prev->y);
    r.z = t * (dPrev * cur->z - d * prev->z);
    return r;
}

void Poly_ClipAgainstPlane(Poly *poly, Vec3f *pt, Vec3f *nrm, f32 *minOut, Vec3f *out) {
    Vec3f *prev = &poly->v[poly->n - 1];
    Vec3f *cur;
    f32 dPrev;
    f32 d;
    f32 zero;
    s32 i;
    s32 k = 0;

    dPrev = nrm->x * (prev->x - pt->x) + nrm->y * (prev->y - pt->y) + nrm->z * (prev->z - pt->z);
    *minOut = 0.0f;
    for (i = 0; i < poly->n; i++) {
        zero = *(const f32 *)minOut;
        cur = &poly->v[i];
        d = nrm->x * (cur->x - pt->x) + nrm->y * (cur->y - pt->y) + nrm->z * (cur->z - pt->z);
        if (zero < d) {
            if (dPrev < zero) {
                out[k++] = Intersect(cur, prev, dPrev, d);
                if (dPrev < *(volatile f32 *)minOut) {
                    *minOut = dPrev;
                }
            }
        } else if (d < zero) {
            if (zero < dPrev) {
                out[k++] = Intersect(cur, prev, dPrev, d);
            }
            if (d < *(volatile f32 *)minOut) {
                *minOut = d;
            }
        } else {
            out[k++] = *cur;
        }
        prev = cur;
        dPrev = d;
    }
}
