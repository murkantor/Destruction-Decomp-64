#include "common.h"
#include "m2c.h"

extern void *gGame;

void Game_SetMode(s16 arg0, s32 arg1) {
    if (arg1 & 0xFF) {
        M2C_FIELD(gGame, s8 *, 0x10FE) = 1;
    } else {
        M2C_FIELD(gGame, s8 *, 0x10FE) = 0;
    }
    M2C_FIELD(gGame, s16 *, 0x10FC) = arg0;
}
