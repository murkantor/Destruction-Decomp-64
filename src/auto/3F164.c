#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3;
typedef struct {
    u8 pad0[0x2B4];
    Vec3 v[3];
} Car;
typedef struct {
    s16 x0;
    u8 pad2[0xA];
    Car *xC;
    s8 x10;
} Ev;
extern Vec3 D_801242C0[][3];
void Mtx3_RotateY(Vec3 *, f32);

void func_8003E564(Ev *ev) {
    Car *car;

    if (ev->x0 == 0x18) {
        car = ev->xC;
        Mtx3_RotateY(car->v, 0.013962634f);
        D_801242C0[ev->x10][0] = car->v[0];
        D_801242C0[ev->x10][1] = car->v[1];
        D_801242C0[ev->x10][2] = car->v[2];
    }
}
