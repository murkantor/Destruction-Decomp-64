#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct {
    u8 pad0[0x2C];
    u8 x2C[0x78];
    Vec3f pos;
    Vec3f xB0;
    u8 padBC[0xC];
    u8 xC8[0x58];
    f32 x120;
    u8 pad124[0xC];
    u8 x130[0x4];
} Phys;
typedef struct {
    u8 pad0[0xC];
    u8 xC;
    u8 padD[0x1DF];
    union {
        Phys p;
        struct {
            u8 pad0[0x2C];
            u8 x218[0x78];
            Vec3f pos;
            Vec3f x29C;
            u8 pad2A8[0x64];
            f32 x30C;
        } d;
    } u;
    u8 pad[0x88C];
    u8 xBAC;
} Car;
typedef struct { u8 pad0[0x74]; Car *cars[12]; s32 numCars; } Race;
typedef struct { u8 pad0[0x1100]; Race *race; } Game;
typedef struct {
    f32 x0, x4, x8;
    u8 padC[4];
    Vec3f x10;
    f32 x1C;
    Vec3f x20;
    Vec3f x2C;
    u8 pad38[4];
    Vec3f x3C;
    Vec3f x48;
    u8 pad54[8];
} Hit;
extern Game *gGame;


void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Normalize(Vec3f *, Vec3f *);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
u8 func_80035E78(void *, Vec3f *, void *, Vec3f *, f32, s32, f32, f32 *, Vec3f *, Vec3f *, Vec3f *);
void Car_ApplyCollisionImpulse(Car *, Car *, Hit *);
u8 func_80033168(void *, void *, Vec3f *, void *, void *, Vec3f *);
void func_8003225C(Car *, Car *);

void func_80031EC0(Car *a, s32 idx) {
    Vec3f posA;
    Vec3f posB;
    Vec3f d;
    Vec3f rel;
    f32 dot[2];
    Vec3f outA;
    Vec3f outB;
    Vec3f contact;
    f32 depth[2];
    Hit hit;
    s32 i;
    Car *b;
    f32 r;
    f32 r2;
    f32 dist2;
    Phys *pa = &a->u.p;
    Phys *pb;
    Vec3f *aPos;
    u8 *aX;
    Vec3f *bPos;
    u8 *bX;

    posA = a->u.d.pos;
    aPos = &a->u.d.pos;
    aX = a->u.d.x218;
    for (i = idx + 1; i < gGame->race->numCars; i++) {
        b = gGame->race->cars[i];
        if (b->xBAC) {
            continue;
        }
        pb = &b->u.p;
        posB = b->u.d.pos;
        r = pa->x120 + b->u.d.x30C;
        r2 = r * r;
        bPos = &b->u.d.pos;
        bX = b->u.d.x218;
        if (!b->xC) {
            continue;
        }
        d.x = posB.x - posA.x;
        d.y = posB.y - posA.y;
        d.z = posB.z - posA.z;
        dist2 = d.x * d.x + d.y * d.y + d.z * d.z;
        if (dist2 < 900.0) {
            Vec3f_Sub(&rel, &a->u.d.x29C, &b->u.d.x29C);
            Vec3f_Dot(dot, &rel, &rel);
            if (900.0 < dot[0]) {
                if (func_80035E78(aX, aPos, bX, bPos, 1.05f, 0, 1.0f, depth, &outA, &outB, &contact)) {
                    pa->pos = outA;
                    b->u.d.pos = outB;
                    contact.y += 0.05;
                    hit.x1C = 1.0f;
                    hit.x0 = 0.5f;
                    hit.x4 = 0.1f;
                    hit.x8 = 0.1f;
                    Vec3f_Sub(&hit.x10, &b->u.d.x29C, &a->u.d.x29C);
                    Vec3f_Normalize(&hit.x10, &hit.x10);
                    Vec3f_Sub(&hit.x2C, &contact, &outA);
                    Vec3f_Sub(&hit.x48, &contact, &outB);
                    hit.x20 = pa->xB0;
                    hit.x3C = b->u.d.x29C;
                    Car_ApplyCollisionImpulse(a, b, &hit);
                }
            }
        }
        if (dist2 < r2) {
            if (func_80033168(pa->x130, pa->xC8, &pa->pos, pb->x130, pb->xC8, &pb->pos)) {
                func_8003225C(a, b);
            }
        }
    }
}
