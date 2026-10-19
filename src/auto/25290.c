#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xC]; u32 xC; u32 x10; u8 pad2[0xB0 - 0x14]; } Pad;
typedef struct { u8 pad[0x10]; s8 x10; } Player;
typedef struct { u8 x0; u8 x1; u8 x2; u8 x3; u8 x4; } Out;
typedef struct { u8 pad[0x10FC]; u16 x10FC; } Game;
extern Game *gGame;
extern s32 gGameTime;
extern s64 D_80095830;
s64 Time_Get(s32 *);

void func_80024690(Pad *pads, Player *pl, Out *out) {
    u32 a, b, c;

    a = pads[pl->x10].x10 & 0x1000;
    if (gGame->x10FC == 0x1B || gGame->x10FC == 0x1C) {
        b = pads[pl->x10].x10 & 8;
        c = pads[pl->x10].xC & 0x2000;
    } else {
        b = 0;
        c = 0;
    }
    out->x3 = 0;
    out->x0 = a != 0;
    out->x1 = b != 0;
    out->x2 = c != 0;
    out->x4 = 0;
    if (out->x3 || out->x2 || out->x1 || out->x0) {
        D_80095830 = Time_Get(&gGameTime);
    }
}
