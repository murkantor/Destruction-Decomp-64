#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x224]; Vec3f x224; u8 pad230[0xC]; Vec3f x23C; u8 pad248[0xBA8 - 0x248]; f32 xBA8; } Car;
typedef struct { u8 pad[0x10FC]; s16 x10FC; } Game;
extern Game *gGame;
void Vec3f_Dot(f32 *out, Vec3f *a, Vec3f *b);

s32 func_8001FA58(Car *car) {
    f32 d1;
    f32 d2;

    if (car == NULL) {
        return 0;
    }
    if (gGame->x10FC == 0x1B && car->xBA8 > 1010.0) {
        return 1;
    }
    Vec3f_Dot(&d1, &car->x224, &car->x224);
    if (d1 < 1.0) {
        return 0;
    }
    Vec3f_Dot(&d2, &car->x23C, &car->x224);
    if (d2 > 0.0f) {
        return 1;
    }
    return -1;
}
