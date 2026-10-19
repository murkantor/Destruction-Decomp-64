#include "common.h"
#include "m2c.h"

void guMtxIdentF(f32 mf[4][4]) {
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
