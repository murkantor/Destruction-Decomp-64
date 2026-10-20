#include "common.h"
#include "m2c.h"

M2C_UNK DebugMenu_HandleInput();                            /* extern */
extern u32 D_80125C90;

s32 DebugMenu_Update(void) {
    DebugMenu_HandleInput();
    return (((u32) D_80125C90 >> 0xC) & 1) ^ 1;
}

M2C_UNK DebugMenu_Draw();                            /* extern */

void func_80054B78(void *arg0) {
    DebugMenu_Draw(arg0);
}
