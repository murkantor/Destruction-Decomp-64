#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct {
    u8 pad0[0x290];
    u8 x290[0x29C - 0x290];
    Vec3f x29C;
    u8 pad2A8[0x2B4 - 0x2A8];
    u8 x2B4[0x31C - 0x2B4];
    u8 x31C[4];
} Car;
typedef struct { u8 pad[0x74]; Car *cars[12]; s32 xA4; } CarList;
typedef struct { u8 pad[0x1100]; CarList *x1100; } Game;
typedef struct {
    s32 x0;
    f32 x4;
    u8 x8[0x38 - 0x8];
    Vec3f x38;
    f32 x44;
    f32 x48;
    f32 x4C;
} Trigger;

extern Game *gGame;
u8 Obb_OverlapsSphere(void *, void *, void *, f32, void *);

void Trigger_CheckCarsInZone(Trigger *t) {
    s32 i;
    Car *c;

    for (i = 0; i < gGame->x1100->xA4; i++) {
        c = gGame->x1100->cars[i];
        if (Obb_OverlapsSphere(c->x31C, c->x2B4, c->x290, t->x4, t->x8)) {
            t->x0 = 0;
            t->x38 = c->x29C;
            t->x38.y += 2.0f;
            t->x44 = 1.0f;
            t->x48 = 1.0f;
            t->x4C = 1.0f;
        }
    }
}
