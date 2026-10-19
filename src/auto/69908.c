#include "common.h"
#include "m2c.h"

typedef struct { s32 m[4][4]; } Mtx;
#define FIX32TOF(x) ((float)(x) * (1.0f / (float)0x00010000))

void guMtxL2F(float mf[4][4], Mtx *m) {
    int i, j;
    unsigned int e1, e2;
    unsigned int *ai, *af;
    int q1, q2;

    ai = (unsigned int *)&m->m[0][0];
    af = (unsigned int *)&m->m[2][0];

    for (i = 0; i < 4; i++)
        for (j = 0; j < 2; j++) {
            e1 = (*ai & 0xffff0000) | ((*af >> 16) & 0xffff);
            e2 = ((*(ai++) << 16) & 0xffff0000) | (*(af++) & 0xffff);
            q1 = *((int *)&e1);
            q2 = *((int *)&e2);
            mf[i][j * 2] = FIX32TOF(q1);
            mf[i][j * 2 + 1] = FIX32TOF(q2);
        }
}
