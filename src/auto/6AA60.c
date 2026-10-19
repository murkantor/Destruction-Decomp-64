#include "common.h"
#include "m2c.h"

void guMtxF2L(f32 mf[4][4], void *m);
float sinf(float);
float cosf(float);
extern f32 D_800B5680;

inline void guPositionF(f32 mf[4][4], f32 r, f32 p, f32 h, f32 s, f32 x, f32 y, f32 z) {
    f32 sinr, sinp, sinh, cosr, cosp, cosh;

    r *= D_800B5680;
    p *= D_800B5680;
    h *= D_800B5680;
    sinr = sinf(r);
    cosr = cosf(r);
    sinp = sinf(p);
    cosp = cosf(p);
    sinh = sinf(h);
    cosh = cosf(h);
    mf[0][0] = (cosp * cosh) * s;
    mf[0][1] = (cosp * sinh) * s;
    mf[0][2] = (-sinp) * s;
    mf[0][3] = 0.0;
    mf[1][0] = (sinr * sinp * cosh - cosr * sinh) * s;
    mf[1][1] = (sinr * sinp * sinh + cosr * cosh) * s;
    mf[1][2] = (sinr * cosp) * s;
    mf[1][3] = 0.0;
    mf[2][0] = (cosr * sinp * cosh + sinr * sinh) * s;
    mf[2][1] = (cosr * sinp * sinh - sinr * cosh) * s;
    mf[2][2] = (cosr * cosp) * s;
    mf[2][3] = 0.0;
    mf[3][0] = x;
    mf[3][1] = y;
    mf[3][2] = z;
    mf[3][3] = 1.0;
}


void guPosition(void *m, f32 r, f32 p, f32 h, f32 s, f32 x, f32 y, f32 z) {
    f32 mf[4][4];

    guPositionF(mf, r, p, h, s, x, y, z);
    guMtxF2L(mf, m);
}
