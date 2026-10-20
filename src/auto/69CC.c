#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { f32 x0, x4, x8, xC; u8 x10; } Target;
typedef struct Car Car;
typedef struct {
    u8 pad[0x10];
    u32 x10;
    u8 pad14[4];
    Car *x18;
    u8 pad1C[0x5C - 0x1C];
    Vec3f x5C;
    f32 x68;
    u8 x6C;
    u8 x6D;
} Ai;
struct Car { u8 pad[0x38]; Ai *x38; };

extern s32 D_800958A8;
extern s32 D_800958AC;
extern Car **D_80096074;
extern void Ai_PickTarget2(Car *, Target *);
extern void AI_PickAttackTarget(Car *);
extern void func_8000663C(Car *);
extern void func_80000AC8(Car *, Car *, Target *);
extern Vec3f Car_GetPos(Car *);
extern Vec3f Car_GetVelocity(Car *);
extern Vec3f func_8001FC08(Car *);
extern f32 func_800005C8(Vec3f, Vec3f);
extern void Car_UpdateTrackSection(Car *);
extern void Ai_UpdateLaneOffset(Car *);
extern void Ai_UpdateTargetPoint(Car *);
extern void Ai_ConstrainTarget(Car *);
extern void Ai_Steer(Car *);

static inline void aim(Car *car, Target t) {
    Ai *ai;
    Vec3f p;
    Vec3f q;
    Vec3f d;

    ai = car->x38;
    if (t.xC >= 3.0) {
        t.xC = 2.0f;
    }
    p = Car_GetPos(ai->x18);
    q = Car_GetVelocity(ai->x18);
    ai->x5C.x = p.x + q.x * t.xC;
    ai->x5C.y = p.y + q.y * t.xC;
    ai->x5C.z = p.z + q.z * t.xC;
    if (t.x10) {
        ai->x6C = 1;
    }
    q = func_8001FC08(car);
    p = Car_GetPos(car);
    d.x = ai->x5C.x - p.x;
    d.y = ai->x5C.y - p.y;
    d.z = ai->x5C.z - p.z;
    ai->x68 = func_800005C8(q, d);
}

void Ai_Update(Car *car) {
    Target t = { -1.0f, -1.0f, -1.0f, -1.0f, 0 };
    Ai *ai = car->x38;

    switch (ai->x10) {
    case 0:
        Ai_PickTarget2(car, &t);
        goto do_aim;
    case 1:
    case 3:
        AI_PickAttackTarget(car);
        break;
    case 4:
        if (D_800958A8 == 3 && D_800958AC == 1 && *D_80096074 != car) {
            AI_PickAttackTarget(car);
            func_8000663C(car);
            break;
        }
        if (t.x0 == -1.0) {
            func_80000AC8(car, ai->x18, &t);
        }
        aim(car, t);
        func_8000663C(car);
        break;
    case 5:
        ai->x6D = 1;
        if (D_800958A8 == 3 && D_800958AC == 1) {
            ai->x18 = *D_80096074;
        }
        func_80000AC8(car, ai->x18, &t);
        aim(car, t);
        ai->x6D = 1;
        ai->x6C = 0;
        break;
    case 8:
        Car_UpdateTrackSection(car);
        Ai_UpdateLaneOffset(car);
        ai->x10 = 7;
        break;
    case 10:
        func_80000AC8(car, ai->x18, &t);
    case 2:
    do_aim:
        aim(car, t);
        break;
    case 11:
        Ai_UpdateTargetPoint(car);
        ai->x6D = 1;
        break;
    case 7:
    case 9:
    case 12:
        Ai_UpdateLaneOffset(car);
        break;
    case 6:
        break;
    }
    if (ai->x10 < 7) {
        Ai_ConstrainTarget(car);
    }
    Ai_Steer(car);
}
