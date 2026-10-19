#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x30]; s32 x30; } Body;
void func_80004620(Body *, s32, Body *, s32);
void CarAI_OnCollision(Body *, s32, s32, Body *, s32, s32);

void func_800045B0(Body *a, s32 p1, s32 p2, Body *b, s32 p4, s32 p5) {
    if (a->x30 == 0) {
        if (b->x30 != 0) {
            func_80004620(a, p1, b, p5);
        }
    } else if (b->x30 == 0) {
        func_80004620(b, p4, a, p5);
    } else {
        CarAI_OnCollision(a, p1, p2, b, p4, p5);
    }
}
