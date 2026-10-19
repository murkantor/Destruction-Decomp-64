#include "common.h"
#include "m2c.h"

static inline f64 _frexpf(f64 value, s32 *eptr) {
    f64 absvalue;

    *eptr = 0;
    if (value == 0.0) {
        return value;
    }
    absvalue = (value > 0.0) ? value : -value;
    for (; absvalue >= 1.0; absvalue *= 0.5) {
        ++*eptr;
    }
    for (; absvalue < 0.5; absvalue += absvalue) {
        --*eptr;
    }
    return (value > 0.0) ? absvalue : -absvalue;
}

s16 _getRate(f64 vol, f64 tgt, s32 count, u16 *ratel) {
    s16 s;
    f64 invn = 1.0 / count, eps, a, fs, mant;
    s32 i_invn, ex, indx;

    if (count == 0) {
        if (tgt >= vol) {
            *ratel = 0xFFFF;
            return 0x7FFF;
        } else {
            *ratel = 0;
            return 0;
        }
    }

    if (tgt < 1.0) {
        tgt = 1.0;
    }
    if (vol <= 0) {
        vol = 1;
    }

    {
        f64 logtab[] = {
            -0.912537, -0.752072, -0.607683, -0.476438,
            -0.356144, -0.245112, -0.142019, -0.045804,
        };

        a = tgt / vol;
        i_invn = (s32)(invn * (1 << 30));
        mant = _frexpf(a, &ex);
        indx = (s32)(mant * 16);
        fs = (logtab[indx - 8] + ex) * 0.6931471805599453 / (1 << 30);
        eps = 1.0;
        fs = 1.0 + fs;
        while (i_invn) {
            if (i_invn & 1) {
                eps *= fs;
            }
            i_invn >>= 1;
            fs *= fs;
        }
        eps *= eps;
        eps *= eps;
        eps *= eps;
        s = (s16)eps;
        *ratel = (s32)((eps - (f32)s) * 65535);
        return s;
    }
}
