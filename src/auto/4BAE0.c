#include "common.h"
#include "m2c.h"

M2C_UNK Pause_Return(s32, s32);                     /* extern */
M2C_UNK func_80063380(M2C_UNK);                     /* extern */
M2C_UNK Game_SetNextMode(M2C_UNK, M2C_UNK);            /* extern */
extern s8 D_800958A0;
extern u8 D_80097335;
extern void *gGame;
extern void *D_8011EEF8;

s32 Records_Back(s32 arg0) {
    s16 temp_v1;
    s32 var_v1;

    temp_v1 = M2C_FIELD(gGame, s16 *, 0xC);
    if (temp_v1 == 1) {
        var_v1 = 0x19;
        if (D_80097335 != 0) {
            var_v1 = 0x27;
        }
    } else {
        var_v1 = (-(temp_v1 == 2) & 0x1F) | 0x19;
    }
    if (var_v1 == 0x19) {
        M2C_FIELD(D_8011EEF8, s8 *, 0x530) = 0;
        D_800958A0 = 1;
        func_80063380(1);
        Game_SetNextMode(0x13, 1);
    } else {
        Pause_Return(arg0, 3);
    }
    return 1;
}
