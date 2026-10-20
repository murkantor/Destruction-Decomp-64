#include "common.h"
#include "m2c.h"

f64 _frexpf(f64 x, s32 *exp) {
    f64 y;

    *exp = 0;
    if (x == 0.0) {
        return x;
    }
    y = (x > 0.0) ? x : -x;
    while (y >= 1.0) {
        y *= 0.5;
        (*exp)++;
    }
    while (y < 0.5) {
        y += y;
        (*exp)--;
    }
    return (x > 0.0) ? y : -y;
}
