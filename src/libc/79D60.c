#include "common.h"
#include "m2c.h"

extern s64 D_800B6DE0[];
extern f64 D_800BC780;

void _atan_cordic(s64 x0, s64 y, f64 *out) {
    s64 x = x0;
    s64 z = 0;
    s64 nx;
    s32 i;

    for (i = 0; i < 52; i++) {
        if (y >= 0) {
            nx = x + (y >> i);
            y -= x >> i;
            z += D_800B6DE0[i];
        } else {
            nx = x - (y >> i);
            y += x >> i;
            z -= D_800B6DE0[i];
        }
        x = nx;
    }
    *out = z / D_800BC780;
}
