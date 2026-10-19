#include "common.h"
#include "m2c.h"

f32 func_80073384(f32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    f32 var_f2;
    f32 var_f4;
    s32 var_a1;
    s32 var_v1;

    var_a1 = arg1 >> 3;
    if (var_a1 == 0) {
        return arg0;
    }
    var_f4 = 1.0f;
    var_v1 = 0;
    var_f2 = ((f32) (arg2 << 0x10) + (f32) (arg3 & 0xFFFF)) / 65536.0f;
loop_3:
    if (var_a1 & 1) {
        var_f4 *= var_f2;
    }
    var_a1 = var_a1 >> 1;
    if (var_a1 != 0) {
        var_v1 += 1;
        var_f2 *= var_f2;
        if (var_v1 >= 0x20) {

        } else {
            goto loop_3;
        }
    }
    return arg0 * var_f4;
}
