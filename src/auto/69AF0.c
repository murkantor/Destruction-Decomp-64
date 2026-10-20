#include "common.h"
#include "m2c.h"

void guMtxCatF(f32 a[4][4], f32 b[4][4], f32 out[4][4]) {
    f32 tmp[4][4];
    s32 i;
    s32 j;
    s32 k;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmp[i][j] = 0.0f;
            for (k = 0; k < 4; k++) {
                tmp[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            out[i][j] = tmp[i][j];
        }
    }
}
