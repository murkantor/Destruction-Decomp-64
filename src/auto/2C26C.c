#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xD]; u8 xD; u8 padE[2]; s32 x10; u8 pad14[4]; s32 x18; u8 pad1C[0x3C - 0x1C]; s32 x3C; } Car;
typedef struct { u8 pad[0x14]; u8 x14[0x74 - 0x14]; Car *x74[RACE_CARS]; s32 xA4; } Race;
typedef struct { u8 pad[0x1100]; Race *x1100; } Game;
extern Game *gGame[];
extern s32 D_800958A8;
extern s32 D_800958AC;
extern s32 D_800958A4;
void Standings_Resort(void *, Car *);

void Race_AwardHealthBonus(void) {
    Race *r = gGame[0]->x1100;
    s32 i;
    Car *c;

    if (D_800958A8 == 3 && (u32)(D_800958AC - 1) < 2) {
        return;
    }
    for (i = 0; i < gGame[0]->x1100->xA4; i++) {
        c = gGame[0]->x1100->x74[i];
        if (c->xD == 0 && c->x3C == 0 && c->x10 > 0) {
            c->x18 += c->x10;
            Standings_Resort(r->x14, gGame[0]->x1100->x74[i]);
        }
    }
    for (i = 0; i < D_800958A4; i++) {
        Standings_Resort(r->x14, gGame[0]->x1100->x74[i]);
    }
}
