#include "common.h"
#include "m2c.h"

extern s32 D_800958A4;
void func_80063564(s32, s32);

s32 func_800407AC(u8 *obj) {
    func_80063564(0x16, 1);
    *(s32 *)(obj + 0x10) = D_800958A4 - 2;
    return 1;
}
