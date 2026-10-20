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
void func_8005DC68(s32);
void func_80063564(s32, s32);

s32 func_8003B410(void) {
    s32 ret = 0;
    u64 lim;

    if (gGame->x10FC != D_8011EEEA) {
        return 0;
    }
    lim = 4000000000ULL;
    if (gGame->x10A8 >= lim) {
        Ui_RevealFree(gGame, lim);
        func_8005DC68(gUi);
        func_80063564(1, 1);
        ret = 1;
    }
    return ret;
}
