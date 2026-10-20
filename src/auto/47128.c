#include "common.h"
#include "m2c.h"

extern s32 D_800958A8;
extern u32 D_800958AC;
extern u32 D_800958B4;
void func_80063380(s32);
void Game_SetNextMode(s32, s32);

s32 func_80046528(void) {
    func_80063380(1);
    if (D_800958A8 == 2) {
        Game_SetNextMode(0x2A, 1);
    } else if (D_800958A8 == 3) {
        switch (D_800958AC) {
            case 1:
                Game_SetNextMode(0x2B, 1);
                break;
            case 0:
                Game_SetNextMode(0x22, 1);
                break;
            case 2:
                Game_SetNextMode(0x2C, 1);
                break;
            case 3:
                Game_SetNextMode(0x23, 1);
                break;
        }
    } else if (D_800958B4 < 4) {
        Game_SetNextMode(0x22, 1);
    } else {
        Game_SetNextMode(0x23, 1);
    }
    return 1;
}
