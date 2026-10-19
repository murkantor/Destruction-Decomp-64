#include "common.h"
#include "m2c.h"

void guMtxIdentF(f32 mf[4][4]);
void guMtxF2L(f32 mf[4][4], void *m);
float sinf(float);
float cosf(float);
extern f32 D_800B4E70;

inline void guRotateRPYF(f32 mf[4][4], f32 r, f32 p, f32 h) {
    f32 sinr, sinp, sinh, cosr, cosp, cosh;

    r *= D_800B4E70;
    p *= D_800B4E70;
    h *= D_800B4E70;
    sinr = sinf(r);
    cosr = cosf(r);
    sinp = sinf(p);
    cosp = cosf(p);
    sinh = sinf(h);
    cosh = cosf(h);
    guMtxIdentF(mf);
    mf[0][0] = cosp * cosh;
    mf[0][1] = cosp * sinh;
    mf[0][2] = -sinp;
    mf[1][0] = sinr * sinp * cosh - cosr * sinh;
    mf[1][1] = sinr * sinp * sinh + cosr * cosh;
    mf[1][2] = sinr * cosp;
    mf[2][0] = cosr * sinp * cosh + sinr * sinh;
    mf[2][1] = cosr * sinp * sinh - sinr * cosh;
    mf[2][2] = cosr * cosp;
}


void guRotateRPY(void *m, f32 r, f32 p, f32 h) {
    f32 mf[4][4];

    guRotateRPYF(mf, r, p, h);
    guMtxF2L(mf, m);
}
