#include "common.h"
#include "m2c.h"

typedef union {
    f64 d;
    struct {
        u32 hi;
        u32 lo;
    } w;
} DU;

typedef union {
    f32 f;
    u32 w;
} FU;

f64 __kernel_standard(s32, char *, f64, f64, f64);

f64 sqrt(f64 x) {
    DU u;
    DU z;
    FU g;
    FU h;
    f32 f;
    s32 e;
    s32 i;
    s32 n;
    u32 a;

    z.d = x;
    u.d = x;
    if ((s32)u.w.hi < 0) {
        return __kernel_standard(1, "sqrt", x, 0.0, 0.0);
    }
    e = (u.w.hi >> 20) & 0x7FF;
    if (e == 0) {
        return 0.0;
    }
    e -= 0x3FF;
    u.w.hi = (u.w.hi & 0x800FFFFF) | 0x3FD00000;
    f = u.d;
    g.f = f;
    g.w = (g.w & 0xC07FFFFF) | 0x3D000000;
    if (e & 1) {
        e--;
        n = e >> 1;
        g.f = (f + g.f) + 0.4375;
        z.w.hi = (z.w.hi & 0x800FFFFF) | 0x3FE00000;
    } else {
        n = e >> 1;
        g.f = (f - g.f) + 0.28125;
        z.w.hi = (z.w.hi & 0x800FFFFF) | 0x3FD00000;
    }
    h.f = z.d;
    g.f = g.f + h.f / g.f;
    a = g.w & 0x3F800000;
    g.w = (g.w & 0xC07FFFFF) | ((a + 0xFF800000) & 0x3F800000);
    u.d = g.f;
    for (i = 0; i < 2; i++) {
        u.d = u.d + z.d / u.d;
        a = u.w.hi & 0x7FF00000;
        u.w.hi = (u.w.hi & 0x800FFFFF) | ((a + 0xFFF00000) & 0x7FF00000);
    }
    u.w.hi = (u.w.hi & 0x800FFFFF) | (((n + 0x3FF) & 0x7FF) << 20);
    return u.d;
}
