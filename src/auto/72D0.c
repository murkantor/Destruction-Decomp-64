#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x18]; void *x18; u8 pad1C[0x30]; s32 x4C; } Sub066D0;
typedef struct { u8 pad[0x30]; s32 x30; u8 pad34[4]; Sub066D0 *x38; } Obj066D0;
typedef struct { u8 pad[8]; s32 x8; u8 padC[0x68]; Obj066D0 *x74[12]; s32 xA4; } Track066D0;
typedef struct { u8 pad[0x1100]; Track066D0 *x1100; } Game066D0;
extern Game066D0 *gGame;
void Ai_PickTarget(Obj066D0 *);

void func_800066D0(Obj066D0 *arg0) {
    Sub066D0 *sub;
    Track066D0 *track;
    Obj066D0 *obj;
    s32 i;
    u8 flag;

    sub = arg0->x38;
    track = gGame->x1100;
    flag = sub->x4C == 6;
    for (i = 0; i < gGame->x1100->xA4; i++) {
        obj = gGame->x1100->x74[i];
        sub = obj->x38;
        if (obj->x30 == 0 || obj == arg0) {
            continue;
        }
        if (sub->x18 == arg0) {
            Ai_PickTarget(obj);
        }
        if (track->x8 > 0) {
            if (flag && sub->x4C != 6) {
                sub->x4C = 6;
                Ai_PickTarget(obj);
            }
        }
    }
}
