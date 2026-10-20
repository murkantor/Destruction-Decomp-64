#include "common.h"
#include "m2c.h"

#ifndef TARGET_PC /* the PC port uses the host's */
u8 *strcpy(u8 *arg0, u8 *arg1) {
    u8 *var_a1;
    u8 *var_v1;
    u8 temp_v0;
    u8 temp_v0_2;

    temp_v0 = *arg1;
    var_a1 = arg1 + 1;
    var_v1 = arg0 + 1;
    *arg0 = temp_v0;
    if (temp_v0 != 0) {
        do {
            temp_v0_2 = *var_a1;
            var_a1 += 1;
            *var_v1 = temp_v0_2;
            var_v1 += 1;
        } while (temp_v0_2 != 0);
    }
    return arg0;
}
#endif

M2C_UNK DebugMenu_SetCurrent();                            /* extern */
extern s32 D_800B04A0;

void DebugMenu_Reset(s32 arg0) {
    D_800B04A0 = 0;
    if (arg0 != 0) {
        DebugMenu_SetCurrent(arg0);
    }
}

void func_80053D58(void) {

}
