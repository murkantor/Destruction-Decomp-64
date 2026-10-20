#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10A8]; u64 x10A8; u8 pad10B0[0x4C]; s16 x10FC; } Game;
extern Game *gGame;
extern void *gUi[];
extern s16 D_8011EEEA;
void Ui_ClearDrawList(void *);
void Game_SetNextMode(s32, s32);

s32 Logo_N64Update(void) {
    s32 r = 0;
    u64 lim;

    if (gGame->x10FC != D_8011EEEA) {
        return 0;
    }
    lim = 4000000000ULL;
    if (gGame->x10A8 >= lim) {
        Ui_ClearDrawList(gUi[0]);
        Game_SetNextMode(0, 1);
        r = 1;
    }
    return r;
}
