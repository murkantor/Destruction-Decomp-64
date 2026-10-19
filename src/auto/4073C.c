#include "common.h"
#include "m2c.h"

extern s32 D_800958A8;
void func_80063564(s32, s32);

s32 func_8003FB3C(u8 *obj) {
    func_80063564(0x13, 1);
    *(s32 *)(obj + 0x10) = D_800958A8;
    return 1;
}
