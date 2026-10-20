#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 x0; u8 x1; u8 x2; u8 pad3; s16 x4; } Ai;
typedef struct {
    u8 pad0[0x28];
    s32 x28;
    u8 pad2C[0x30];
    f32 x5C;
    f32 x60;
    f32 x64;
    f32 x68;
    u8 pad6C;
    u8 x6D;
} Target;
typedef struct {
    u8 pad0[0x2C];
    Ai *x2C;
    u8 pad30[8];
    Target *x38;
    u8 pad3C[0xA5C - 0x3C];
    f32 xA5C;
    f32 xA60;
    f32 xA64;
} Car;
typedef struct { s32 x0; s32 x4; } Checkpoint;
typedef struct { u8 pad0[0x10]; s32 x10; u8 pad14[0x20]; Checkpoint *x34; } Race;
typedef struct { u8 pad[0x1100]; Race *x1100; } Game;

extern Game *gGame;
extern f64 D_800B7EF0;
extern f64 D_800B7EF8;
extern f64 D_800B7F00;
extern f64 D_800B7F08;

Vec3f Car_GetPos(Car *);
Vec3f func_8001FC08(Car *);
f32 Vec3f_AngleBetween(Vec3f, Vec3f);
f32 func_800005C8(Vec3f, Vec3f);
f32 Car_GetSpeed(Car *);
f32 fabsf(f32);

void Ai_UpdateSteer(Car *car) {
    Target *t = car->x38;
    Race *race;
    f32 f;

    if (car->x2C->x4 != 0) {
        Vec3f d;
        Vec3f pos;
        Vec3f fwd;
        f32 diff;
        f32 side;

        pos = Car_GetPos(car);
        fwd = func_8001FC08(car);
        if (t->x6D) {
            fwd.x = -fwd.x;
            fwd.z = -fwd.z;
        }
        d.x = t->x5C - pos.x;
        d.y = 0.0f;
        d.z = t->x64 - pos.z;
        fwd.y = 0.0f;
        diff = fabsf(Vec3f_AngleBetween(fwd, d) - (f32)(car->xA5C * D_800B7EF0));
        side = func_800005C8(fwd, d);
        if (diff < D_800B7EF8 || (t->x6D && D_800B7F00 < diff) || side != t->x68) {
            car->x2C->x4 = 0;
            car->x2C->x2 = 0;
            if (car->x2C->x1) {
                car->x2C->x1 = 0;
                car->x2C->x0 = 1;
            }
            t->x68 = 0.0f;
        } else if (diff < D_800B7F08 || ((t->x6D != 0) & (175.0f < diff))) {
            car->x2C->x4 /= 2;
        }
    }
    race = gGame->x1100;
    f = Car_GetSpeed(car);
    if (race->x10 == 0) {
        if ((f32)race->x34[t->x28].x0 < f) {
            car->x2C->x1 = 1;
            car->x2C->x0 = 0;
        } else {
            car->x2C->x1 = 0;
            car->x2C->x0 = 1;
        }
    }
    if (car->xA60 != 0.0f || car->xA64 != 0.0f) {
        car->x2C->x1 = 1;
        car->x2C->x0 = 0;
    }
}
