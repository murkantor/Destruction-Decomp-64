#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10A8]; u64 x10A8; u8 pad10B0[0x4C]; s16 x10FC; } Game;
extern Game *gGame;
extern void *gUi[];
extern s16 D_8011EEEA;
void func_8005DC68(void *);
void func_80063564(s32, s32);

s32 func_8003B62C(void) {
    s32 r = 0;
    u64 lim;

    if (gGame->x10FC != D_8011EEEA) {
        return 0;
    }
    lim = 4000000000ULL;
    if (gGame->x10A8 >= lim) {
        func_8005DC68(gUi[0]);
        func_80063564(0, 1);
        r = 1;
    }
    return r;
}
