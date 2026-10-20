#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { s32 x0; s32 x4; } Node;
typedef struct { u8 pad[0x10]; s32 x10; u8 pad14[0x34 - 0x14]; Node *x34; } Race;
typedef struct { u8 pad[0x1100]; Race *race; } Game;
typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[0x5C - 0x14];
    Vec3f x5C;
    u8 pad68[0x6D - 0x68];
    u8 x6D;
} Ai;
typedef struct { u8 accel; u8 brake; u8 x2; u8 x3; s16 x4; } Input;
typedef struct {
    u8 pad0[0x28];
    s32 x28;
    Input *x2C;
    u8 pad30[0x38 - 0x30];
    Ai *x38;
} Car;
extern Game *gGame;
Vec3f Car_GetPos(Car *);
Vec3f func_8001FC08(Car *);
f32 Vec3f_AngleBetween(Vec3f, Vec3f);
f32 func_800005C8(Vec3f, Vec3f);
f32 Car_GetSpeed(Car *);

void Ai_Steer(Car *car) {
    Ai *ai = car->x38;
    Race *race = gGame->race;
    Vec3f d;
    Vec3f f;
    Vec3f pos;
    Vec3f fwd;
    f32 ang;
    s32 steer;
    s32 accel, brake;
    f32 spd;
    s32 lim;
    Input *in;

    pos = Car_GetPos(car);
    fwd = func_8001FC08(car);
    accel = 1;
    brake = 0;
    if (ai->x6D) {
        fwd.x = -fwd.x;
        fwd.y = -fwd.y;
        fwd.z = -fwd.z;
    }
    d.x = ai->x5C.x - pos.x;
    d.y = 0.0f;
    d.z = ai->x5C.z - pos.z;
    f.x = fwd.x;
    f.y = 0.0f;
    f.z = fwd.z;
    ang = Vec3f_AngleBetween(f, d);
    if (func_800005C8(f, d) > 0.0f) {
        ang = -ang;
    }
    ang *= 12.0f;
    if (ang > 61.0) {
        ang = 61.0f;
    }
    if (ang < -61.0) {
        ang = -61.0f;
    }
    steer = ang;
    if (race->x10 == 0) {
        spd = Car_GetSpeed(car);
        lim = race->x34[car->x28].x0;
        if (lim < spd) {
            brake = 1;
            accel = 0;
        }
        if (ai->x10 == 12 && lim - 5 < spd) {
            brake = 1;
            accel = 0;
        }
    }
    in = car->x2C;
    in->accel = accel;
    in->brake = brake;
    if (ai->x6D) {
        in->x3 = ai->x6D;
        if (race->x10 == 0) {
            in->x4 = 0;
        } else {
            in->x4 = -steer;
        }
    } else {
        in->x3 = 0;
        in->x4 = steer;
    }
}
