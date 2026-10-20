#include "common.h"
#include "m2c.h"

extern s32 D_80097350;
void Game_SetNextMode(s32, s32);

s32 SaveMenu_Select(void) {
    s32 ret;

    if (D_80097350 != 0) {
        Game_SetNextMode(4, 1);
        ret = 1;
    } else {
        ret = 0;
    }
    return ret;
}
