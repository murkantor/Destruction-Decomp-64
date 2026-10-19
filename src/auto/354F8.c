#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
extern f64 D_800B9558;
double fabs(double);

s32 Plane_IntersectSegment(Vec3f *p, Vec3f *n, f32 d, Vec3f *a, Vec3f *b, Vec3f *out1, Vec3f *out2, f32 *t) {
    Vec3f v;
    f32 d1, d2, tt;
    v.x = a->x - p->x;
    v.y = a->y - p->y;
    v.z = a->z - p->z;
    d1 = v.x * n->x + v.y * n->y + v.z * n->z;
    v.x = b->x - p->x;
    v.y = b->y - p->y;
    v.z = b->z - p->z;
    d2 = v.x * n->x + v.y * n->y + v.z * n->z;
    if (fabs(d1 - d2) < D_800B9558) {
        return 0;
    }
    d1 -= d;
    d2 -= d;
    tt = d1 / (d1 - d2);
    v.x = b->x - a->x;
    v.y = b->y - a->y;
    v.z = b->z - a->z;
    *t = tt;
    out1->x = a->x + *t * v.x;
    out1->y = a->y + *t * v.y;
    out1->z = a->z + *t * v.z;
    out2->x = out1->x - d * n->x;
    out2->y = out1->y - d * n->y;
    out2->z = out1->z - d * n->z;
    return 1;
}

typedef struct { s32 a, b, c, d; } Blk;
typedef struct {
    Blk x0;
    Vec3f x10;
    f32 x1C;
    Vec3f x20;
    Vec3f x2C;
    f32 x38;
    Vec3f x3C;
    Vec3f x48;
    s32 x54;
    u8 pad[0x8];
} Body;
typedef struct { f32 xx, yy, zz, xy, xz, yz; } Tensor;
typedef struct { u8 pad[0x10]; f32 x10; u8 pad2[0x104 - 0x14]; Tensor x104; } World;
extern void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
extern Vec3f Tensor_MulVec3f(Tensor *, Vec3f *);
extern double fabs(double);
extern void func_80034C40(World *, Body *);

void Physics_AddContact(Body *bodies, s32 *count, Body *src, World *w) {
    Body *dst;
    Vec3f tmp;
    f32 d;

    if (*count >= 0x33) {
        return;
    }
    if (*count != 0x32) {
        dst = (Body *)(*count * sizeof(Body) + (u32)bodies);
        dst->x0 = src->x0;
        dst->x1C = src->x1C;
        dst->x10 = src->x10;
        dst->x2C = src->x2C;
        dst->x20 = src->x20;
        dst->x48 = src->x48;
        dst->x3C = src->x3C;
        Vec3f_Cross(&tmp, &src->x2C, &src->x10);
        tmp = Tensor_MulVec3f(&w->x104, &tmp);
        Vec3f_Cross(&tmp, &tmp, &src->x2C);
        d = fabs(src->x10.x * tmp.x + src->x10.y * tmp.y + src->x10.z * tmp.z);
        dst->x38 = w->x10 + d;
        dst->x54 = 0;
        func_80034C40(w, dst);
        (*count)++;
    }
}
