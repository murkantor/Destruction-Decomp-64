#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0xC];
    s16 mode;
    u8 padE[0x104A];
    u64 clock;
} GameState;
extern GameState *gGame;
extern u8 D_80097335;
extern u64 D_80131298;
u64 Time_Get(u64 *);
void Game_SetNextMode(s32, s32);

s32 Records_Timeout(void) {
    s32 ret = 0;
    s32 n;

    if (gGame->mode == 1) {
        n = D_80097335 ? 39 : 25;
    } else {
        n = (gGame->mode == 2) ? 31 : 25;
    }
    if (n == 25) {
        if (Time_Get(&gGame->clock) >= D_80131298) {
            Game_SetNextMode(25, 1);
            ret = 1;
        }
    }
    return ret;
}
