#include "common.h"
#include "m2c.h"

f64 __kernel_standard(s32, char *, f64, f64, f64);
f64 atan(f64);

f64 atan2(f64 y, f64 x) {
    f64 r;

    if ((x == 0.0) & (y == 0.0)) {
        return __kernel_standard(1, "atan2", x, y, 0.0);
    }
    if (x == 0.0) {
        if (y > 0.0) {
            return 1.5707963267948966;
        }
        return -1.5707963267948966;
    }
    r = atan(y / x);
    if (x > 0.0) {
        return r;
    }
    if (y == 0.0) {
        return 3.141592653589793;
    }
    if (y > 0.0) {
        return r + 3.141592653589793;
    }
    return r - 3.141592653589793;
}
