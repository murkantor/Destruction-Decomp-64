#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10A8];
    u64 x10A8;
    u8 pad10B0[0x4C];
    s16 x10FC;
} GameState;
extern GameState *gGame;
extern s16 D_8011EEEA;
extern s32 gUi;
void Ui_RevealFree(GameState *, u64);
void Ui_ClearDrawList(s32);
void Game_SetNextMode(s32, s32);

s32 Logo_ThqUpdate(void) {
    s32 ret = 0;
    u64 lim;

    if (gGame->x10FC != D_8011EEEA) {
        return 0;
    }
    lim = 4000000000ULL;
    if (gGame->x10A8 >= lim) {
        Ui_RevealFree(gGame, lim);
        Ui_ClearDrawList(gUi);
        Game_SetNextMode(1, 1);
        ret = 1;
    }
    return ret;
}
