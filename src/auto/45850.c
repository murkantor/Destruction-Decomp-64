#include "common.h"
#include "m2c.h"

M2C_UNK Game_SetNextMode(M2C_UNK, M2C_UNK);            /* extern */
extern s32 D_80125C38;

s32 Countdown_Pause(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x10) = 0;
    D_80125C38 = 0x1B;
    Game_SetNextMode(0x1F, 1);
    return 1;
}
