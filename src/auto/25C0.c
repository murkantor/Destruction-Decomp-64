#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x24]; s32 x24; } Ctrl;
typedef struct { u8 pad[0x28]; s32 x28; u8 pad2[4]; s32 x30; u8 pad3[4]; Ctrl *x38; u8 pad4[0x1C]; s32 x58; } Car;
typedef struct { u8 pad[0x74]; Car *x74[RACE_CARS]; s32 xA4; } Level;
typedef struct { u8 pad[0x1100]; Level *level; } Game;
extern Game *gGame;
#ifdef TARGET_PC
#include "port_sret.h"
#else
void Car_GetPos(Vec3f *out, Car *c);
void func_8001FC08(Vec3f *out, Car *c);
#endif

static inline f32 cross_y(Vec3f a, Vec3f b) {
    return a.z * b.x - a.x * b.z;
}

s32 Ai_AdjustLaneFromNeighbour(Car *car) {
    s32 i;
    for (i = 0; i < gGame->level->xA4; i++) {
        Car *c = gGame->level->x74[i];
        if (c->x30 == 0 && car->x28 == c->x28) {
            Ctrl *ctrl = car->x38;
            Vec3f d, p;
            f32 sgn;
            Car_GetPos(&p, car);
            Car_GetPos(&d, gGame->level->x74[i]);
            d.x -= p.x;
            d.y -= p.y;
            d.z -= p.z;
            func_8001FC08(&p, car);
            sgn = ((f64)cross_y(p, d) < 0.0) ? -1.0f : 1.0f;
            if (sgn == (f32)gGame->level->x74[i]->x58) {
                ctrl->x24 -= 2;
            } else {
                ctrl->x24 += 2;
            }
            if (ctrl->x24 >= 20) ctrl->x24 = 19;
            if (ctrl->x24 < 0) ctrl->x24 = 0;
            return 1;
        }
    }
    return 0;
}
