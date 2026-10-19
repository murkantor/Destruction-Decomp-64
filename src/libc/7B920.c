#include "common.h"
#include "m2c.h"

double modf(double value, double *iptr) {
    s32 e;
    s32 i;

    *iptr = value;
    e = ((*(u32 *)iptr >> 20) & 0x7FF) - 0x3FF;
    if (e < 0) {
        ((u32 *)iptr)[0] = 0;
        ((u32 *)iptr)[1] = 0;
        return value;
    }
    if (e >= 52) {
        *iptr = value;
        return 0.0;
    }
    e += 12;
    i = e >> 3;
    ((u8 *)iptr)[i] &= 0xFF << (8 - (e & 7));
    while (i < 7) {
        i++;
        ((u8 *)iptr)[i] = 0;
    }
    return value - *iptr;
}
