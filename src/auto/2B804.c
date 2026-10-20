#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; u8 pad14[4]; s32 x18; u8 pad1C[0x18]; s32 x34; } Car;
typedef struct {
    u8 pad0[4];
    s32 x4;
    s32 x8;
    u8 padC[0x68];
    Car *cars[RACE_CARS];
    s32 xA4;
} Race;
typedef struct { u8 pad[0x1100]; Race *x1100; } Game;

extern Game *gGame[];
extern s32 D_800958A8;
extern s32 D_800958AC;
extern s32 D_800958BC;
extern s32 D_80095D94[][3];

void Race_AwardHealthBonus(void);
void Results_TallyStandings(void);
s32 Race_FindCarSlot(s32);
int rand(void);

void Race_AwardPot(void) {
    Race *race;
    s32 i;
    s32 r;
    s32 p;

    Race_AwardHealthBonus();
    if (D_800958A8 == 3 && D_800958AC == 1) {
        race = gGame[0]->x1100;
        if (race->x4 > 0) {
            i = 0;
            r = rand() % race->x4;
            for (; i < gGame[0]->x1100->xA4; i++) {
                if (gGame[0]->x1100->cars[i]->x10 > 0) {
                    p = Race_FindCarSlot(gGame[0]->x1100->cars[i]->x34);
                    gGame[0]->x1100->cars[i]->x18 += race->x4 * 33;
                    race->x4--;
                    if (r == 0) {
                        gGame[0]->x1100->cars[i]->x18 += 50;
                        D_80095D94[p][D_800958BC] = 50;
                    }
                    r--;
                }
            }
        }
        if (race->x8 > 0) {
            for (i = 0; i < gGame[0]->x1100->xA4; i++) {
                if (gGame[0]->x1100->cars[i]->x10 > 0) {
                    p = Race_FindCarSlot(gGame[0]->x1100->cars[i]->x34);
                    gGame[0]->x1100->cars[i]->x18 += 50;
                    D_80095D94[p][D_800958BC] = 50;
                }
            }
        }
    }
    Results_TallyStandings();
}
