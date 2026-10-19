#include "common.h"
#include "m2c.h"

typedef union { struct { u32 hi; u32 lo; } word; f64 d; } du;
typedef union { u32 i; f32 f; } fu;

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

extern f32 D_800BC250;

#define ROUND(d) (s32)(((d) >= 0.0) ? ((d) + 0.5) : ((d) - 0.5))

f32 sinf(f32 x) {
    f64 dx, xsq, poly;
    f64 dn;
    s32 n;
    f64 result;
    s32 ix, xpt;

    ix = *(s32 *)&x;
    xpt = (ix >> 22);
    xpt &= 0x1FF;

    if (xpt < 0xFF) {
        dx = x;
        if (xpt >= 0xE6) {
            xsq = dx * dx;
            poly = ((P[4].d * xsq + P[3].d) * xsq + P[2].d) * xsq + P[1].d;
            result = dx + (dx * xsq) * poly;
            return (f32)result;
        }
        return x;
    }

    if (xpt < 0x136) {
        dx = x;
        dn = dx * rpi.d;
        n = ROUND(dn);
        dn = n;
        dx = dx - dn * pihi.d;
        dx = dx - dn * pilo.d;
        xsq = dx * dx;
        poly = ((P[4].d * xsq + P[3].d) * xsq + P[2].d) * xsq + P[1].d;
        result = dx + (dx * xsq) * poly;
        if ((n & 1) == 0) {
            return (f32)result;
        }
        return -(f32)result;
    }

    if (x != x) {
        return D_800BC250;
    }
    return zero.f;
}
