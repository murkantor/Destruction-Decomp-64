#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { s32 x0; Vec3f normal; s32 count; Vec3f verts[1]; } Poly34524;
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Normalize(Vec3f *, Vec3f *);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);

s32 Poly_ContainsPoint(Poly34524 *poly, Vec3f *pt, f32 eps) {
    Vec3f edge;
    Vec3f side;
    Vec3f rel;
    f32 dot;
    s32 i;
    Vec3f *prev;
    Vec3f *cur;
    Vec3f *normal;

    normal = &poly->normal;
    prev = &poly->verts[poly->count - 1];
    for (i = 0; i < poly->count; i++) {
        cur = &poly->verts[i];
        Vec3f_Sub(&edge, cur, prev);
        Vec3f_Cross(&side, &edge, normal);
        Vec3f_Normalize(&side, &side);
        Vec3f_Sub(&rel, pt, prev);
        Vec3f_Dot(&dot, &rel, &side);
        if (eps < dot) {
            return 0;
        }
        prev = cur;
    }
    return 1;
}
