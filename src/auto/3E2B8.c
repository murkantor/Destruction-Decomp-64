#include "common.h"
#include "m2c.h"

u8 Cont_AllPaksOk(void);
void func_80063380(s32);
void Game_SetNextMode(s32, s32);

s32 PakMenu_CheckPaks(void) {
    s32 id;

    if (Cont_AllPaksOk()) {
        func_80063380(2);
        id = 0x1A;
    } else {
        id = 0x10;
    }
    Game_SetNextMode(id, 1);
    return 1;
}
