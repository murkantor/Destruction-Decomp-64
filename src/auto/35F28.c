#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { f32 xx, yy, zz, xy, xz, yz; } Tensor;
typedef struct { Vec3f f; Vec3f t; } Impulse;

typedef struct {
    f32 x0;
    f32 x4;
    u8 pad8[0x10 - 0x8];
    Vec3f x10;
    f32 x1C;
    Vec3f x20;
    Vec3f x2C;
    f32 x38;
    Vec3f x3C;
    Vec3f x48;
    f32 x54;
} Contact;

typedef struct {
    u8 pad0[0x10];
    f32 x10;
    u8 pad14[0xC8 - 0x14];
    Vec3f xC8;
    u8 padD4[0xD8 - 0xD4];
    f32 xD8;
    u8 padDC[0x104 - 0xDC];
    Tensor x104;
} Body;

typedef struct {
    u8 pad0[0x1EC];
    Body x1EC;
    u8 pad[0xBE5 - 0x1EC - sizeof(Body)];
    u8 xBE5;
} Car;

void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
void Vec3f_Normalize(Vec3f *, Vec3f *);
void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
extern f64 D_800B95F8;
Vec3f Tensor_MulVec3f(Tensor *, Vec3f *);

Impulse Contact_ComputeImpulse(Car *car, Contact *c) {
    Impulse res;
    Vec3f r;
    Vec3f n;
    Vec3f rel;
    Vec3f tang;
    Vec3f imp;
    Vec3f dir;
    Vec3f cr;
    Vec3f fr;
    f32 dot;
    Body *body;
    f32 j;
    f32 px, py, pz;
    f32 tx, ty, tz;
    f32 len;

    r = c->x2C;
    n = c->x10;
    Vec3f_Sub(&rel, &c->x20, &c->x3C);
    Vec3f_Dot(&dot, &rel, &n);
    px = dot * n.x;
    py = dot * n.y;
    pz = dot * n.z;
    j = (-(c->x0 + 1.0f) * dot) / (c->x38 + c->x54);
    tx = rel.x - px;
    ty = rel.y - py;
    tz = rel.z - pz;
    len = tx * tx + ty * ty + tz * tz;
    body = &car->x1EC;
    tang.x = tx;
    tang.y = ty;
    tang.z = tz;
    if (len > 25.0f) {
        Vec3f_Normalize(&dir, &tang);
    } else {
        Vec3f_Scale(&dir, &tang, 0.2f);
    }
    imp.x = j * n.x;
    imp.y = j * n.y;
    imp.z = j * n.z;
    fr.x = -j * c->x4 * dir.x;
    fr.y = -j * c->x4 * dir.y;
    fr.z = -j * c->x4 * dir.z;
    Vec3f_Add(&imp, &imp, &fr);
    res.f.x = body->x10 * imp.x;
    res.f.y = body->x10 * imp.y;
    res.f.z = body->x10 * imp.z;
    if (body->xD8 < D_800B95F8 && car->xBE5 == 0) {
        Vec3f a;
        Vec3f b;
        Vec3f s;
        f32 d2;

        Vec3f_Dot(&d2, &fr, &body->xC8);
        Vec3f_Scale(&a, &body->xC8, d2);
        Vec3f_Sub(&b, &fr, &a);
        Vec3f_Scale(&s, &b, 4.0f);
        Vec3f_Add(&imp, &imp, &s);
        Vec3f_Scale(&s, &a, 1.0f);
        Vec3f_Add(&imp, &imp, &s);
    }
    Vec3f_Cross(&cr, &r, &imp);
    res.t = Tensor_MulVec3f(&body->x104, &cr);
    return res;
}
