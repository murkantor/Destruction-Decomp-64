#include "common.h"
#include "m2c.h"

static inline void guMtxIdentF_inline(f32 mf[4][4]) {
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (i == j) {
                mf[i][j] = 1.0f;
            } else {
                mf[i][j] = 0.0f;
            }
        }
    }
}

static inline void guMtxF2L_inline(f32 mf[4][4], s32 *m) {
    int i, j;
    int e1, e2;
    int *ai, *af;

    ai = m;
    af = m + 8;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            e1 = (s32)(mf[i][j * 2] * 65536.0f);
            e2 = (s32)(mf[i][j * 2 + 1] * 65536.0f);
            *(ai++) = (e1 & 0xFFFF0000) | ((e2 >> 16) & 0xFFFF);
            *(af++) = ((e1 << 16) & 0xFFFF0000) | (e2 & 0xFFFF);
        }
    }
}

void guMtxIdent(s32 *m) {
    f32 mf[4][4];

    guMtxIdentF_inline(mf);
    guMtxF2L_inline(mf, m);
}
