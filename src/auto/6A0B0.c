#include "common.h"
#include "m2c.h"

void guMtxIdentF(f32 mf[4][4]);
void guMtxF2L(f32 mf[4][4], void *m);
void guNormalize(f32 *, f32 *, f32 *);
float sinf(float);
float cosf(float);
extern f32 D_800B4E60;

inline void guRotateF(f32 mf[4][4], f32 a, f32 x, f32 y, f32 z) {
    f32 sine, cosine, ab, bc, ca, t;

    guNormalize(&x, &y, &z);
    a *= D_800B4E60;
    sine = sinf(a);
    cosine = cosf(a);
    t = (1 - cosine);
    ab = x * y * t;
    bc = y * z * t;
    ca = z * x * t;
    guMtxIdentF(mf);
    t = x * x;
    mf[0][0] = t + cosine * (1 - t);
    mf[2][1] = bc - x * sine;
    mf[1][2] = bc + x * sine;
    t = y * y;
    mf[1][1] = t + cosine * (1 - t);
    mf[2][0] = ca + y * sine;
    mf[0][2] = ca - y * sine;
    t = z * z;
    mf[2][2] = t + cosine * (1 - t);
    mf[1][0] = ab - z * sine;
    mf[0][1] = ab + z * sine;
}


void guRotate(void *m, f32 a, f32 x, f32 y, f32 z) {
    f32 mf[4][4];

    guRotateF(mf, a, x, y, z);
    guMtxF2L(mf, m);
}
