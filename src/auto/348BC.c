#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct {
    u8 pad0[0x4];
    Vec3f n;
    s32 count;
    Vec3f v[1];
} Poly;

u8 Poly_StraddlesPlane(Poly *poly, Vec3f *p, Vec3f *n);
void Poly_ClipAgainstPlane(Poly *poly, Vec3f *pt, Vec3f *nrm, f32 *minOut, Vec3f *out);
extern const double D_800B9540;

s32 Poly_IntersectSegment(Poly *a, f32 *minA, Poly *b, f32 *minB, Vec3f *out)
{
    Vec3f d;
    Vec3f s1[2];
    Vec3f s2[2];
    Vec3f e;
    f32 len2, t0, t1, tmp, lo, hi, dot;

    if (0.0f < a->n.x * b->n.x + a->n.y * b->n.y + a->n.z * b->n.z) {
        return 0;
    }
    if (!Poly_StraddlesPlane(a, &b->v[0], &b->n)) {
        return 0;
    }
    if (!Poly_StraddlesPlane(b, &a->v[0], &a->n)) {
        return 0;
    }
    Poly_ClipAgainstPlane(a, &b->v[0], &b->n, minA, s1);
    Poly_ClipAgainstPlane(b, &a->v[0], &a->n, minB, s2);
    d.x = s1[1].x - s1[0].x;
    d.y = s1[1].y - s1[0].y;
    d.z = s1[1].z - s1[0].z;
    len2 = d.x * d.x + d.y * d.y + d.z * d.z;
    if (len2 < D_800B9540) {
        return 0;
    }
    e.x = s2[0].x - s1[0].x;
    e.y = s2[0].y - s1[0].y;
    e.z = s2[0].z - s1[0].z;
    dot = d.x * e.x + d.y * e.y + d.z * e.z;
    e.x = s2[1].x - s1[0].x;
    e.y = s2[1].y - s1[0].y;
    e.z = s2[1].z - s1[0].z;
    len2 = 1.0f / len2;
    t0 = dot * len2;
    t1 = (d.x * e.x + d.y * e.y + d.z * e.z) * len2;
    if (t1 < t0) {
        tmp = t1;
        t1 = t0;
        t0 = tmp;
    }
    lo = 0.0f;
    if (!(t0 <= 0.0f)) {
        lo = t0;
    }
    hi = 1.0f;
    if (!(1.0f <= t1)) {
        hi = t1;
    }
    if (!(lo < hi)) {
        return 0;
    }
    {
        out[0].x = s1[0].x + lo * d.x;
        out[0].y = s1[0].y + lo * d.y;
        out[0].z = s1[0].z + lo * d.z;
        out[1].x = s1[0].x + hi * d.x;
        out[1].y = s1[0].y + hi * d.y;
        out[1].z = s1[0].z + hi * d.z;
    }
    return 1;
}
