#include "common.h"
#include "m2c.h"

s32 func_8004FD80(s32 *);                           /* extern */
s32 func_80050910(s32);                             /* extern */
M2C_UNK func_80063564(M2C_UNK, M2C_UNK);            /* extern */
extern s32 D_800964D4;
extern u8 D_8009734C;
extern u8 D_8009734D;
extern void *gGame;
extern s16 D_8011EEEA;

s32 func_8003C480(void *arg0) {
    s32 sp10;
    M2C_UNK var_a0;

    if (func_8004FD80(&sp10) & 0xFF) {
        if (func_80050910(sp10) & 0xFF) {
            if (D_8009734C != 0) {
                var_a0 = 9;
            } else {
                var_a0 = 6;
                if (D_8009734D != 0) {
                    var_a0 = 0xB;
                }
            }
            func_80063564(var_a0, 1);
        } else if (M2C_FIELD(gGame, s16 *, 0x10FC) == D_8011EEEA) {
            goto block_9;
        }
    } else {
block_9:
        func_80063564(8, 1);
        M2C_FIELD(arg0, s32 *, 0x10) = 0;
    }
    D_800964D4 = 5;
    return 1;
}
