#include "common.h"
#include "m2c.h"

M2C_UNK func_80063380(M2C_UNK);                     /* extern */
M2C_UNK Game_SetNextMode(M2C_UNK, M2C_UNK);            /* extern */
extern u8 D_80097335;
extern u8 D_80097337;

s32 TTResults_Continue(void *arg0) {
    M2C_UNK var_a0;

    if (D_80097337 != 0) {
        M2C_FIELD(arg0, s32 *, 0x10) = 0;
        var_a0 = 0x2D;
        goto block_4;
    }
    var_a0 = 0x26;
    if (D_80097335 != 0) {
        M2C_FIELD(arg0, s32 *, 0x10) = 0;
block_4:
        Game_SetNextMode(var_a0, 1);
        return 1;
    }
    func_80063380(2);
    Game_SetNextMode(0x19, 1);
    M2C_FIELD(arg0, s32 *, 0x10) = 0;
    return 1;
}
