#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xD]; u8 xD; } Car;
typedef struct { u8 pad[0x74]; Car *x74[12]; s32 xA4; } Sub1100;
typedef struct { u8 pad[0x1100]; Sub1100 *x1100; } Game;
extern Game *gGame;
extern s32 rand(void);
Car *Car_PickRandomOther(Car *exclude) {
    s32 idx[12];
    s32 i;
    s32 n = 0;
    for (i = 0; i < gGame->x1100->xA4; i++) {
        Car *c = gGame->x1100->x74[i];
        if (c->xD == 0) {
            if (c != exclude) {
                idx[n++] = i;
            }
        }
    }
    return gGame->x1100->x74[idx[rand() % n]];
}
