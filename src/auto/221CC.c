#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x29C]; Vec3f x29C; } Body;
typedef struct { s16 x0; u8 pad[0x17]; u8 x19; u8 pad2[6]; Body *x20; } Res;
typedef struct { u8 pad[0x98]; Vec3f x98; u8 padA4[0xC]; Res *xB0; u8 xB4; } Car;
typedef struct { u8 pad[0xC]; s16 xC; } Game;
extern Game *gGame;
void Vec3f_Set(Vec3f *, f32, f32, f32);
void func_80021700(Car *, f32);

void func_800215CC(Car *car) {
    Vec3f saved;
    Vec3f *pos;
    s32 i;
    Res *r;
    s16 n;

    if (car != NULL && gGame->xC == 2) {
        r = car->xB0;
        n = r->x0;
        if (n == 1 && r->x19 != 0 && car->xB4 != 7) {
            car->xB4 = (car->xB4 + 1) % 3;
            if (car->xB4 == n) {
                pos = &car->xB0->x20->x29C;
                saved = *pos;
                Vec3f_Set(pos, 0.0f, 0.0f, 0.0f);
                for (i = 0; i < 60; i++) {
                    func_80021700(car, 0.06f);
                }
                *pos = saved;
                car->x98 = saved;
            }
        }
    }
}
