#include "common.h"
#include "m2c.h"

extern s64 D_800B70E0[];

f64 _log_frac(f64 a) {
    s64 y = 0;
    s64 x = (u64)(a * 1152921504606846976.0);
    s32 i;

    for (i = 1; i < 54; i++) {
        s64 t = x + (x >> i);

        if (t < 0x1000000000000000LL) {
            x = t;
            y -= D_800B70E0[i];
        }
    }
    return (f64)y / 1152921504606846976.0;
}
