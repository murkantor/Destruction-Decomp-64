#include "common.h"
#include "m2c.h"

M2C_UNK func_800540C0();                            /* extern */
extern u32 D_80125C90;

s32 func_80054B48(void) {
    func_800540C0();
    return (((u32) D_80125C90 >> 0xC) & 1) ^ 1;
}

M2C_UNK DebugMenu_Draw();                            /* extern */

void func_80054B78(void) {
    DebugMenu_Draw();
}
