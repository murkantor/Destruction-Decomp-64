#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x74]; void *x74[12]; s32 xA4; } Sub1100;
typedef struct { u8 pad[0x1100]; Sub1100 *x1100; } Game;
extern Game *gGame;
extern u8 func_800330A8(void *, void *, void *, s32, void *);
extern void func_8002A68C(void *, void *);
typedef struct { s32 x0; s32 x4; u8 x8[1]; } Arg;
void Cars_CheckCollisions(Arg *a) {
    s32 i;
    u8 *c;
    for (i = 0; i < gGame->x1100->xA4; i++) {
        c = gGame->x1100->x74[i];
        if (func_800330A8(c + 0x31C, c + 0x2B4, c + 0x290, a->x4, a->x8)) {
            func_8002A68C(a, c);
        }
    }
}
