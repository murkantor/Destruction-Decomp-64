#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

typedef struct {
    Vec3f a;
    Vec3f b;
} Edge;

typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[0x14];
    s32 x28;
    u8 pad2C[4];
    s32 x30;
    u8 pad34[0x2C];
    u64 x60;
    u64 x68[4];
    s32 x88;
} Car;

typedef struct {
    u8 pad0[0x30];
    Edge *x30;
    u8 pad34[4];
    s32 x38;
    u8 pad3C[0x38];
    Car *cars[12];
    s32 numCars;
} Race;

typedef struct {
    u8 pad0[0x10A8];
    u64 x10A8;
    u8 pad10B0[0x50];
    Race *race;
} Game;

extern Game *gGame;
extern s32 D_800958B4[];
extern s32 D_80095FF8[];

f32 Vec3f_DistXZ(Vec3f a, Vec3f b);
f32 Car_GetSpeed(Car *);
u64 Time_Get(u64 *);

void Race_EstimateFinishTimes(void) {
    s32 i;
    s32 j;
    Car *car;
    Race *race;
    Edge *e;
    f32 dist = 0.0f;
    f32 speed;
    u64 t;
    u64 t2;
    Vec3f a, b;

    for (i = 0; i < gGame->race->numCars; i++) {
        car = gGame->race->cars[i];
        if (car->x88 != D_80095FF8[D_800958B4[0]] && car->x10 > 0 && car->x30 == 1) {
            race = gGame->race;
            j = car->x28; e = race->x30;
            for (; i < race->x38; i++) {
                a.x = e[j - 1].a.x + (e[j - 1].b.x - e[j - 1].a.x) * 0.5;
                a.z = e[j - 1].a.z + (e[j - 1].b.z - e[j - 1].a.z) * 0.5;
                b.x = e[j].a.x + (e[j].b.x - e[j].a.x) * 0.5;
                b.z = e[j].a.z + (e[j].b.z - e[j].a.z) * 0.5;
                dist += Vec3f_DistXZ(a, b);
            }
            speed = Car_GetSpeed(car);
            t = Time_Get(&gGame->x10A8) - car->x60;
            t2 = t + (dist / speed) * 1000000000.0;
            for (j = 0; j < D_80095FF8[D_800958B4[0]]; j++) {
                if (car->x68[j] == 0) {
                    car->x68[j] = t2;
                }
            }
            car->x88 = D_80095FF8[D_800958B4[0]];
        }
    }
}
