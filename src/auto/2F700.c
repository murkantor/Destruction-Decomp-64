#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f a; Vec3f b; } Seg;
typedef struct {
    Vec3f pos;
    Vec3f vel;
    Vec3f angVel;
    Vec3f bx;
    Vec3f by;
    Vec3f bz;
    u8 pad48[0x30];
} State;
typedef struct {
    u8 pad0[0x2C];
    State s0;
    State s1;
    u8 pad11C[0x1BC - 0x11C];
} Body;
typedef struct { u8 b[0x180]; } Buf180;
typedef struct Car {
    s16 x0;
    u8 pad2[0xA];
    u8 xC;
    u8 padD[0x1B];
    s32 x28;
    u8 pad2C[0xC];
    void *x38;
    u8 pad3C[0x1C];
    s32 x58;
    u8 pad5C[0x1EC - 0x5C];
    union {
        Body body;
        struct {
            u8 pad0[0x218 - 0x1EC];
            Vec3f x218;
            u8 pad224[0x23C - 0x224];
            Vec3f x23C;
            u8 pad248[0x24C - 0x248];
            f32 x24C;
            u8 pad250[0x290 - 0x250];
            Vec3f x290;
            u8 pad29C[0x2B4 - 0x29C];
            Vec3f x2B4;
            u8 pad2C0[0x330 - 0x2C0];
            Buf180 *x330;
            u8 pad334[0x348 - 0x334];
            Buf180 *x348;
            u8 pad34C[0x364 - 0x34C];
            Body wheels[4];
            u8 padA54[0xA64 - 0xA54];
            f32 xA64;
            u8 padA68[0xBAC - 0xA68];
            u8 xBAC;
            u8 padBAD[0xBE5 - 0xBAD];
            u8 xBE5;
        } f;
    } u;
} Car;
typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[0x1C];
    Seg *x30;
    u8 pad34[4];
    s32 x38;
    u8 pad3C[0x38];
    Car *x74[RACE_CARS];
    s32 xA4;
} Race;
typedef struct { u8 pad0[0x1100]; Race *x1100; } Game;
typedef struct {
    u8 pad0[0x40C];
    Car *x40C[(0xD12 - 0x40C) / 4];
    u8 padD10[2];
    s16 xD12;
} World;

extern Game *gGame;
extern Vec3f D_800B9318;
extern f32 D_800D2E78;
extern u8 D_800960C0;
extern s32 D_800958B4;
void Body_ClearContacts(Car *);
void Model_TransformVerts(Body *);
void Camera_CollideTrack(Car *);
void Trigger_CheckCarsInZone(void *);
void Team_CheckFlagPickup(void *);
void Team_CheckBaseZone(void *);
void Car_UpdateWheels(Car *);
void Car_CollideOtherCars(Car *, s32);
void Car_CollideTrack(Body *, void *, s32 *);
void Car_ApplyContactImpulses(Car *, Body *);
void Car_CheckTriggersAndPickups(Car *);
void func_8002E740(void);
void Collision_ResetLastPair(void);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
void Vec3f_Set(Vec3f *, f32, f32, f32);
f32 Vec3f_DistXZ(Vec3f, Vec3f);
void Basis_Orthonormalize(Vec3f *);
f64 fabs(f64);

