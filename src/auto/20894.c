#include "common.h"
#include "m2c.h"

typedef struct { f32 x0; u8 pad4[0x1B8]; } Wheel;
typedef struct { u8 pad[0x500]; Wheel w[4]; } CarBody;

s32 func_8001FC94(CarBody *c) {
    s32 r = 0;
    s8 i;

    for (i = 0; i < 4; i++) {
        if (c->w[i].x0 < 1.0) {
            r = 1;
            break;
        }
    }
    return r;
}
