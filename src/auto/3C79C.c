#include "common.h"
#include "m2c.h"

extern s32 D_80097350;
void func_80063564(s32, s32);

s32 func_8003BB9C(void) {
    s32 ret;

    if (D_80097350 != 0) {
        func_80063564(4, 1);
        ret = 1;
    } else {
        ret = 0;
    }
    return ret;
}
