#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0xC];
    u8 xC;
    u8 padD[0x27];
    s32 x34;
} Car;
typedef struct Link {
    Car *x0;
    struct Link *x4;
} Link;
typedef struct {
    u8 pad0[0x10];
    s32 x10;
    Link *x14;
    u8 pad18[8];
    Link *x20;
    u8 pad24[0x80 + 4 * (RACE_CARS - 12)]; /* the car list (0x74), then its count */
    s32 xA4;
} Cfg;
typedef struct {
    u8 pad0[0x1100];
    Cfg *x1100;
} Game;
extern Game *gGame;
extern Car *D_800958F0;

static inline s8 func_80027F7C_find(Link *l, Cfg *cfg) {
    Car *me = D_800958F0;
    s8 i;

    for (i = 0; i < cfg->xA4; i++) {
        if (l->x0->xC) {
            if (l->x0->x34 == me->x34) {
                return i + 1;
            }
            l = l->x4;
        }
    }
    return 99;
}

s8 Race_GetPlayerPosition(void) {
    Cfg *cfg = gGame->x1100;

    if (cfg->x10 == 1) {
        return func_80027F7C_find(cfg->x14, cfg);
    } else {
        return func_80027F7C_find(cfg->x20, cfg);
    }
}
