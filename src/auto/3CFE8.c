#include "common.h"
#include "m2c.h"

extern s32 D_800964D4;
extern u8 D_8009734C;
extern u8 D_8009734D;
u8 Pak_Open(s32 *);
u8 Pak_HandleError(s32);
void Game_SetNextMode(s32, s32);

s32 PakMenu_NoPakTick(void) {
    s32 h;

    if (D_800964D4 == 0) {
        if (Pak_Open(&h) && Pak_HandleError(h)) {
        if (D_8009734C) {
            Game_SetNextMode(9, 1);
        } else if (D_8009734D) {
            Game_SetNextMode(0xB, 1);
        } else {
            Game_SetNextMode(6, 1);
        }
            return 1;
        }
        D_800964D4 = 5;
    } else {
        D_800964D4--;
    }
    return 0;
}
