#include "common.h"
#include "m2c.h"

void func_80079160(u64, u64, f64 *);

f64 atan(f64 x) {
    s32 sign;
    u64 num;
    u64 den;
    f64 r;

    if (x < 0.0) {
        x = -x;
        sign = -1;
    } else {
        sign = 1;
    }
    if (x > 1e-07) {
        if (x >= 1.0) {
            den = 0x1000000000000000ULL;
            num = (u64)(1.0 / x * 1152921504606846976.0);
        } else {
            num = 0x1000000000000000ULL;
            den = (u64)(x * 1152921504606846976.0);
        }
        func_80079160(num, den, &r);
    } else {
        r = x;
    }
    return sign < 0 ? -r : r;
}
