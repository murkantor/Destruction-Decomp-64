#include "common.h"
#include "m2c.h"

s32 Math_SameSign(f32 arg0, f32 arg1) {
    f64 temp_f12;
    s32 var_v0;

    temp_f12 = (f64) arg0;
    if (!(temp_f12 < 0.0) || (var_v0 = 1, !((f64) arg1 < 0.0))) {
        var_v0 = 0;
        if (temp_f12 > 0.0) {
            var_v0 = 1;
            if (!((f64) arg1 > 0.0)) {
                var_v0 = 0;
            }
        }
    }
    return var_v0;
}
