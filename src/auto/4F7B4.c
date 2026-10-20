#include "common.h"
#include "m2c.h"

extern u8 D_80097335;
extern s32 D_800958A8;
void Game_SetNextMode(s32, s32);

s32 func_8004EBB4(u8 *obj) {
    if (D_80097335) {
        *(s32 *)(obj + 0x10) = 0;
        Game_SetNextMode(0x26, 1);
    } else {
        *(s32 *)(obj + 0x10) = D_800958A8;
        Game_SetNextMode(0x13, 1);
    }
    return 1;
}
