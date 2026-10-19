#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
extern f64 fabs(f64);
#pragma intrinsic(fabs)

static inline void SinCos_(f32 x, f32 *s, f32 *c) {
    f32 x2;

    if (x > 3.14159265) {
        x = x - (s32)((x + 3.14159265) * 0.1591549) * 6.283185;
    } else if (x < -3.14159265) {
        x = x + (s32)((3.14159265 - x) * 0.1591549) * 6.283185;
    }
    x2 = x * x;
    *s = x * (x2 * (x2 * (x2 * (x2 * (2.755732e-06 - x2 * 2.505211e-08) + -0.0001984127) + 0.008333333) + -0.1666667) + 1.0);
    *c = x2 * (x2 * (x2 * (x2 * (x2 * (x2 * 2.087675e-09 + -2.755732e-07) + 2.480159e-05) + -0.001388889) + 0.04166667) + -0.5) + 1.0;
}

void func_8003A568(f32 m[3][3], Vec3f *axis, f32 angle) {
    f32 sc[2];
    f32 x, y, z, w;
    f32 x2, y2, z2, w2;
    f32 xx, yy, zz, xy, xz, yz, wx, wy, wz;
    f32 m00, m01, m02, m10, m11, m12, m20, m21, m22;
    f32 a, b, c;
    s32 i;

    if (fabs(angle) < 1e-06) {
        return;
    }
    SinCos_(angle * 0.5, &sc[0], &sc[1]);
    x = sc[0] * axis->x;
    y = sc[0] * axis->y;
    z = sc[0] * axis->z;
    w = sc[1];
    x2 = x + x;
    w2 = w + w;
    y2 = y + y;
    z2 = z + z;
    xy = x2 * y;
    zz = z2 * z;
    wz = w2 * z;
    xx = x2 * x;
    yy = y2 * y;
    yz = y2 * z;
    xz = x2 * z;
    wx = w2 * x;
    wy = w2 * y;
    m10 = xy + wz;
    m01 = xy - wz;
    m20 = xz - wy;
    m02 = xz + wy;
    m21 = yz + wx;
    m12 = yz - wx;
    m00 = 1.0f - yy - zz;
    m11 = 1.0f - xx - zz;
    m22 = 1.0f - xx - yy;
    for (i = 0; i < 3; i++) {
        a = m[i][0];
        b = m[i][1];
        c = m[i][2];
        m[i][0] = m00 * a + m01 * b + m02 * c;
        m[i][1] = m10 * a + m11 * b + m12 * c;
        m[i][2] = m20 * a + m21 * b + m22 * c;
    }
}
