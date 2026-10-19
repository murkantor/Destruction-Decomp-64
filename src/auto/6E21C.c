#include "common.h"
#include "m2c.h"

s32 func_8006C120();                                /* extern */
s32 func_8006C160();                                /* extern */

s32 func_8006D61C(s32 arg0) {
    s32 temp_s0;

    if (arg0 != 0) {
        temp_s0 = func_8006C120();
        return arg0 & -(temp_s0 == func_8006C160());
    }
    return 0;
}
