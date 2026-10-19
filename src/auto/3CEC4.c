#include "common.h"
#include "m2c.h"

s32 func_8004FD80(s32 *);                           /* extern */
s32 func_80050910(s32);                             /* extern */
M2C_UNK func_80063380(M2C_UNK);                     /* extern */
M2C_UNK func_80063564(M2C_UNK, M2C_UNK);            /* extern */
extern u8 D_80095898;
extern u8 D_8009734C;
extern u8 D_8009734D;
extern void *gGame;
extern s16 D_8011EEEA;

s32 func_8003C2C4(void) {
    s32 sp10;
    M2C_UNK var_a0;

    if (func_8004FD80(&sp10) & 0xFF) {
        if (func_80050910(sp10) & 0xFF) {
            if (M2C_FIELD(gGame, s16 *, 0xC) != 1) {
                func_80063380(1);
            }
            if (D_80095898 == 0) {
                var_a0 = 9;
                if (D_8009734C == 0) {
                    var_a0 = 0xB;
                    if (D_8009734D != 0) {

                    } else {
                        goto block_16;
                    }
                }
                goto block_17;
            }
            goto block_13;
        }
        if (M2C_FIELD(gGame, s16 *, 0x10FC) == D_8011EEEA) {
            if (M2C_FIELD(gGame, s16 *, 0xC) != 1) {
                func_80063380(1);
            }
            func_80063564(6, 1);
        }
        if (D_80095898 != 0) {
block_13:
            func_80063564(3, 1);
            D_80095898 = 0;
        }
    } else {
        var_a0 = 6;
        if (M2C_FIELD(gGame, s16 *, 0xC) != 1) {
            func_80063380(1);
block_16:
            var_a0 = 6;
        }
block_17:
        func_80063564(var_a0, 1);
    }
    return 1;
}
