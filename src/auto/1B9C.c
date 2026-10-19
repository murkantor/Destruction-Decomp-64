#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct Car Car;
typedef struct {
    u8 pad0[0x18];
    Car *x18;
    s32 x1C;
    u8 pad20[0x2C];
    s32 x4C;
    s32 x50;
    u8 pad54[0x18];
    u8 x6C;
} Ai;
struct Car {
    u8 pad0[0xD];
    u8 xD;
    u8 padE[0x2A];
    Ai *x38;
};
typedef struct { u8 pad0[0x74]; Car *cars[12]; s32 xA4; } Race;
typedef struct { u8 pad[0x1100]; Race *x1100; } Game;

extern Game *gGame;
extern f64 D_800B7D80;
extern f64 D_800B7D88;

Car *Obj_PickMinKey10(Car *, Car *);
Car *Obj_MaxByKey10(Car *, Car *);
Car *Obj_PickMaxKey18(Car *, Car *);
Car *Obj_MaxByKey24(Car *, Car *);
Car *Obj_PickNearer(Car *, Car *, Car *);
Car *Car_PickRandomOther(Car *);
Car *Car_PickRandomActive(void);
Car *func_80001668(void);
Vec3f func_8001FBA8(Car *);
Vec3f func_8001FC08(Car *);
f64 acos(f64);
f64 fabs(f64);
f32 sqrtf(f32);

static inline f32 AngleBetween(Vec3f a, Vec3f b) {
    f32 dot = a.x * b.x + a.y * b.y + a.z * b.z;

    if (fabs(dot) < D_800B7D80) {
        return 0.0f;
    }
    return acos(dot / sqrtf((a.x * a.x + a.y * a.y + a.z * a.z) * (b.x * b.x + b.y * b.y + b.z * b.z))) * D_800B7D88;
}

void Ai_PickTarget(Car *car) {
    Ai *ai;
    s32 mode;
    s32 i;
    Car *target;
    Car *other;
    Vec3f d;
    Vec3f cp;
    Vec3f tp;

    i = 0;
    ai = car->x38;
    mode = ai->x4C;
    target = NULL;
    for (; i < gGame->x1100->xA4; i++) {
        other = gGame->x1100->cars[i];
        if (other->xD != 0 || other == car) {
            continue;
        }
        switch (mode) {
            case 0:
                target = Obj_PickMinKey10(other, target);
                break;
            case 1:
                target = Obj_MaxByKey10(other, target);
                break;
            case 2:
                target = Obj_PickMaxKey18(other, target);
                break;
            case 3:
                target = Obj_MaxByKey24(other, target);
                break;
            case 4:
                target = Obj_PickNearer(car, other, target);
                break;
            case 5:
                target = Car_PickRandomOther(car);
                break;
            case 6:
                target = Car_PickRandomActive();
                break;
            case 7:
                target = func_80001668();
                break;
        }
        if (mode == 5 || mode == 6) {
            break;
        }
    }
    if (target == NULL) {
        target = Car_PickRandomOther(car);
    }
    ai->x18 = target;
    ai->x1C = ai->x50;
    tp = func_8001FBA8(target);
    cp = func_8001FBA8(car);
    d.x = tp.x - cp.x;
    d.y = tp.x - cp.y;
    d.z = tp.x - cp.z;
    if (90.0f < AngleBetween(func_8001FC08(car), d)) {
        ai->x6C = 1;
    }
}
