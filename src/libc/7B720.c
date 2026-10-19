#include "common.h"
#include "m2c.h"

f64 __kernel_standard(s32, char *, f64, f64, f64);
f64 modf(f64, f64 *);
f64 func_8007AA20(f64);

f64 func_8007AB20(f64 x) {
    union { f64 d; u64 ll; } u;
    f64 ip;
    f64 f;
    u32 e;
    u32 *p = (u32 *)&u;

    if (x >= 710.0) {
        goto overflow;
    }
    if (x <= -710.0) {
        return 0.0;
    }
    f = modf(x * 1.4426950408889634, &ip) * 0.6931471805599453;
    if (f > 0.0) {
        u.d = func_8007AA20(f);
    } else if (f < 0.0) {
        u.d = 1.0 / func_8007AA20(-f);
    } else {
        u.d = 1.0;
    }
    e = ((*p >> 20) & 0x7FF) + (s32)ip;
    if (e >= 0x7FF) {
overflow:
        u.ll = 0x7FF0000000000000LL;
        return __kernel_standard(3, "exp", x, 0.0, u.d);
    }
    if (e != 0) {
        *p = (*p & 0x800FFFFF) | (e << 20);
        return u.d;
    }
    return 0.0;
}
