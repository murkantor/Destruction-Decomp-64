#include "common.h"
#include "m2c.h"

f64 __kernel_standard();
f64 frexp(f64, s32 *);
f64 _log_frac(f64);
extern char D_800BC7F0[];
extern f64 D_800BC7F8;
extern f64 D_800BC800;

f64 log(f64 x) {
    f64 r;
    s32 e;

    if (x < 0.0) {
        return __kernel_standard(1, D_800BC7F0, x, 0.0, 0.0);
    }
    if (x == 0.0) {
        *(u64 *)&r = 0xFFF0000000000000ULL;
        return __kernel_standard(3, D_800BC7F0, x, 0.0, r);
    }
    if (x == D_800BC7F8) {
        return 0.0;
    }
    r = _log_frac(frexp(x, &e));
    r = e * D_800BC800 + r;
    return r;
}
