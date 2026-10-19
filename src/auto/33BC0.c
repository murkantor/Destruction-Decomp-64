#include "common.h"
#include "m2c.h"

s32 func_80032FC0(void *arg0, void *arg1) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f2_3;
    f32 var_f0;
    f32 var_f0_2;
    f32 var_f0_3;
    f32 var_f4;
    f32 var_f4_2;
    f32 var_f4_3;
    s32 var_v0;

    var_f4 = M2C_FIELD(arg0, f32 *, 0);
    temp_f0 = M2C_FIELD(arg1, f32 *, 0);
    var_v0 = 0;
    if (!(temp_f0 <= var_f4)) {
        var_f4 = temp_f0;
    }
    var_f0 = M2C_FIELD(arg0, f32 *, 0xC);
    temp_f2 = M2C_FIELD(arg1, f32 *, 0xC);
    if (!(var_f0 <= temp_f2)) {
        var_f0 = temp_f2;
    }
    if (var_f4 <= var_f0) {
        var_f4_2 = M2C_FIELD(arg0, f32 *, 4);
        temp_f0_2 = M2C_FIELD(arg1, f32 *, 4);
        if (!(temp_f0_2 <= var_f4_2)) {
            var_f4_2 = temp_f0_2;
        }
        var_f0_2 = M2C_FIELD(arg0, f32 *, 0x10);
        temp_f2_2 = M2C_FIELD(arg1, f32 *, 0x10);
        if (!(var_f0_2 <= temp_f2_2)) {
            var_f0_2 = temp_f2_2;
        }
        if (var_f4_2 <= var_f0_2) {
            var_f4_3 = M2C_FIELD(arg0, f32 *, 8);
            temp_f0_3 = M2C_FIELD(arg1, f32 *, 8);
            if (!(temp_f0_3 <= var_f4_3)) {
                var_f4_3 = temp_f0_3;
            }
            var_f0_3 = M2C_FIELD(arg0, f32 *, 0x14);
            temp_f2_3 = M2C_FIELD(arg1, f32 *, 0x14);
            if (!(var_f0_3 <= temp_f2_3)) {
                var_f0_3 = temp_f2_3;
            }
            if (var_f4_3 <= var_f0_3) {
                var_v0 = 1;
            }
        }
    }
    return var_v0;
}
