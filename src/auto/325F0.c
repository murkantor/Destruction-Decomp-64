#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f a; Vec3f b; } Vec3fPair;
typedef struct {
    u8 pad0[0x10];
    f32 x10;
    u8 pad14[0xA4 - 0x14];
    Vec3f xA4;
    Vec3f xB0;
    Vec3f xBC;
    u8 padC8[0x104 - 0xC8];
    u8 x104[4];
} Phys;
typedef struct {
    u8 pad0[0x1EC];
    Phys phys;
} Car;
typedef struct { u8 pad0[0x10]; s32 x10; } Race;
typedef struct { u8 pad0[0x1100]; Race *race; } Game;
typedef struct {
    f32 x0, x4, x8;
    u8 padC[4];
    Vec3f x10;
    f32 x1C;
    Vec3f x20;
    Vec3f x2C;
    f32 x38;
    Vec3f x3C;
    Vec3f x48;
    f32 x54;
} Hit;
extern Game *gGame;

void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
void Vec3f_Length(f32 *, Vec3f *);
void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
void Tensor_MulVec3f(Vec3f *, void *, Vec3f *);
void func_80035328(Vec3fPair *, Car *, Hit *);
void Car_OnCollision2(void *, Vec3f, void *, Vec3f, f32);
f32 fabsf(f32);

void Car_ApplyCollisionImpulse(Car *a, Car *b, Hit *hit) {
    Vec3fPair r;
    Vec3f t;
    Vec3f s;
    f32 f;
    Phys *pa;
    Phys *pb;

    Vec3f_Cross(&t, &hit->x2C, &hit->x10);
    Tensor_MulVec3f(&t, a->phys.x104, &t);
    Vec3f_Cross(&t, &t, &hit->x2C);
    Vec3f_Dot(&f, &t, &hit->x10);
    f = fabsf(f);
    hit->x38 = a->phys.x10 + f;
    Vec3f_Cross(&t, &hit->x48, &hit->x10);
    Tensor_MulVec3f(&t, b->phys.x104, &t);
    Vec3f_Cross(&t, &t, &hit->x48);
    Vec3f_Dot(&f, &t, &hit->x10);
    f = fabsf(f);
    hit->x54 = b->phys.x10 + f;
    Vec3f_Scale(&s, &hit->x10, hit->x1C);
    func_80035328(&r, a, hit);
    Vec3f_Length(&f, &r.a);
    pb = &b->phys;
    pa = &a->phys;
    Car_OnCollision2(pa, hit->x2C, pb, hit->x48, f);
    if (gGame->race->x10 == 0) {
        Vec3f_Scale(&r.a, &r.a, 0.9f);
        r.a.y = r.a.y * 0.5;
        if (r.a.y > 4.0) {
            r.a.y = 4.0f;
        }
        if (r.a.y + a->phys.xB0.y > 6.0 && a->phys.xB0.y <= 6.0) {
            r.a.y = a->phys.xB0.y - 6.0;
        }
    }
    Vec3f_Scale(&r.a, &r.a, 0.8f);
    Vec3f_Scale(&r.b, &r.b, 0.7f);
    Vec3f_Add(&pa->xA4, &pa->xA4, &s);
    Vec3f_Add(&pa->xB0, &pa->xB0, &r.a);
    Vec3f_Add(&pa->xBC, &pa->xBC, &r.b);

    hit->x10.x = -hit->x10.x;
    hit->x10.y = -hit->x10.y;
    hit->x10.z = -hit->x10.z;
    t = hit->x2C;
    hit->x2C = hit->x48;
    hit->x48 = t;
    t = hit->x20;
    hit->x20 = hit->x3C;
    hit->x3C = t;
    Vec3f_Scale(&s, &hit->x10, hit->x1C);
    func_80035328(&r, b, hit);
    Vec3f_Length(&f, &r.a);
    Car_OnCollision2(pb, hit->x48, pa, hit->x2C, f);
    if (gGame->race->x10 == 0) {
        Vec3f_Scale(&r.a, &r.a, 0.9f);
        r.a.y = r.a.y * 0.5;
        if (r.a.y > 4.0) {
            r.a.y = 4.0f;
        }
        if (r.a.y + pb->xB0.y > 6.0 && pb->xB0.y <= 6.0) {
            r.a.y = pb->xB0.y - 6.0;
        }
    }
    Vec3f_Scale(&r.a, &r.a, 0.8f);
    Vec3f_Scale(&r.b, &r.b, 0.7f);
    Vec3f_Add(&pb->xA4, &pb->xA4, &s);
    Vec3f_Add(&pb->xB0, &pb->xB0, &r.a);
    Vec3f_Add(&pb->xBC, &pb->xBC, &r.b);
}
