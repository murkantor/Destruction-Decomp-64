#include "common.h"
#include "m2c.h"

extern s32 D_800B0C30;
extern f32 D_800BD430[];
f32 sinf(f32);
f32 cosf(f32);

static inline s32 atan_find(f32 v) {
    s32 lo = 0;
    f32 *t = D_800BD430;
    s32 hi = 0x3FF;
    s32 mid;
    while (lo < hi) {
        mid = (lo + hi) / 2;
        if (t[mid] < v) {
            lo = mid + 1;
        } else if (v < t[mid]) {
            hi = mid - 1;
        } else {
            return mid;
        }
    }
    return lo;
}

f32 Math_Atan2(f32 a, f32 b) {
    s32 i;
    f32 na, nb;

    if (D_800B0C30 == 0) {
        for (i = 0; i < 0x400; i++) {
            f32 ang = (f32)i * 0.000766990393066406301;
            D_800BD430[i] = sinf(ang) / cosf(ang);
        }
        D_800B0C30 = 1;
    }
    if (b == 0.0f) {
        if (0.0f < a) {
            return 1.5707964f;
        }
        return -1.5707964f;
    }
    if (a == 0.0f) {
        if (0.0f < b) {
            return 0.0f;
        }
        return 3.1415927f;
    }
    if (0.0f < a) {
        if (0.0f < b) {
            if (a < b) {
                return atan_find(a / b) * 0.000766990393066406301;
            }
            return 1.5707963250000001 - atan_find(b / a) * 0.000766990393066406301;
        }
        {
        f32 nb2 = -b;
        if (a < nb2) {
            return 3.14159265000000021 - atan_find(a / nb2) * 0.000766990393066406301;
        }
        return atan_find(nb2 / a) * 0.000766990393066406301 + 1.5707963250000001;
        }
    }
    if (0.0f < b) {
        na = -a;
        if (na < b) {
            return -(atan_find(na / b) * 0.000766990393066406301);
        }
        return atan_find(b / na) * 0.000766990393066406301 + -1.5707963250000001;
    }
    {
    f32 nb3 = -b;
    f32 na3 = -a;
    if (na3 < nb3) {
        return atan_find(na3 / nb3) * 0.000766990393066406301 + -3.14159265000000021;
    }
    return -1.5707963250000001 - atan_find(nb3 / na3) * 0.000766990393066406301;
    }
}
