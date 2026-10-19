#include "common.h"
#include "m2c.h"

extern u32 D_800958B4;
void func_80063564(s32, s32);

s32 func_8004D284(void) {
    if (D_800958B4 < 4) {
        func_80063564(0x22, 1);
    } else {
        func_80063564(0x23, 1);
    }
    return 1;
}
