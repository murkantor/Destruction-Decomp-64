#include "common.h"
#include "m2c.h"

extern s32 D_800958A8;
void Game_SetNextMode(s32, s32);

s32 PlayerMenu_Back(u8 *obj) {
    Game_SetNextMode(0x13, 1);
    *(s32 *)(obj + 0x10) = D_800958A8;
    return 1;
}
