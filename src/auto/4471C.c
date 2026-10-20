#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10A8]; u64 x10A8; } Game;
extern Game *gGame;
extern s32 D_80097358[];
extern u8 D_80097354;
extern s32 D_80096EB8;
extern u8 D_80096ED0[];
void func_80063380(s32);
void Game_SetNextMode(s32, s32);

s32 Lights_Cycle(void) {
    u64 lim = 30000000000ULL;
    u8 ret = 0;

    if (gGame->x10A8 >= lim) {
        ret = 1;
        switch (D_80097358[0]) {
        case 0:
            D_80097354 = 0;
            D_80097358[0] = 1;
            func_80063380(2);
            break;
        case 1:
            D_80097354 = ret;
            D_80097358[0] = 2;
            func_80063380(2);
            break;
        case 2:
            D_80097354 = 0;
            D_80097358[0] = 3;
            func_80063380(1);
            Game_SetNextMode(0x27, 1);
            break;
        case 3:
            D_80097354 = ret;
            D_80097358[0] = 0;
            func_80063380(2);
            if (++D_80096EB8 == 2) {
                for (D_80096EB8 = 0; D_80096EB8 < 12; D_80096EB8++) {
                    if (D_80096EB8 < 4) {
                        D_80096ED0[D_80096EB8] = 1;
                    } else {
                        D_80096ED0[D_80096EB8] = 0;
                    }
                }
                D_80096EB8 = 0;
            }
            break;
        }
    }
    return ret;
}
