#include "common.h"
#include "m2c.h"

void guMtxIdentF(f32 mf[4][4]);
void guMtxF2L(f32 mf[4][4], void *m);

static inline void guScaleF(f32 mf[4][4], f32 x, f32 y, f32 z) {
    guMtxIdentF(mf);
    mf[0][0] = x;
    mf[1][1] = y;
    mf[2][2] = z;
    mf[3][3] = 1;
}

void guScale(void *m, f32 x, f32 y, f32 z) {
    f32 mf[4][4];

    guScaleF(mf, x, y, z);
    guMtxF2L(mf, m);
}