void Car_PhysicsStep(void *w, f32 dt) {
    Buf180 buf1[12];
    Buf180 buf2[12];
    u8 buf3[0x12C0];
    s32 cnt;
    f32 dot;
    s32 i;
    s32 k;
    s32 k2;
    Car *c;
    Car *car;
    Body *b;
    s32 n;
    f32 floorY;
    f32 len;

    cnt = 0;
    D_800D2E78 = dt;
    n = 0;
    for (i = 0; i < ((World *)w)->xD12; i++) {
        c = ((World *)w)->x40C[i];
        switch (c->x0) {
        case 3:
            b = &c->u.body;
            if (!c->u.f.xBAC) {
                Body_ClearContacts(c);
                c->u.f.x330 = &buf1[n];
                c->u.f.x348 = &buf2[n];
                n++;
                Model_TransformVerts(b);
            }
            break;
        case 4:
            break;
        case 5:
            break;
        }
    }
    for (i = 0; i < ((World *)w)->xD12; i++) {
        Car *c2 = ((World *)w)->x40C[i];
        switch (c2->x0) {
        case 9:
            Camera_CollideTrack(c2);
            break;
        case 6:
            Trigger_CheckCarsInZone(c2->x38);
            break;
        case 15:
        case 17:
            Team_CheckFlagPickup(c2->x38);
            break;
        case 16:
        case 18:
            Team_CheckBaseZone(c2->x38);
            break;
        case 19:
            break;
        }
    }
    for (i = 0; i < gGame->x1100->xA4; i++) {
        c = gGame->x1100->x74[i];
        b = &c->u.body;
        if (!c->u.f.xBAC) {
            Car_UpdateWheels(c);
            if (c->xC) {
                Car_CollideOtherCars(c, i);
            }
            Car_CollideTrack(b, buf3, &cnt);
            Car_ApplyContactImpulses(c, b);
        }
        Car_CheckTriggersAndPickups(c);
    }
    func_8002E740();
    Collision_ResetLastPair();
    for (i = 0; i < gGame->x1100->xA4; i++) {
        car = gGame->x1100->x74[i];
        Vec3f_Dot(&dot, &car->u.body.s1.vel, &car->u.body.s1.vel);
        if (car->u.f.xBAC) {
            continue;
        }
        w = gGame->x1100;
        floorY = -30.0f;
        b = &car->u.body;
        if (((Race *)w)->x10 == 0) {
            floorY = ((Race *)w)->x30[car->x28].a.y - 90.0f;
        }
        if (car->u.f.x24C < 0.25 && dot < 900.0) {
            car->u.f.xA64 += dt;
            if (car->u.f.xA64 > 3.0f) {
                D_800960C0 = 1;
                car->u.f.xA64 = 0.0f;
            }
        } else {
            car->u.f.xA64 = 0.0f;
        }
        if (D_800960C0) {
            car->u.f.xBE5 = 1;
            Vec3f_Set(&b->s1.vel, 0.0f, 0.0f, 0.0f);
            Vec3f_Set(&b->s1.angVel, 0.0f, 0.0f, 0.0f);
            if (fabs(b->s1.bx.y) < 0.99) {
                if (b->s1.bz.y > 0.0f) {
                    b->s1.angVel.x += b->s1.bx.x * 7.5f;
                    b->s1.angVel.y += b->s1.bx.y * 7.5f;
                    b->s1.angVel.z += b->s1.bx.z * 7.5f;
                } else {
                    b->s1.angVel.x -= b->s1.bx.x * 7.5f;
                    b->s1.angVel.y -= b->s1.bx.y * 7.5f;
                    b->s1.angVel.z -= b->s1.bx.z * 7.5f;
                }
            } else {
                Vec3f_Set(&b->s1.bx, 1.0f, 0.0f, 0.0f);
                Vec3f_Set(&b->s1.by, 0.0f, 1.0f, 0.0f);
                Vec3f_Set(&b->s1.bz, 0.0f, 0.0f, 1.0f);
            }
            b->s0 = b->s1;
            for (k = 0; k < 4; k++) {
                Body *wb = &car->u.f.wheels[k];
                wb->s1.pos.y = 0.0f;
                Vec3f_Set(&wb->s1.vel, 0.0f, 0.0f, 0.0f);
                Vec3f_Set(&wb->s1.angVel, 0.0f, 0.0f, 0.0f);
                Vec3f_Set(&wb->s1.bx, 1.0f, 0.0f, 0.0f);
                Vec3f_Set(&wb->s1.by, 0.0f, 1.0f, 0.0f);
                Vec3f_Set(&wb->s1.bz, 0.0f, 0.0f, 1.0f);
                wb->s0 = wb->s1;
            }
            D_800960C0 = 0;
        }
        if (b->s0.pos.y < floorY) {
            if (((Race *)w)->x10 == 0) {
                s32 seg = car->x28;
                s32 next = seg + car->x58;
                Vec3f p0 = D_800B9318;
                Vec3f p1 = D_800B9318;
                Vec3f d = D_800B9318;
                if (next >= ((Race *)w)->x38) {
                    next -= ((Race *)w)->x38;
                } else if (next < 0) {
                    next += ((Race *)w)->x38;
                }
                p0.x = (((Race *)w)->x30[seg].a.x + ((Race *)w)->x30[seg].b.x) * 0.5;
                p0.y = (((Race *)w)->x30[seg].a.y + ((Race *)w)->x30[seg].b.y) * 0.5 + 1.0;
                p0.z = (((Race *)w)->x30[seg].a.z + ((Race *)w)->x30[seg].b.z) * 0.5;
                p1.x = (((Race *)w)->x30[next].a.x + ((Race *)w)->x30[next].b.x) * 0.5;
                p1.y = (((Race *)w)->x30[next].a.y + ((Race *)w)->x30[next].b.y) * 0.5 + 1.0;
                p1.z = (((Race *)w)->x30[next].a.z + ((Race *)w)->x30[next].b.z) * 0.5;
                len = Vec3f_DistXZ(p0, p1);
                if (len < 1e-9) {
                    len = 1e-9f;
                }
                p0.x = p0.x + (p1.x - p0.x) / len * 0.5;
                p0.y = p0.y + (p1.y - p0.y) / len * 0.5;
                p0.z = p0.z + (p1.z - p0.z) / len * 0.5;
                car->u.f.x218 = p0;
                car->u.f.x290 = p0;
                d.x = p1.x - p0.x;
                d.y = p1.y - p0.y;
                d.z = p1.z - p0.z;
                Vec3f_Set(&b->s1.bx, 1.0f, 0.0f, 0.0f);
                Vec3f_Set(&b->s1.by, 0.0f, 1.0f, 0.0f);
                Vec3f_Set(&b->s1.bz, 0.0f, 0.0f, 1.0f);
                Vec3f_Set(&b->s1.vel, 0.0f, 0.0f, 0.0f);
                Vec3f_Set(&b->s1.angVel, 0.0f, 0.0f, 0.0f);
                car->u.f.x23C = d;
                Basis_Orthonormalize(&car->u.f.x23C);
                car->u.f.x2B4 = d;
                Basis_Orthonormalize(&car->u.f.x2B4);
            } else {
                if (D_800958B4 == 2) {
                    Vec3f_Set(&b->s1.pos, 10.0f, 12.0f, 10.0f);
                } else {
                    Vec3f_Set(&b->s1.pos, 0.0f, 12.0f, 0.0f);
                }
                Vec3f_Set(&b->s1.bx, 1.0f, 0.0f, 0.0f);
                Vec3f_Set(&b->s1.by, 0.0f, 1.0f, 0.0f);
                Vec3f_Set(&b->s1.bz, 0.0f, 0.0f, 1.0f);
                Vec3f_Set(&b->s1.vel, 0.0f, 0.0f, 0.0f);
                Vec3f_Set(&b->s1.angVel, 0.0f, 0.0f, 0.0f);
            }
            b->s0 = b->s1;
            for (k2 = 0; k2 < 4; k2++) {
                Body *wb = &car->u.f.wheels[k2];
                wb->s1.pos.y = 0.0f;
                Vec3f_Set(&wb->s1.vel, 0.0f, 0.0f, 0.0f);
                Vec3f_Set(&wb->s1.angVel, 0.0f, 0.0f, 0.0f);
                Vec3f_Set(&wb->s1.bx, 1.0f, 0.0f, 0.0f);
                Vec3f_Set(&wb->s1.by, 0.0f, 1.0f, 0.0f);
                Vec3f_Set(&wb->s1.bz, 0.0f, 0.0f, 1.0f);
                wb->s0 = wb->s1;
            }
        }
    }
}
