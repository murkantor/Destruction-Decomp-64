#include "common.h"
#include "m2c.h"

M2C_UNK DebugMenu_HandleInput();                            /* extern */
M2C_UNK DebugMenu_DrawPage(s32);                         /* extern */
extern M2C_UNK D_80125C90;

M2C_UNK *DebugMenu_Step(s32 arg0) {
    M2C_UNK *var_s0;
    s32 var_s1;

    var_s1 = arg0;
    DebugMenu_HandleInput();
    var_s0 = &D_80125C90;
    var_s0 = D_80125C90;
    var_s0 = (M2C_UNK *) ((u32) var_s0 >> 0xC);
    var_s0 = (M2C_UNK *) ((s32) var_s0 & 1);
    var_s0 = (M2C_UNK *) (var_s0 == NULL);
    DebugMenu_DrawPage(var_s1);
    return var_s0;
}
