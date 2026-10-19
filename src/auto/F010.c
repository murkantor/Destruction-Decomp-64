#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; u8 pad4[4]; s32 x8; s32 xC; s8 x10; s8 x11; s8 x12; u8 pad13[5]; u64 x18; } Bar;
typedef struct { u8 pad[0x10A8]; u64 x10A8; } Game;
extern Game *gGame;
extern u64 D_8007EF40;
u64 Time_Get(u64 *);

s32 func_8000E410(Bar *b) {
    s32 d = (Time_Get(&gGame->x10A8) - b->x18) / D_8007EF40;
    s8 v;

    {
        f32 total = b->x8;

        v = b->x12 = b->xC / total * b->x0;
    }
    if (b->x11 - d < v) {
        d = b->x11 - v;
    }
    if (b->x10 >= v && b->xC != 0) {
        b->x12 = b->x10;
        if (b->x11 <= b->x10) {
            b->x11 = b->x10 + 1;
        }
    }
    b->x11 -= d;
    return d;
}
