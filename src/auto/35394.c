#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3f;

typedef struct {
    u8 pad0[4];
    Vec3f normal;
    s32 count;
    Vec3f verts[1];
} Poly;

u8 Plane_IntersectSegment(Vec3f *, Vec3f *, f32, void *, void *, void *, Vec3f *, void *);
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Normalize(Vec3f *, Vec3f *);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);

static inline u8 InsidePoly(Poly *p, f32 tol, Vec3f *pt) {
    Vec3f *normal = &p->normal;
    Vec3f edge;
    Vec3f n;
    Vec3f d;
    f32 dot;
    s32 i;
    Vec3f *prev = &p->verts[p->count - 1];
    Vec3f *cur;

    for (i = 0; i < p->count; i++) {
        cur = &p->verts[i];
        Vec3f_Sub(&edge, cur, prev);
        Vec3f_Cross(&n, &edge, normal);
        Vec3f_Normalize(&n, &n);
        Vec3f_Sub(&d, pt, prev);
        Vec3f_Dot(&dot, &d, &n);
        if (tol < dot) {
            return 0;
        }
        prev = cur;
    }
    return 1;
}

u8 func_80034794(Poly *p, f32 tol, void *a2, void *a3, void *a4, Vec3f *pt, void *a6) {
    if (Plane_IntersectSegment(p->verts, &p->normal, tol, a2, a3, a4, pt, a6)) {
        if (InsidePoly(p, tol, pt)) {
            return 1;
        }
    }
    return 0;
}
