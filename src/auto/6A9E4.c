#include "common.h"
#include "m2c.h"

void guMtxIdentF(f32 mf[4][4]);
void guMtxF2L(f32 mf[4][4], void *m);

static inline void guTranslateF(f32 mf[4][4], f32 x, f32 y, f32 z) {
    guMtxIdentF(mf);
    mf[3][0] = x;
    mf[3][1] = y;
    mf[3][2] = z;
}

void guTranslate(void *m, f32 x, f32 y, f32 z) {
    f32 mf[4][4];

    guTranslateF(mf, x, y, z);
    guMtxF2L(mf, m);
}
