#include "common.h"
#include "m2c.h"

f64 __kernel_standard(s32, char *, f64, f64, f64);
f64 asin(f64);

f64 acos(f64 x) {
    if ((x > 1.0) | (x < -1.0)) {
        return __kernel_standard(1, "acos", x, 0.0, 0.0);
    }
    return 1.57079632679489656 - asin(x);
}
