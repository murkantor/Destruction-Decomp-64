#include "common.h"
#include "m2c.h"

typedef union {
    struct {
        u32 hi;
        u32 lo;
    } word;
    f64 d;
} du;

typedef union {
    u32 i;
    f32 f;
} fu;

extern fu D_800BC250;

static const du P[] = {
    { { 0x3FF00000, 0x00000000 } },
    { { 0xBFC55554, 0xBC83656D } },
    { { 0x3F8110ED, 0x3804C2A0 } },
    { { 0xBF29F6FF, 0xEEA56814 } },
    { { 0x3EC5DBDF, 0x0E314BFE } },
};

static const du rpi = { { 0x3FD45F30, 0x6DC9C883 } };

static const du pihi = { { 0x400921FB, 0x50000000 } };

static const du pilo = { { 0x3E6110B4, 0x611A6263 } };

static const fu zero = { 0x00000000 };

#define ROUND(d) (s32)(((d) >= 0.0) ? ((d) + 0.5) : ((d) - 0.5))

f32 cosf(f32 x) {
    f64 dx;
    f64 xsq;
    f64 poly;
    f64 dn;
    s32 n;
    f64 result;
    s32 ix;
    s32 xpt;

    ix = *(s32 *)&x;
    xpt = (ix >> 22) & 0x1FF;

    if (xpt < 0x136) {
        dx = (x > 0.0f) ? x : -x;

        dn = dx * rpi.d + .5;
        n = ROUND(dn);
        dn = n;

        dn -= .5;

        dx -= dn * pihi.d;
        dx -= dn * pilo.d;

        xsq = dx * dx;

        poly = ((P[4].d * xsq + P[3].d) * xsq + P[2].d) * xsq + P[1].d;

        result = dx + (dx * xsq) * poly;

        if ((n & 1) == 0) {
            return (f32)result;
        }
        return -(f32)result;
    }

    if (x != x) {
        return D_800BC250.f;
    }

    return zero.f;
}
