#include "common.h"
#include "m2c.h"

extern s32 D_800958A4;
void Game_SetNextMode(s32, s32);

s32 GameTypeMenu_Back(u8 *obj) {
    Game_SetNextMode(0x16, 1);
    *(s32 *)(obj + 0x10) = D_800958A4 - 2;
    return 1;
}
