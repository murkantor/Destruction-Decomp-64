#include "common.h"
#include "m2c.h"

typedef struct { s32 v[9]; } Arr9;
typedef struct { s32 n; s32 v; } Pair;
extern Arr9 D_800B7DB0;
extern s32 D_8007E950[];
extern s32 D_8007EAE0[9][20];

void InitLookupTables(void) {
    Pair pairs[5];
    Arr9 vals = D_800B7DB0;
    s32 k, j, i, v, step;
    s32 m;

    k = 0;
    pairs[0].n = 50;
    pairs[0].v = 0;
    pairs[1].n = 15;
    pairs[1].v = 1;
    pairs[2].n = 15;
    pairs[2].v = -1;
    pairs[3].n = 10;
    pairs[3].v = 2;
    pairs[4].n = 10;
    pairs[4].v = -2;
    for (i = 0; i < 5; i++) {
        for (m = 0; m < pairs[i].n; m++) {
            D_8007E950[k++] = pairs[i].v;
        }
    }
    for (i = 0; i < 9; i++) {
        D_8007EAE0[i][9] = vals.v[i];
        D_8007EAE0[i][10] = vals.v[i];
        step = vals.v[i] / 9;
        v = vals.v[i];
        for (j = 8; j >= 0; j--) {
            v -= step;
            if (v == 0) v = 1;
            D_8007EAE0[i][j] = v;
        }
        step = (100 - vals.v[i]) / 9;
        v = vals.v[i];
        for (j = 11; j < 20; j++) {
            v += step;
            if (v == 100) v = 99;
            D_8007EAE0[i][j] = v;
        }
    }
}
