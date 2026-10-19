#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xBE4]; u8 xBE4; } Car;
typedef struct { u8 pad[0x20]; Car *x20; } Driver;
typedef struct { s32 x0; s32 x4; u8 pad8[0x43]; u8 x4B; } Replay;
typedef struct { u8 pad[0xB0]; Driver *xB0; u8 xB4; } Obj;
extern u8 D_80095628;
extern Replay *D_8009562C;

static inline s32 Car_Get(Obj *o, Car *c) {
    if (o->xB4 == 9 && D_8009562C != NULL && D_8009562C->x0 == 0 && D_8009562C->x4 == 0) {
        return D_8009562C->x4B;
    }
    return c->xBE4;
}

u8 func_800232DC(Obj *o) {
    Car *c = o->xB0->x20;
    u8 r;

    if (D_80095628) {
        r = Car_Get(o, c);
    } else {
        r = 0;
    }
    return r;
}
