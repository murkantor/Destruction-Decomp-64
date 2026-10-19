#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f pos; s32 xC; s32 x10; } ClipVtx;
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);

static inline void Vec3f_Scale(Vec3f *v, f32 t) {
    f32 x = v->x * t;
    f32 y = v->y * t;
    f32 z = v->z * t;
    v->x = x;
    v->y = y;
    v->z = z;
}

s32 Poly_ClipSH(ClipVtx *out, ClipVtx *in, s32 n, Vec3f *normal, f32 d) {
    f32 dprev, dcur;
    s32 i, prev, count;
    f32 t;
    ClipVtx *cur;
    ClipVtx *p;

    Vec3f_Dot(&dprev, normal, &in[n - 1].pos);
    dprev -= d;
    count = 0;
    for (i = 0; i < n; i++) {
        prev = i - 1;
        if (prev < 0) prev = n - 1;
        Vec3f_Dot(&dcur, normal, &in[i].pos);
        dcur -= d;
        if (dprev >= 0.0f) {
            out[count] = in[prev];
            count++;
        }
        if (dprev * dcur < 0.0f) {
            t = dprev / (dprev - dcur);
            p = &in[prev];
            Vec3f_Sub(&out[count].pos, &in[i].pos, &p->pos);
            Vec3f_Scale(&out[count].pos, t);
            Vec3f_Add(&out[count].pos, &out[count].pos, &p->pos);
            count++;
        }
        dprev = dcur;
    }
    return count;
}
