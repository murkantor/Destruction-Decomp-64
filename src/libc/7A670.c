#include "common.h"
#include "m2c.h"

f64 __kernel_standard(s32, char *, f64, f64, f64);
f64 func_80079750(f64);
f64 func_8007AB20(f64);

f64 pow(f64 x, f64 y) {
    s32 sign = 1;
    s32 n;

    if ((x == 1.0) | (y == 1.0)) {
        return x;
    }
    if (x == 0.0) {
        if (y <= 0.0) {
            return __kernel_standard(2, "pow", x, y, 0.0);
        }
        return 0.0;
    } else if (x < 0.0) {
        n = (s32)y;
        if (y != (f64)n) {
            return __kernel_standard(2, "pow", x, y, 0.0);
        }
        if (n & 1) {
            sign = -1;
        }
        x = __builtin_fabs(x);
    }
    return func_8007AB20(func_80079750(x) * y) * sign;
}
