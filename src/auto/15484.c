#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; f32 x10; } Cam;
typedef struct { u8 pad[0x38]; Cam *x38; } Obj;
extern s32 D_80083DB0;

void func_80014884(Obj *o) {
    Cam *c = o->x38;

    c->x10 += D_80083DB0 * 0.005;
    if (c->x10 < -3.0) {
        D_80083DB0 = 1;
    }
    if (c->x10 > -0.6) {
        D_80083DB0 = -1;
    }
}
