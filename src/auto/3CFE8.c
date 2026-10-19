#include "common.h"
#include "m2c.h"

extern s32 D_800964D4;
extern u8 D_8009734C;
extern u8 D_8009734D;
u8 func_8004FD80(s32 *);
u8 func_80050910(s32);
void func_80063564(s32, s32);

s32 func_8003C3E8(void) {
    s32 h;

    if (D_800964D4 == 0) {
        if (func_8004FD80(&h) && func_80050910(h)) {
        if (D_8009734C) {
            func_80063564(9, 1);
        } else if (D_8009734D) {
            func_80063564(0xB, 1);
        } else {
            func_80063564(6, 1);
        }
            return 1;
        }
        D_800964D4 = 5;
    } else {
        D_800964D4--;
    }
    return 0;
}
