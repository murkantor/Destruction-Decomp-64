#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { f32 x, y, z, w; } Vtx16;
typedef struct { u16 idx[10]; u8 cnt; u8 pad[3]; } Poly;
typedef struct { u8 pad[0x5C]; Poly *polys; Vtx16 *verts; } Grid;
typedef struct { u8 x0; u8 pad1[3]; Vec3f v[3]; s32 x28; } Ent2C;
typedef struct { f32 a, b, c; } Plane;
typedef struct {
    u8 pad0[0x224];
    Ent2C *x224;
    s32 x228;
    u8 pad22C[0x25C - 0x22C];
    Plane planes[4];
    f32 d[4];
    Plane plane5;
    f32 d5;
} Mgr;
extern Grid *D_800BC9C4;
s32 Poly_ClipToPlane(Vec3f *, Vec3f *, s32, Plane *, f32);

void Skid_ClipPolys(Mgr *m, s32 *ids, s32 n) {
    s32 i;
    s32 k;
    s32 cnt;
    s32 r;
    Poly *poly;
    Vec3f bufA[10];
    Vec3f bufB[10];
    Grid *grid;
    Vtx16 *v;
    Vec3f *first;
    Vec3f *q;

    for (i = 0; i < n; i++) {
        poly = &D_800BC9C4->polys[ids[i]];
        cnt = poly->cnt;
        for (k = 0; k < cnt; k++) {
            v = &D_800BC9C4->verts[poly->idx[k]];
            bufA[k].x = v->x;
            bufA[k].y = v->y;
            bufA[k].z = v->z;
        }
        r = Poly_ClipToPlane(bufB, bufA, cnt, &m->planes[0], m->d[0]);
        if (r == 0) continue;
        r = Poly_ClipToPlane(bufA, bufB, r, &m->planes[1], m->d[1]);
        if (r == 0) continue;
        r = Poly_ClipToPlane(bufB, bufA, r, &m->planes[2], m->d[2]);
        if (r == 0) continue;
        r = Poly_ClipToPlane(bufA, bufB, r, &m->planes[3], m->d[3]);
        if (r == 0) continue;
        r = Poly_ClipToPlane(bufB, bufA, r, &m->plane5, m->d5);
        if (r == 0) continue;
        if (r < 3) continue;
        first = bufB;
        for (k = 0; k < r - 2; k++) {
            m->x224[m->x228].v[0] = first[0];
            m->x224[m->x228].v[1] = first[k + 1];
            m->x224[m->x228].v[2] = first[k + 2];
            m->x224[m->x228].x0 = 1;
            m->x224[m->x228].x28 = 20;
            m->x228++;
            if (m->x228 >= 40) {
                m->x228 = 0;
            }
        }
    }
}
